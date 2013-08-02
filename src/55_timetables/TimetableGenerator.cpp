
/** TimetableGenerator class implementation.
	@file TimetableGenerator.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "TimetableGenerator.h"

#include "JourneyPattern.hpp"
#include "LinePhysicalStop.hpp"
#include "PlacesList.hpp"
#include "StopPoint.hpp"
#include "SchedulesBasedService.h"
#include "StopArea.hpp"
#include "TimetableRowGroup.hpp"
#include "TimetableRowGroupItem.hpp"
#include "Env.h"
#include "CalendarModule.h"
#include "JourneyPatternCopy.hpp"
#include "PTUseRule.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace algorithm;
	using namespace calendar;
	using namespace graph;
	using namespace pt;
	using namespace util;
	
	namespace timetables
	{
		TimetableGenerator::TimetableGenerator(
			const Env& env
		):	_transferTimetableBefore(NULL),
			_transferTimetableAfter(NULL),
			_withContinuousServices(true),
			_env(env),
			_mergeColsWithSameTimetables(true),
			_compression(false)
		{}



		bool TimetableGenerator::_isJourneyPatternSelected(
			const JourneyPattern& journeyPattern
		) const	{
			const UseRule& useRule(
				journeyPattern.getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
			);
			if(	dynamic_cast<const PTUseRule*>(&useRule) &&
				static_cast<const PTUseRule&>(useRule).getForbiddenInTimetables()
			){
				return false;
			}

			bool result(false);
			bool passageOk(false);
			Path::Edges::const_iterator itEdge;
			const Path::Edges& edges(journeyPattern.getAllEdges());

			// JourneyPattern is authorized according to :
			//  - authorized lines
			//  - wayback filter
			//  - calendar filter
			if(	(	!_authorizedLines.empty() &&
					_authorizedLines.find(journeyPattern.getCommercialLine()) == _authorizedLines.end()
				) || (
					_wayBackFilter &&
					journeyPattern.getWayBack() != *_wayBackFilter
			)	){
				return false;
			}

			// Old method
			if(!_rows.empty())
			{
				// A1: JourneyPattern selection : there must be at least a departure stop of the line in the departures rows
				Rows::const_iterator itRow;
				for (itRow = _rows.begin(); itRow != _rows.end(); ++itRow)
				{
					if (!itRow->getIsDeparture() || !itRow->getPlace())
					{
						continue;
					}

					for (itEdge = edges.begin(); itEdge != edges.end(); ++itEdge)
					{
						if(	(*itEdge)->isDeparture() &&
							(itEdge+1) != edges.end() &&
							(*itEdge)->getHub() &&
							dynamic_cast<const StopArea*>((*itEdge)->getHub())->getKey() == itRow->getPlace()->getKey() &&
							(	_authorizedPhysicalStops.empty() ||
								_authorizedPhysicalStops.find(dynamic_cast<const StopPoint*>((*itEdge)->getFromVertex())) != _authorizedPhysicalStops.end()
							)
						){
							result = true;
							if (itRow->getIsArrival() || itRow->getCompulsory() == TimetableRow::PassageSuffisant)
								passageOk = true;
							break;
						}
					}
					if (result)
					{
						break;
					}
				}
				if (!result)
				{
					return false;
				}


				// A2: JourneyPattern selection : there must be at least an arrival stop of the line in the arrival rows, after the departure
				// this test is ignored if the timetable is defined only by a departure stop
				if(_rows.size() > 1)
				{
					result = false;
					const LineStop* departureLinestop(static_cast<const LineStop*>(*itEdge));

					for (++itRow; itRow != _rows.end(); ++itRow)
					{
						if(!itRow->getPlace())
						{
							continue;
						}

						if(	itRow->getIsArrival()
	// 					if(	(	itRow->getIsArrival() &&
	// 							(	passageOk ||
	// 								itRow->getCompulsory() == TimetableRow::PassageSuffisant
	// 						)	) ||
	// 						(	itRow->getIsDeparture() &&
	// 							itRow->getIsArrival()
	// 						)
						){
							for(const Edge* arrivalLinestop(departureLinestop->getFollowingArrivalForFineSteppingOnly());
								arrivalLinestop != NULL;
								arrivalLinestop = arrivalLinestop->getFollowingArrivalForFineSteppingOnly()
							){
								if(	dynamic_cast<const StopArea*>(arrivalLinestop->getFromVertex()->getHub())->getKey() == itRow->getPlace()->getKey()
								){
									result = true;
									break;
								}
							}
							if (result)
							{
								break;
							}
						}
					}
				}
			}
			else if(!_rowGroups.empty()) // New method
			{
				// A1: JourneyPattern selection : there must be at least a departure stop of the line in the departures rows
				bool departureOK(false);
				RowGroups::const_iterator itRowGroup;
				Path::Edges::const_iterator firstIsForbidden(edges.end());
				for(itRowGroup = _rowGroups.begin(); itRowGroup != _rowGroups.end(); ++itRowGroup)
				{
					const TimetableRowGroup& rowGroup(**itRowGroup);

					// Check if this row group is a departure filter
					if(	!rowGroup.get<IsDeparture>() ||
						rowGroup.get<TimetableRowRule>() == NeutralRow
					){
						continue;
					}

					// Search for a matching edge / row pair
					BOOST_FOREACH(const TimetableRowGroupItem* item, rowGroup.getItems())
					{
						bool forbiddenEdgeBefore(firstIsForbidden == edges.end());
						for (itEdge = edges.begin(); itEdge != edges.end(); ++itEdge)
						{
							if(firstIsForbidden != edges.end() && itEdge == firstIsForbidden)
							{
								forbiddenEdgeBefore = true;
							}

							if(	(*itEdge)->isDeparture() &&
								(itEdge+1) != edges.end() &&
								(*itEdge)->getHub() &&
								dynamic_cast<const StopArea*>((*itEdge)->getHub()) == &(*item->get<StopArea>()) &&
								(	_authorizedPhysicalStops.empty() ||
									_authorizedPhysicalStops.find(dynamic_cast<const StopPoint*>((*itEdge)->getFromVertex())) != _authorizedPhysicalStops.end()
								)
							){
								// Avoid first is forbidden rows
								if(!departureOK &&
									rowGroup.get<TimetableRowRule>() == FirstIsForbidden
								){
									firstIsForbidden = itEdge;
									break;
								}

								departureOK = true;
								if(	firstIsForbidden != edges.end() &&
									forbiddenEdgeBefore
								){
									return false;
								}

								firstIsForbidden = edges.end();
								if(rowGroup.get<TimetableRowRule>() == SufficientRow)
								{
									return true;
								}
								break;
							}
						}

						if(departureOK || firstIsForbidden != edges.end())
						{
							break;
						}
					}

					if (departureOK)
					{
						break;
					}
				}

				// Break if possible
				if(!departureOK)
				{
					return false;
				}



				// A2: JourneyPattern selection : there must be at least an arrival stop of the line in the arrival rows, after the departure
				// this test is ignored if the timetable is defined only by a departure stop
				if(_rowGroups.size() > 1)
				{
					result = false;
					const LineStop* departureLinestop(static_cast<const LineStop*>(*itEdge));
					const Edge* firstIsForbidden(NULL);

					for (++itRowGroup; itRowGroup != _rowGroups.end(); ++itRowGroup)
					{
						const TimetableRowGroup& rowGroup(**itRowGroup);

						// Check if this row group is an arrival filter
						if(	!rowGroup.get<IsArrival>() ||
							rowGroup.get<TimetableRowRule>() == NeutralRow
						){
							continue;
						}

						// Search for a matching edge / row pair
						bool arrivalOK(false);
						BOOST_FOREACH(const TimetableRowGroupItem* item, rowGroup.getItems())
						{
							bool forbiddenEdgeBefore(!firstIsForbidden);
							for(const Edge* arrivalLinestop(departureLinestop->getFollowingArrivalForFineSteppingOnly());
								arrivalLinestop != NULL;
								arrivalLinestop = arrivalLinestop->getFollowingArrivalForFineSteppingOnly()
							){
								if(firstIsForbidden && arrivalLinestop == firstIsForbidden)
								{
									forbiddenEdgeBefore = true;
								}

								if(	dynamic_cast<const StopArea*>(arrivalLinestop->getFromVertex()->getHub()) == &(*item->get<StopArea>())
								){
									// Avoid first is forbidden rows
									if(!arrivalOK &&
										rowGroup.get<TimetableRowRule>() == FirstIsForbidden
									){
										firstIsForbidden = arrivalLinestop;
										break;
									}

									if(	firstIsForbidden &&
										forbiddenEdgeBefore
									){
										return false;
									}

									arrivalOK = true;
									break;
								}
							}

							if (arrivalOK || firstIsForbidden)
							{
								break;
							}
						}

						if(arrivalOK)
						{
							result = true;
							break;
						}
					}
				}
			}

			return result;
		}



		TimetableResult TimetableGenerator::build(
			bool withWarnings,
			boost::shared_ptr<TimetableResult::Warnings> warnings
		) const	{
			TimetableResult result(warnings);

			// Journey patterns selection
			typedef std::vector<const JourneyPattern*> JourneyPatterns;
			JourneyPatterns	journeyPatterns;
			BOOST_FOREACH(Registry<JourneyPattern>::value_type it, _env.getRegistry<JourneyPattern>())
			{
				// JourneyPattern selection
				const JourneyPattern& journeyPattern(*it.second);
				if(!_isJourneyPatternSelected(journeyPattern))
				{
					continue;
				}

				// Insertion in the journey patterns list
				if(_baseCalendar.hasAtLeastOneCommonDateWith(journeyPattern))
				{
					journeyPatterns.push_back(&journeyPattern);
				}
				BOOST_FOREACH(const JourneyPattern::SubLines::value_type& subline, journeyPattern.getSubLines())
				{
					if (!_baseCalendar.hasAtLeastOneCommonDateWith(*subline))
					{
						continue;
					}
					journeyPatterns.push_back(subline);
				}
			}

			// Rows list generation (new method)
			if(!_rowGroups.empty())
			{
				_rows.clear();
				size_t rank(0);
				for(RowGroups::const_iterator itRowGroup(_rowGroups.begin()); itRowGroup != _rowGroups.end(); ++itRowGroup)
				{
					const TimetableRowGroup& rowGroup(**itRowGroup);

					if(rowGroup.get<AutoRowsOrder>())
					{
						// Creation of the stops list
						typedef PlacesList<const StopArea*, const JourneyPattern*> PlacesListConfiguration;
						PlacesListConfiguration orderedPlaces;
						BOOST_FOREACH(const JourneyPattern* jp, journeyPatterns)
						{
							PlacesListConfiguration::List jlist;

							// Build of the places list of the route
							bool beforeRowGroup(true);
							BOOST_FOREACH(Edge* edge, jp->getAllEdges())
							{
								const LinePhysicalStop* ls(dynamic_cast<const LinePhysicalStop*>(edge));
								const StopArea& stopArea(*ls->getPhysicalStop()->getConnectionPlace());

								if(	!rowGroup.contains(stopArea)
								){
									// Jump over preceding row groups
									if(beforeRowGroup)
									{
										continue;
									}

									// Check if a following row group has been reached
									RowGroups::const_iterator itFollowingRowGroup(itRowGroup);
									for(++itFollowingRowGroup;
										itFollowingRowGroup != _rowGroups.end();
										++itFollowingRowGroup
									){
										if((*itFollowingRowGroup)->contains(stopArea))
										{
											break;
										}
									}

									continue;
								}

								beforeRowGroup = false;

								PlacesListConfiguration::PlaceInformation item(
									&stopArea,
									false,
									false
								);
								jlist.push_back(item);
							}

							orderedPlaces.addList(make_pair(jp, jlist));
						}

						// Integration of the stops in the temporary timetable object
						BOOST_FOREACH(const PlacesListConfiguration::List::value_type& place, orderedPlaces.getResult())
						{
							TimetableRow row;
							row.setIsArrival(rowGroup.get<IsArrival>());
							row.setIsDeparture(rowGroup.get<IsDeparture>());
							row.setPlace(place.place);
							row.setRank(rank++);
							_rows.push_back(row);
						}
					}
					else
					{
						BOOST_FOREACH(const TimetableRowGroup::Items::value_type& item, rowGroup.getItems())
						{
							TimetableRow row;
							row.setIsDeparture(rowGroup.get<IsDeparture>());
							row.setIsArrival(rowGroup.get<IsArrival>());
							row.setPlace(&(*item->get<StopArea>()));
							row.setRank(rank++);
							_rows.push_back(row);
						}
					}
				}
				
			}

			if(!_rows.empty())
			{
				// Loop on each line of the database
				BOOST_FOREACH(const JourneyPattern* journeyPattern, journeyPatterns)
				{
					_scanServices(result, *journeyPattern);
				}

				if(withWarnings)
				{
					_buildWarnings(result);
				}

				// Before transfers
				if(_transferTimetableBefore.get())
				{
					result.createBeforeTransfer();
					TimetableResult beforeResult(_transferTimetableBefore->build(false, boost::shared_ptr<TimetableResult::Warnings>()));

					for(TimetableResult::Columns::const_iterator col(result.getColumns().begin()); col != result.getColumns().end(); ++col)
					{
						// Tests if the columns service begins actually at the first row
						if(!col->getContent().begin()->first)
						{
							result.getBeforeTransferTimetable(1).getColumns().push_back(TimetableColumn(*_transferTimetableBefore));
							continue;
						}

						// Search of the best transfer col in the transfer generated result
						optional<TimetableColumn> lastOKCol;
						BOOST_REVERSE_FOREACH(const TimetableResult::Columns::value_type& beforeCol, beforeResult.getColumns())
						{
							// Tests if the column is compatible for a transfer
							if(	!beforeCol.getCalendar().includesDates(col->getCalendar()) || // Calendar compatibility
								!beforeCol.getContent().rbegin()->first // Transfer available in the same stop area
							){
								continue;
							}

							// Tests if the arrival time is compatible, considering transfer time
							time_duration maxTime(col->getContent().begin()->second);
							maxTime -= _rows.begin()->getPlace()->getTransferDelay(
								*beforeCol.getContent().rbegin()->first,
								*col->getContent().begin()->first
							);
							if(beforeCol.getContent().rbegin()->second <= maxTime)
							{
								// Record the column as best transfer
								lastOKCol = beforeCol;
								break;
							}
						}

						if(lastOKCol)
						{
							// Test if the column was not already used
							for(TimetableResult::Columns::const_iterator prec(result.getColumns().begin()); prec != col; ++prec)
							{
								if(!prec->getLine())
								{
									continue;
								}
								if(prec->getCalendar() == col->getCalendar() &&
									result.getBeforeTransferTimetable(1).getColumns().at(prec - result.getColumns().begin()) == lastOKCol
								){
									lastOKCol = optional<TimetableColumn>();
								}
							}
						}

						// Store the transfer column
						result.getBeforeTransferTimetable(1).getColumns().push_back(lastOKCol ? *lastOKCol : TimetableColumn(*_transferTimetableBefore));
					}
				}

				// After transfers
				if(_transferTimetableAfter.get())
				{
					result.createAfterTransfer();
					TimetableResult afterResult(_transferTimetableAfter->build(false, boost::shared_ptr<TimetableResult::Warnings>()));

					for(TimetableResult::Columns::const_reverse_iterator col(result.getColumns().rbegin()); col != result.getColumns().rend(); ++col)
					{
						// Tests if the columns service begins actually at the first row
						if(!col->getContent().rbegin()->first)
						{
							result.getAfterTransferTimetable(1).getColumns().insert(
								result.getAfterTransferTimetable(1).getColumns().begin(),
								TimetableColumn(*_transferTimetableAfter)
							);
							continue;
						}

						// Search of the best transfer col in the transfer generated result
						optional<TimetableColumn> lastOKCol;
						BOOST_FOREACH(const TimetableResult::Columns::value_type& afterCol, afterResult.getColumns())
						{
							// Tests if the column is compatible for a transfer
							if(	!afterCol.getCalendar().includesDates(col->getCalendar()) || // Calendar compatibility
								!afterCol.getContent().begin()->first || // Transfer available in the same stop area
								!_rows.rbegin()->getPlace()
							){
								continue;
							}

							// Tests if the arrival time is compatible, considering transfer time
							time_duration minTime(col->getContent().rbegin()->second);
							minTime += _rows.rbegin()->getPlace()->getTransferDelay(
								*col->getContent().rbegin()->first,
								*afterCol.getContent().begin()->first
							);
							if(afterCol.getContent().begin()->second >= minTime)
							{
								// Record the column as best transfer
								lastOKCol = afterCol;
								break;
							}
						}

						if(lastOKCol)
						{
							// Test if the column was not already used
							for(TimetableResult::Columns::const_reverse_iterator prec(result.getColumns().rbegin()); prec != col; ++prec)
							{
								if(!prec->getLine())
								{
									continue;
								}
								if(prec->getCalendar() == col->getCalendar() &&
									result.getAfterTransferTimetable(1).getColumns().at(result.getColumns().size() - (result.getColumns().rend() - prec)) == lastOKCol
								){
									lastOKCol = optional<TimetableColumn>();
								}
							}
						}

						// Store the transfer column
						result.getAfterTransferTimetable(1).getColumns().insert(
							result.getAfterTransferTimetable(1).getColumns().begin(),
							lastOKCol ? *lastOKCol : TimetableColumn(*_transferTimetableAfter)
						);
					}
				}

				// Compression
				if(_compression)
				{
					TimetableResult newResult(result.copy());
					const TimetableResult::Columns& allColumns(result.getColumns());
					pair<TimetableResult::Columns::const_iterator, TimetableResult::Columns::const_iterator> lastColSet(
						make_pair(allColumns.end(), allColumns.end())
					);
					vector<size_t> sequence;
					vector<TimetableColumn> standByColumns;

					// Loop on columns
					long nextHour(0);
					for(TimetableResult::Columns::const_iterator itCol(allColumns.begin()); itCol != allColumns.end(); ++itCol)
					{
						// If no last col set, begin a new hour
						if(lastColSet.first == allColumns.end())
						{
							lastColSet.first = itCol;
							lastColSet.second = itCol;
							newResult.getColumns().push_back(*itCol);
							sequence.clear();
							sequence.push_back(newResult.getColumns().size() - 1);
							nextHour = itCol->getHour() + 1;
							continue;
						}

						// Check if the current col is in the last col set
						if(itCol->getHour() == lastColSet.first->getHour())
						{
							lastColSet.second = itCol;
							newResult.getColumns().push_back(*itCol);
							sequence.push_back(newResult.getColumns().size() - 1);
							continue;
						}

						// We are in the next col set : search for repeated sequence
						size_t repeats(0);
						TimetableResult::Columns::const_iterator itColStart(itCol);
						for(time_duration delta(hours(1)); ; delta += hours(1))
						{
							// The next sequence
							TimetableResult::Columns::const_iterator seqEnd(allColumns.end());
							for(TimetableResult::Columns::const_iterator itCol2(itColStart); itCol2 != allColumns.end() && itCol2->getHour() == nextHour; ++itCol2)
							{
								seqEnd = itCol2;
							}

							// Comparison of the sequence
							if(	seqEnd != allColumns.end() &&
								seqEnd - itColStart == lastColSet.second - lastColSet.first
							){
								bool ok(true);
								for(size_t curSetColRank(0); curSetColRank<=seqEnd - itColStart; ++curSetColRank)
								{
									if(	!(itColStart+curSetColRank)->isLike(*(lastColSet.first + curSetColRank), delta)
									){
										ok = false;
										break;
									}
								}

								if(ok)
								{
									itColStart = seqEnd + 1;
									++repeats;
									++nextHour;
								}
								else
								{
									lastColSet = make_pair(allColumns.end(), allColumns.end());
									break;
								}
							}
							else
							{
								lastColSet = make_pair(allColumns.end(), allColumns.end());
								break;
							}
						}

						// If at least 4 repeats, then cut the result
						if(repeats >= 4)
						{
							size_t compressionRank(0);
							BOOST_FOREACH(size_t colRank, sequence)
							{
								newResult.getColumns().at(colRank).setCompression(
									compressionRank++,
									repeats
								);
							}
							itCol += (repeats * sequence.size() - 1);
						}
					
						if(itCol == allColumns.end())
						{
							break;
						}
					}
					return newResult;
				}
			}
			return result;
		}



		void TimetableGenerator::_scanServices(
			TimetableResult& result,
			const pt::JourneyPattern& line
		) const	{
			// Loop on each service
			BOOST_FOREACH(const Service* servicePtr, line.getServices())
			{
				// Permanent service filter
				const SchedulesBasedService* service(dynamic_cast<const SchedulesBasedService*>(servicePtr));
				if (service == NULL)
					continue;

				// Calendar filter
				if(	!(_baseCalendar.hasAtLeastOneCommonDateWith(*service))
					|| (!_withContinuousServices && service->isContinuous())
				)	continue;

				// Column creation
				TimetableColumn col(*this, *service);

				// Column storage or merge
				_insert(result, col);
			}
		}



		void TimetableGenerator::_insert(
			TimetableResult& result,
			const TimetableColumn& col
		) const {
			TimetableResult::Columns::iterator itCol;
			for (itCol = result.getColumns().begin(); itCol != result.getColumns().end(); ++itCol)
			{
				if(	(	*itCol == col ||
						(	(col.includes(*itCol) || itCol->includes(col)) &&
							col.getCalendar() == itCol->getCalendar()
					)	) &&
					_mergeColsWithSameTimetables
				){
					if(itCol->includes(col))
					{
						itCol->merge(col);
					}
					else
					{
						TimetableColumn newCol(col);
						newCol.merge(*itCol);
						*itCol = newCol;
						// todo handle transfers too
					}
					return;
				}

				if (col <= *itCol)
				{
					result.getColumns().insert(itCol, col);
					return;
				}
			}
			result.getColumns().push_back(col);
		}



		void TimetableGenerator::_buildWarnings(
			TimetableResult& result
		) const	{
			int nextNumber(result.getWarnings().size() + 1);
			CalendarModule::CalendarTitlesGenerator calendarTitlesGenerator(_baseCalendar);
			for(TimetableResult::Columns::iterator itCol(result.getColumns().begin()); itCol != result.getColumns().end(); ++itCol)
			{
				if(itCol->getCalendar() == _baseCalendar) continue;

				boost::shared_ptr<TimetableWarning> warn;
				BOOST_FOREACH(const TimetableResult::Warnings::value_type& itWarn, result.getWarnings())
				{
					if(itWarn.second->getCalendar() == itCol->getCalendar())
					{
						warn = itWarn.second;
						break;
					}
				}

				if (!warn.get())
				{
					CalendarModule::BaseCalendar baseCalendar(
						calendarTitlesGenerator.getBestCalendarTitle(itCol->getCalendar())
					);
					warn = result.getWarnings().insert(
						make_pair(
							nextNumber,
							boost::shared_ptr<TimetableWarning>(new TimetableWarning(
								itCol->getCalendar(),
								nextNumber,
								baseCalendar.second,
								baseCalendar.first
					)	)	)	).first->second;
					++nextNumber;
				}

				itCol->setWarning(warn);
			}
		}



		const TimetableGenerator& TimetableGenerator::getBeforeTransferTimetable( std::size_t depth ) const
		{
			if(depth == 0 || !_transferTimetableBefore.get())
			{
				return *this;
			}
			else
			{
				return _transferTimetableBefore->getBeforeTransferTimetable(depth - 1);
			}
		}



		const TimetableGenerator& TimetableGenerator::getAfterTransferTimetable( std::size_t depth ) const
		{
			if(depth == 0 || !_transferTimetableAfter.get())
			{
				return *this;
			}
			else
			{
				return _transferTimetableAfter->getAfterTransferTimetable(depth - 1);
			}
		}



		bool TimetableGenerator::RowGroupsSort::operator()(
			TimetableRowGroup* g1,
			TimetableRowGroup* g2
		) const	{
			assert(g1);
			assert(g2);

			if(	g1 &&
				g2 &&
				g1->get<Rank>() != g2->get<Rank>()
				){
					return g1->get<Rank>() < g2->get<Rank>();
			}

			return g1 < g2;
		}
}	}
