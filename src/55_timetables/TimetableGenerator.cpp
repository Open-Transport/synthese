
/** TimetableGenerator class implementation.
	@file TimetableGenerator.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "LineStop.h"
#include "StopPoint.hpp"
#include "SchedulesBasedService.h"
#include "StopArea.hpp"
#include "Env.h"
#include "CalendarModule.h"
#include "JourneyPatternCopy.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace calendar;
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace timetables
	{
		TimetableGenerator::TimetableGenerator(
			const Env& env
		):	_transferTimetableBefore(NULL),
			_transferTimetableAfter(NULL),
			_withContinuousServices(true),
			_env(env)
			, _maxColumnsNumber(UNKNOWN_VALUE)
		{}



		TimetableResult TimetableGenerator::build(
			bool withWarnings,
			boost::shared_ptr<TimetableResult::Warnings> warnings
		) const	{
			TimetableResult result(warnings);

			if(!_rows.empty())
			{
				// Loop on each line of the database
				BOOST_FOREACH(Registry<JourneyPattern>::value_type it, _env.getRegistry<JourneyPattern>())
				{
					// JourneyPattern selection
					const JourneyPattern& line(*it.second);
					if(!_isLineSelected(line))
					{
						continue;
					}
					
					// A0: JourneyPattern selection upon calendar
					if (_baseCalendar.hasAtLeastOneCommonDateWith(line))
					{
						_scanServices(result, line);
					}

					BOOST_FOREACH(const JourneyPattern::SubLines::value_type& subline, line.getSubLines())
					{
						if (!_baseCalendar.hasAtLeastOneCommonDateWith(*subline))
						{
							continue;
						}
						_scanServices(result, *subline);
					}
				}

				if(withWarnings)
				{
					_buildWarnings(result);
				}

				// Before transfers
				if(_transferTimetableBefore.get())
				{
					result.createBeforeTransfer();
					TimetableResult beforeResult(_transferTimetableBefore->build(false, shared_ptr<TimetableResult::Warnings>()));

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
					TimetableResult afterResult(_transferTimetableAfter->build(false, shared_ptr<TimetableResult::Warnings>()));

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
								!afterCol.getContent().begin()->first // Transfer available in the same stop area
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
					|| !_withContinuousServices && service->isContinuous()
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
				if (*itCol == col ||
					(col.includes(*itCol) || itCol->includes(col)) && col.getCalendar() == itCol->getCalendar()
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

				shared_ptr<TimetableWarning> warn;
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
					warn = result.getWarnings().insert(
						make_pair(
							nextNumber,
							shared_ptr<TimetableWarning>(new TimetableWarning(
								itCol->getCalendar(),
								nextNumber,
								calendarTitlesGenerator.getBestCalendarTitle(itCol->getCalendar())
					)	)	)	).first->second;
					++nextNumber;
				}
				
				itCol->setWarning(warn);
			}
		}



		bool TimetableGenerator::_isLineSelected( const pt::JourneyPattern& line ) const
		{
			if (!line.getUseInTimetables())
				return false;

			bool lineIsSelected(false);
			bool passageOk(false);
			Path::Edges::const_iterator itEdge;
			const Path::Edges& edges(line.getEdges());

			// JourneyPattern is authorized
			if(!_authorizedLines.empty() && _authorizedLines.find(line.getCommercialLine()) == _authorizedLines.end())
			{
				return false;
			}

			// A1: JourneyPattern selection : there must be at least a departure stop of the line in the departures rows
			Rows::const_iterator itRow;
			for (itRow = _rows.begin(); itRow != _rows.end(); ++itRow)
			{
				if (!itRow->getIsDeparture())
					continue;

				for (itEdge = edges.begin(); itEdge != edges.end(); ++itEdge)
				{
					if(	(*itEdge)->isDeparture() &&
						dynamic_cast<const StopArea*>((*itEdge)->getHub())->getKey() == itRow->getPlace()->getKey() &&
						(	_authorizedPhysicalStops.empty() ||
							_authorizedPhysicalStops.find(dynamic_cast<const StopPoint*>((*itEdge)->getFromVertex())) != _authorizedPhysicalStops.end()
						)	
					){
						lineIsSelected = true;
						if (itRow->getIsArrival() || itRow->getCompulsory() == TimetableRow::PassageSuffisant)
							passageOk = true;
						break;
					}
				}
				if (lineIsSelected)
					break;
			}
			if (!lineIsSelected)
				return false;


			// A2: JourneyPattern selection : there must be at least an arrival stop of the line in the arrival rows, after the departure
			// this test is ignored if the timetable is defined only by a departure stop
			if(_rows.size() > 1)
			{
				lineIsSelected = false;
				const LineStop* departureLinestop(static_cast<const LineStop*>(*itEdge));

				for (++itRow; itRow != _rows.end(); ++itRow)
				{
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
								lineIsSelected = true;
								break;
							}
						}
						if (lineIsSelected)
							break;
					}
				}
			}

			return lineIsSelected;
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
	}
}
