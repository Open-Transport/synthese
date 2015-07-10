
/** TimetableBuildService class implementation.
	@file TimetableBuildService.cpp
	@author Hugues Romain
	@date 2012

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

#include "TimetableBuildService.hpp"

#include "Calendar.h"
#include "CalendarTemplate.h"
#include "CityTableSync.h"
#include "CommercialLineTableSync.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LinePhysicalStop.hpp"
#include "PTUseRule.h"
#include "RequestException.h"
#include "Request.h"
#include "SchedulesBasedService.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TimetableRow.h"
#include "TimetableRowGroup.hpp"
#include "TimetableRowGroupItem.hpp"
#include "TimetableTableSync.h"
#include "TimetableWarning.h"

#include <boost/date_time/time_duration.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace geography;
	using namespace calendar;
	using namespace graph;
	using namespace pt;

	template<> const string util::FactorableTemplate<Function,timetables::TimetableBuildService>::FACTORY_KEY("timetable_build");

	namespace timetables
	{
		const string TimetableBuildService::PARAMETER_CALENDAR_ID("cid");
		const string TimetableBuildService::PARAMETER_STOP_PREFIX("stop");
		const string TimetableBuildService::PARAMETER_CITY_PREFIX("city");
		const string TimetableBuildService::PARAMETER_DAY("day");
		const string TimetableBuildService::PARAMETER_WAYBACK_FILTER("wayback");
		const string TimetableBuildService::PARAMETER_IGNORE_PAST_DATES("ignore_past_dates");
		const string TimetableBuildService::PARAMETER_AUTO_INTERMEDIATE_STOPS = "auto_intermediate_stops";
		const string TimetableBuildService::PARAMETER_AUTO_SORT_ROWS_BEFORE = "auto_sort_rows_before";
		const string TimetableBuildService::PARAMETER_AUTO_SORT_ROWS_AFTER = "auto_sort_rows_after";
		const string TimetableBuildService::PARAMETER_ROWS_BEFORE = "rows_before";
		const string TimetableBuildService::PARAMETER_ROWS_AFTER = "rows_after";
		const string TimetableBuildService::PARAMETER_MERGE_COLS_WITH_SAME_SCHEDULES = "merge_cols_with_same_schedules";
		const string TimetableBuildService::PARAMETER_WITH_WARNINGS = "with_warnings";

		const string TimetableBuildService::TAG_TIMETABLE = "timetable";
		const string TimetableBuildService::ATTR_TIMETABLE_RANK = "timetable_rank";
		
		const string TimetableBuildService::TAG_NOTE = "note";
		
		const string TimetableBuildService::TAG_LINE = "line";
		
		const string TimetableBuildService::TAG_STOP_ROW = "stop_row";
		const string TimetableBuildService::ATTR_IS_BEFORE_TRANSFER = "is_before_transfer";
		const string TimetableBuildService::ATTR_TRANSFER_DEPTH = "transfer_depth";
		const string TimetableBuildService::ATTR_IS_ARRIVAL = "is_arrival";
		const string TimetableBuildService::ATTR_IS_DEPARTURE = "is_departure";
		
		const string TimetableBuildService::TAG_PLACE = "place";
		const string TimetableBuildService::TAG_STOP_POINT = "stop_point";

		const string TimetableBuildService::TAG_COL = "col";
		
		const string TimetableBuildService::TAG_CELL = "cell";
		const string TimetableBuildService::ATTR_TIME = "time";
		const string TimetableBuildService::TAG_SERVICE = "service";
		const string TimetableBuildService::ATTR_ID = "id";
		const string TimetableBuildService::ATTR_COMPRESSION_RANK = "compression_rank";
		const string TimetableBuildService::ATTR_COMPRESSION_REPEATED = "compression_repeated";

		const string TimetableBuildService::TAG_COMPRESSION_CELL = "compression_cell";


		TimetableBuildService::TimetableBuildService():
			FactorableTemplate<Function,TimetableBuildService>(),
			_warnings(new TimetableResult::Warnings),
			_timetableRank(0),
			_withWarnings(true)
		{}



		ParametersMap TimetableBuildService::_getParametersMap() const
		{
			ParametersMap map;
			if(_timetable.get())
			{
				if(_ignorePastDates)
				{
					map.insert(PARAMETER_IGNORE_PAST_DATES, *_ignorePastDates);
				}
				if(_commercialLine.get())
				{
					map.insert(Request::PARAMETER_OBJECT_ID, _commercialLine->getKey());
					if(_waybackFilter)
					{
						map.insert(PARAMETER_WAYBACK_FILTER, *_waybackFilter);
					}
					if(_timetable->get<BaseCalendar>())
					{
						if(_timetable->get<BaseCalendar>()->getKey())
						{
							map.insert(PARAMETER_CALENDAR_ID, _timetable->get<BaseCalendar>()->getKey());
						}
						else if(_timetable->get<BaseCalendar>()->isLimited())
						{
							map.insert(PARAMETER_DAY, _timetable->get<BaseCalendar>()->getResult().getFirstActiveDate());
						}
					}
				}
				else if(_line.get())
				{
					map.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
					if(_timetable->get<BaseCalendar>())
					{
						if(_timetable->get<BaseCalendar>()->getKey())
						{
							map.insert(PARAMETER_CALENDAR_ID, _timetable->get<BaseCalendar>()->getKey());
						}
						else if(_timetable->get<BaseCalendar>()->isLimited())
						{
							map.insert(PARAMETER_DAY, _timetable->get<BaseCalendar>()->getResult().getFirstActiveDate());
						}
					}
				}
				else
				{
					if(_timetable->getKey() > 0)
					{
						map.insert(Request::PARAMETER_OBJECT_ID, _timetable->getKey());
					}
					else
					{
						if(_timetable->get<BaseCalendar>())
						{
							map.insert(PARAMETER_CALENDAR_ID, _timetable->get<BaseCalendar>()->getKey());
						}
					}
				}
			}

			return map;
		}



		void TimetableBuildService::_setFromParametersMap(const ParametersMap& map)
		{
			// Calendar to apply
			if(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_ID, 0))
			{
				try
				{
					_calendarTemplate = Env::GetOfficialEnv().get<CalendarTemplate>(map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID));
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("No such calendar");
				}
			}
			else if(map.getOptional<string>(PARAMETER_DAY))
			{
				date curDate(from_string(map.get<string>(PARAMETER_DAY)));
				_calendarTemplate = boost::shared_ptr<CalendarTemplate>(new CalendarTemplate(curDate));
			}

			// Ignore past dates
			_ignorePastDates = map.getOptional<bool>(PARAMETER_IGNORE_PAST_DATES);

			// Wayback filter
			_waybackFilter = map.getOptional<bool>(PARAMETER_WAYBACK_FILTER);

			// Merge cols with same schedules
			_mergeColsWithSameSchedules = map.getOptional<bool>(PARAMETER_MERGE_COLS_WITH_SAME_SCHEDULES);

			// Include warnings
			_withWarnings = map.getDefault<bool>(PARAMETER_WITH_WARNINGS, true);

			// Way 1 : pre-configured timetable
			if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == TimetableTableSync::TABLE.ID)
			{
				try
				{
					_timetable = Env::GetOfficialEnv().get<Timetable>(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					);
				}
				catch(ObjectNotFoundException<Timetable>)
				{
					throw RequestException("No such timetable");
				}

				// Load sub timetables if the timetable is a container
				_containerContent = TimetableTableSync::Search(Env::GetOfficialEnv(), _timetable->getKey());

				// Rows before and after
				string rowsBeforeStr(map.getDefault<string>(PARAMETER_ROWS_BEFORE));
				string rowsAfterStr(map.getDefault<string>(PARAMETER_ROWS_AFTER));
				if(!rowsBeforeStr.empty() || !rowsAfterStr.empty())
				{
					// The template is copied
					boost::shared_ptr<Timetable> timetableCopy(new Timetable);
					timetableCopy->set<BaseCalendar>(_timetable->get<BaseCalendar>());
					timetableCopy->setWaybackFilter(_timetable->getWaybackFilter());
					timetableCopy->set<Title>(_timetable->get<Title>());
					timetableCopy->set<Book>(_timetable->get<Book>());
					timetableCopy->set<Format>(_timetable->get<Format>());
					timetableCopy->setContentType(_timetable->getContentType());
					timetableCopy->set<Rank>(_timetable->get<Rank>());
					timetableCopy->set<AuthorizedLines>(_timetable->get<AuthorizedLines>());
					timetableCopy->set<AuthorizedPhysicalStops>(_timetable->get<AuthorizedPhysicalStops>());
					
					// Rows before
					if(rowsBeforeStr.empty())
					{
						const Timetable* transferTimetableBefore = _timetable->getTransferTimetableBefore(1);

						if(NULL != transferTimetableBefore)
						{
							timetableCopy->set<TransferTimetableBefore>(
								const_cast<Timetable&>(*transferTimetableBefore)
							);
						}
					}
					else
					{
						_addPlacesToRows(
							*timetableCopy,
							rowsBeforeStr,
							true,
							map.getDefault<bool>(PARAMETER_AUTO_SORT_ROWS_BEFORE)
						);
					}

					// Rows of the template
					size_t rank(timetableCopy->getRowGroups().size());
					BOOST_FOREACH(const Timetable::RowGroups::value_type& rowGroup, _timetable->getRowGroups())
					{
						// New row group
						boost::shared_ptr<TimetableRowGroup> rowGroupCopy(new TimetableRowGroup);
						rowGroupCopy->set<Rank>(rank++);
						rowGroupCopy->set<AutoRowsOrder>(rowGroup->get<AutoRowsOrder>());
						rowGroupCopy->set<IsDeparture>(rowGroup->get<IsDeparture>());
						rowGroupCopy->set<IsArrival>(rowGroup->get<IsArrival>());
						rowGroupCopy->set<Timetable>(const_cast<Timetable&>(*_timetable));
						rowGroupCopy->set<Display>(rowGroup->get<Display>());
						rowGroupCopy->set<TimetableRowRule>(rowGroup->get<TimetableRowRule>());
						_rowGroupsOwner.push_back(rowGroupCopy);
						timetableCopy->addRowGroup(*rowGroupCopy);

						// Items loop
						BOOST_FOREACH(const TimetableRowGroup::Items::value_type& item, rowGroup->getItems())
						{
							boost::shared_ptr<TimetableRowGroupItem> itemCopy(new TimetableRowGroupItem);
							itemCopy->set<StopArea>(item->get<StopArea>());
							itemCopy->set<Rank>(item->get<Rank>());
							itemCopy->set<TimetableRowGroup>(*rowGroupCopy);
							_rowGroupItemsOwner.push_back(itemCopy);
							rowGroupCopy->addItem(*itemCopy);
						}
					}

					// Rows after
					if(rowsAfterStr.empty())
					{
						const Timetable* transferTimetableAfter = _timetable->getTransferTimetableAfter(1);

						if(NULL != transferTimetableAfter)
						{
							timetableCopy->set<TransferTimetableAfter>(
								const_cast<Timetable&>(*transferTimetableAfter)
							);
						}
					}
					else
					{
						_addPlacesToRows(
							*timetableCopy,
							rowsAfterStr,
							false,
							map.getDefault<bool>(PARAMETER_AUTO_SORT_ROWS_AFTER, false)
						);
					}

					_timetable = timetableCopy;
				}
			}
			else
			{
				boost::shared_ptr<Timetable> timetable(new Timetable);
				if(!_calendarTemplate.get())
				{
					date curDate(day_clock::local_day());
					_calendarTemplate = boost::shared_ptr<CalendarTemplate>(new CalendarTemplate(curDate));
				}
				if(!_calendarTemplate->isLimited())
				{
					throw RequestException("Calendar must be limited");
				}
				timetable->set<BaseCalendar>(const_cast<CalendarTemplate&>(*_calendarTemplate.get()));

				// Way 2 : line time table
				if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == JourneyPatternTableSync::TABLE.ID)
				{
					try
					{
						_line = Env::GetOfficialEnv().get<JourneyPattern>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
					}
					catch(ObjectNotFoundException<JourneyPattern>&)
					{
						throw RequestException("No such line route");
					}
					timetable->addAuthorizedLine(_line->getCommercialLine());
					timetable->setContentType(Timetable::TABLE_SERVICES_IN_COLS);

					size_t rank(0);
					BOOST_FOREACH(const Edge* edge, _line->getEdges())
					{
						const LinePhysicalStop* lineStop(dynamic_cast<const LinePhysicalStop*>(edge));
						if(!lineStop) // This is an area stop
						{
							continue;
						}

						TimetableRow row;
						row.setIsArrival(edge->isArrival());
						row.setIsDeparture(edge->isDeparture());
						row.setPlace(lineStop->getPhysicalStop()->getConnectionPlace());
						row.setRank(rank++);
						timetable->addRow(row);
					}
				} // Way 4.1 : stop area timetable
				if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == StopAreaTableSync::TABLE.ID)
				{
					boost::shared_ptr<const StopArea> place;
					try
					{
						place = Env::GetOfficialEnv().get<StopArea>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
					}
					catch(ObjectNotFoundException<StopArea>&)
					{
						throw RequestException("No such place");
					}
					timetable->setContentType(Timetable::TABLE_SERVICES_IN_ROWS);
					timetable->setAutoIntermediateStops(map.getOptional<size_t>(PARAMETER_AUTO_INTERMEDIATE_STOPS));

					TimetableRow row;
					row.setIsArrival(false);
					row.setIsDeparture(true);
					row.setPlace(place.get());
					row.setRank(0);
					timetable->addRow(row);
				} // Way 4.2 : physical stop timetable
				else if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == StopPointTableSync::TABLE.ID)
				{
					boost::shared_ptr<const StopPoint> stop;
					try
					{
						stop = Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
					}
					catch(ObjectNotFoundException<StopPoint>&)
					{
						throw RequestException("No such stop");
					}
					timetable->setContentType(Timetable::TABLE_SERVICES_IN_ROWS);
					timetable->setAutoIntermediateStops(map.getOptional<size_t>(PARAMETER_AUTO_INTERMEDIATE_STOPS));

					TimetableRow row;
					row.setIsArrival(false);
					row.setIsDeparture(true);
					row.setPlace(stop->getConnectionPlace());
					row.setRank(0);
					timetable->addRow(row);
					timetable->addAuthorizedPhysicalStop(stop.get());
				}
				else if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == CommercialLineTableSync::TABLE.ID
				){
					try
					{
						_commercialLine = Env::GetOfficialEnv().get<CommercialLine>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
					}
					catch(ObjectNotFoundException<CommercialLine>&)
					{
						throw RequestException("No such line");
					}

					if(_waybackFilter)
					{ // Way 5 : line and wayback
						AddLineDirectionToTimetable(*timetable, *_commercialLine, *_waybackFilter);
					}
					else
					{
						// Timetable properties
						timetable->setContentType(Timetable::CONTAINER);

						boost::shared_ptr<Timetable> tt1(new Timetable);
						tt1->set<BaseCalendar>(timetable->get<BaseCalendar>());
						AddLineDirectionToTimetable(*tt1, *_commercialLine, false);

						boost::shared_ptr<Timetable> tt2(new Timetable);
						tt2->set<BaseCalendar>(timetable->get<BaseCalendar>());
						AddLineDirectionToTimetable(*tt2, *_commercialLine, true);
					}
				}
				else // Way 3 : customized timetable
				{
/*					timetable->setContentType(Timetable::TABLE_SERVICES_IN_COLS);
					for(size_t rank(0);
						!map.getDefault<string>(PARAMETER_CITY_PREFIX + lexical_cast<string>(rank)).empty() &&
						!map.getDefault<string>(PARAMETER_STOP_PREFIX + lexical_cast<string>(rank)).empty();
						++rank
					){
						Site::ExtendedFetchPlaceResult placeResult(
							_site->extendedFetchPlace(
								map.get<string>(PARAMETER_CITY_PREFIX + lexical_cast<string>(rank)),
								map.get<string>(PARAMETER_STOP_PREFIX + lexical_cast<string>(rank))
						)	);
						TimetableRow row;
						if(!dynamic_cast<const StopArea*>(placeResult.placeResult.value))
						{
							throw RequestException("No such place at rank "+ lexical_cast<string>(rank));
						}
						row.setPlace(dynamic_cast<const StopArea*>(placeResult.placeResult.value));
						row.setRank(rank);
						timetable->addRow(row);
					}
*/				}

				_timetable = const_pointer_cast<const Timetable>(timetable);
			}

			// Output
			setOutputFormatFromMap(map, string());
		}



		util::ParametersMap TimetableBuildService::run(
			std::ostream& stream,
			const Request& request
		) const	{

			// New global parameters map
			ParametersMap pm;

			// Building the parameters map from the result
			_outputResult(
				pm,
				_timetable,
				_timetableRank
			);

			// Adding the notes to the parameters map
			BOOST_FOREACH(const TimetableResult::Warnings::value_type& warning, *_warnings)
			{
				boost::shared_ptr<ParametersMap> notePM(new ParametersMap);
				warning.second->toParametersMap(*notePM, true);
				pm.insert(TAG_NOTE, notePM);
			}

			// Output
			if(!getOutputMimeType().empty())
			{
				outputParametersMap(
					pm,
					stream,
					TAG_TIMETABLE,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/55_timetables/timetable_build.xsd"
				);
			}

			return pm;
		}



		void TimetableBuildService::_outputResult(
			util::ParametersMap& pm,
			boost::shared_ptr<const Timetable> object,
			std::size_t rank
		) const {

			object->toParametersMap(pm, true);
			pm.insert(ATTR_TIMETABLE_RANK, rank);

			// Content
			if(object->getContentType() == Timetable::CONTAINER)
			{
				size_t ttRank(0);
				BOOST_FOREACH(const boost::shared_ptr<Timetable>& tt, _containerContent)
				{
					// New parameters map
					boost::shared_ptr<ParametersMap> subPM(new ParametersMap);

					// Recursive call
					_outputResult(
						*subPM,
						tt,
						ttRank++
					);

					// Adding the new map to the current map
					pm.insert(TAG_TIMETABLE, subPM);
				}
			}
			else if(
				object->getContentType() == Timetable::TABLE_SERVICES_IN_ROWS ||
				object->getContentType() == Timetable::TABLE_SERVICES_IN_COLS
			){
				// Generator construction
				auto_ptr<TimetableGenerator> generator(
					object->getGenerator(
						Env::GetOfficialEnv(),
						(	_calendarTemplate.get() && _calendarTemplate->isLimited() ?
							optional<Calendar>(
								(	(_ignorePastDates && *_ignorePastDates) ?
									_calendarTemplate->getResult(Calendar(date(day_clock::local_day()),_calendarTemplate->getMaxDate())) :
									_calendarTemplate->getResult()
								)
							) :
							optional<Calendar>()
				)	)	);
				if(_waybackFilter)
				{
					generator->setWaybackFilter(_waybackFilter);
				}
				if(_mergeColsWithSameSchedules)
				{
					generator->setMergeColsWithSameTimetables(*_mergeColsWithSameSchedules);
				}

				// Timetable build
				TimetableResult result(generator->build(_withWarnings, _warnings));

				if(object->getContentType() == Timetable::TABLE_SERVICES_IN_COLS)
				{
					// 5.1 : Transfers rows before schedules
					stringstream transfersBeforeContent;
					for(size_t depth(object->getBeforeTransferTimetablesNumber()); depth > 0; --depth)
					{
						const TimetableResult::RowServicesVector services(
							result.getBeforeTransferTimetable(depth).getRowServices()
						);
						BOOST_FOREACH(const TimetableGenerator::Rows::value_type& row, generator->getBeforeTransferTimetable(depth).getRows())
						{
							// Jump over corrupted rows
							if(!row.getPlace())
							{
								continue;
							}

							// Jump over empty rows if necessary
							if(object->get<IgnoreEmptyRows>() && !result.getBeforeTransferTimetable(depth).hasSchedules(row.getRank()))
							{
								continue;
							}

							const TimetableResult::RowTimesVector times(
								result.getBeforeTransferTimetable(depth).getRowSchedules(row.getRank())
							);

							boost::shared_ptr<ParametersMap> rowPM(new ParametersMap);

							_outputStopRow(
								*rowPM,
								row,
								times,
								services,
								result.getColumns(),
								true,
								depth
							);

							pm.insert(TAG_STOP_ROW, rowPM);
						}
					}

					// 5.2 : Times rows
					const TimetableResult::RowServicesVector services(result.getRowServices());
					BOOST_FOREACH(const TimetableGenerator::Rows::value_type& row, generator->getRows())
					{
						if(!row.getPlace())
						{
							continue;
						}

						// Jump over empty rows if necessary
						if(object->get<IgnoreEmptyRows>() && !result.hasSchedules(row.getRank()))
						{
							continue;
						}

						const TimetableResult::RowTimesVector times(result.getRowSchedules(row.getRank()));

						boost::shared_ptr<ParametersMap> rowPM(new ParametersMap);

						_outputStopRow(
							*rowPM,
							row,
							times,
							services,
							result.getColumns(),
							false,
							0
						);

						pm.insert(TAG_STOP_ROW, rowPM);
					}

					// 5.3 : Transfers rows after schedules
					for(size_t depth(1); depth <= object->getAfterTransferTimetablesNumber(); ++depth)
					{
						const TimetableResult::RowServicesVector services(
							result.getAfterTransferTimetable(depth).getRowServices()
						);
						BOOST_FOREACH(const TimetableGenerator::Rows::value_type& row, generator->getAfterTransferTimetable(depth).getRows())
						{
							if(!row.getPlace())
							{
								continue;
							}

							// Jump over empty rows if necessary
							if(object->get<IgnoreEmptyRows>() && !result.getAfterTransferTimetable(depth).hasSchedules(row.getRank()))
							{
								continue;
							}

							const TimetableResult::RowTimesVector times(
								result.getAfterTransferTimetable(depth).getRowSchedules(row.getRank())
							);

							boost::shared_ptr<ParametersMap> rowPM(new ParametersMap);

							_outputStopRow(
								*rowPM,
								row,
								times,
								services,
								result.getColumns(),
								false,
								depth
							);

							pm.insert(TAG_STOP_ROW, rowPM);
						}
					}
				}

				// Columns
				size_t rank(0);
				BOOST_FOREACH(const TimetableResult::Columns::value_type& col, result.getColumns())
				{
					boost::shared_ptr<ParametersMap> colPM(new ParametersMap);
					col.toParametersMap(
						*colPM,
						object->getContentType() == Timetable::TABLE_SERVICES_IN_ROWS,
						result.getColumns(),
						rank++
					);
					pm.insert(TAG_COL, colPM);
				}
			}
		}




		bool TimetableBuildService::isAuthorized(const server::Session* session) const
		{
			return true;
		}



		std::string TimetableBuildService::getOutputMimeType() const
		{
			return getOutputMimeTypeFromOutputFormat();
		}



		void TimetableBuildService::_outputStopRow(
			ParametersMap& pm,
			const TimetableRow& place,
			const TimetableResult::RowTimesVector& times,
			const TimetableResult::RowServicesVector& services,
			const TimetableResult::Columns& columns,
			bool isBeforeTransfer,
			std::size_t depth
		) const {

			// Attributes
			pm.insert(ATTR_IS_BEFORE_TRANSFER, isBeforeTransfer);
			pm.insert(ATTR_TRANSFER_DEPTH, depth);
			pm.insert(ATTR_IS_ARRIVAL, place.getIsArrival());
			pm.insert(ATTR_IS_DEPARTURE, place.getIsDeparture());

			// Place
			if(	place.getPlace() &&
				dynamic_cast<const StopArea*>(place.getPlace()))
			{
				boost::shared_ptr<ParametersMap> placePM(new ParametersMap);
				dynamic_cast<const StopArea*>(place.getPlace())->toParametersMap(*placePM, true);
				pm.insert(TAG_PLACE, placePM);
			}

			// Loop on cells
			for(size_t rank(0); rank < times.size(); ++rank)
			{
				// Link to the main parameters map
				pm.insert(
					TAG_CELL,
					_outputCell(
						times,
						services,
						columns,
						rank,
						true
				)	);
			}
		}



		boost::shared_ptr<ParametersMap> TimetableBuildService::_outputCell(
			const TimetableResult::RowTimesVector& times,
			const TimetableResult::RowServicesVector& services,
			const TimetableResult::Columns& columns,
			size_t rank,
			bool extractCompression
		) const	{
			// New parameters map
			boost::shared_ptr<ParametersMap> cellPM(new ParametersMap);

			const TimetableResult::RowTimesVector::value_type& duration(
				times.at(rank)
			);

			// Time
			cellPM->insert(
				ATTR_TIME,
				duration.second.is_not_a_date_time() ? string() : lexical_cast<string>(duration.second)
			);

			// Service ID
			BOOST_FOREACH(const TimetableColumn::Services::value_type& service, services.at(rank))
			{
				boost::shared_ptr<ParametersMap> servicePM(new ParametersMap);
				servicePM->insert(ATTR_ID, service->getKey());
				cellPM->insert(TAG_SERVICE, servicePM);
			}

			// Stop point
			if(duration.first)
			{
				boost::shared_ptr<ParametersMap> stopPointPM(new ParametersMap);
				duration.first->toParametersMap(*stopPointPM, false);
				cellPM->insert(TAG_STOP_POINT, stopPointPM);
			}

			// Compression
			if(columns.at(rank).isCompression())
			{
				cellPM->insert(ATTR_COMPRESSION_RANK, columns.at(rank).getCompressionRank());
				cellPM->insert(ATTR_COMPRESSION_REPEATED, columns.at(rank).getCompressionRepeated());

				// Insert following cells which are integrated to the compression
				if(extractCompression)
				{
					for(size_t compressionRank(rank); 
						(	compressionRank<columns.size() &&
							columns.at(compressionRank).isCompression() &&
							(compressionRank == rank || columns.at(compressionRank).getCompressionRank())
						);
						++compressionRank
					){
						cellPM->insert(
							TAG_COMPRESSION_CELL,
							_outputCell(
								times,
								services,
								columns,
								compressionRank,
								false
						)	);
					}
				}
			}

			return cellPM;
		}



		void TimetableBuildService::AddLineDirectionToTimetable(
			Timetable& timetable,
			const pt::CommercialLine& line,
			bool wayBack
		){
			// Creation of the stops list
			PlacesListConfiguration orderedPlaces;
			BOOST_FOREACH(const Path* route, line.getPaths())
			{
				const JourneyPattern* jp(dynamic_cast<const JourneyPattern*>(route));
				if(	!jp || // the route is not a journey pattern (should not occur)
					jp->getWayBack() != wayBack // wayback filter
				){
					continue;
				}

				// Calendar check
				if(timetable.get<BaseCalendar>()->isLimited())
				{
					bool result(jp->getCalendarCache().hasAtLeastOneCommonDateWith(timetable.get<BaseCalendar>()->getResult()));
					if(!result)
					{
						continue;
					}
				}

				PlacesListConfiguration::List jlist;

				// Build of the places list of the route
				BOOST_FOREACH(Edge* edge, jp->getEdges())
				{
					const LinePhysicalStop* ls(dynamic_cast<const LinePhysicalStop*>(edge));

					PlacesListConfiguration::PlaceInformation item(
						ls->getPhysicalStop()->getConnectionPlace(),
						false,
						false
					);
					jlist.push_back(item);
				}

				orderedPlaces.addList(make_pair(jp, jlist));
			}

			// Integration of the stops in the temporary timetable object
			size_t rank(0);
			BOOST_FOREACH(const PlacesListConfiguration::List::value_type& place, orderedPlaces.getResult())
			{
				TimetableRow row;
				row.setIsArrival(true);
				row.setIsDeparture(true);
				row.setPlace(place.place);
				row.setRank(rank++);
				timetable.addRow(row);
			}

			// Other properties
			timetable.setContentType(Timetable::TABLE_SERVICES_IN_COLS);
			timetable.addAuthorizedLine(&line);
			timetable.setWaybackFilter(wayBack);
		}



		void TimetableBuildService::_addPlacesToRows(
			Timetable& timetable,
			const std::string& places,
			bool before,
			bool autoSort
		) const {
			// Parsing
			vector<string> ids;
			split(ids, places, is_any_of(","));
			if(ids.empty())
			{
				return;
			}

			// New row group
			boost::shared_ptr<TimetableRowGroup> rowGroup(
				new TimetableRowGroup
			);
			rowGroup->set<Rank>(before ? 0 : timetable.getRowGroups().size());
			rowGroup->set<AutoRowsOrder>(autoSort);
			rowGroup->set<IsDeparture>(before);
			rowGroup->set<IsArrival>(!before);
			rowGroup->set<Timetable>(timetable);
			rowGroup->set<Display>(true);
			rowGroup->set<TimetableRowRule>(NecessaryRow);
			_rowGroupsOwner.push_back(rowGroup);
			timetable.addRowGroup(*rowGroup);

			// Items loop
			size_t rank(0);
			BOOST_FOREACH(const string& idStr, ids)
			{
				try
				{
					RegistryKeyType id(lexical_cast<RegistryKeyType>(idStr));

					// id is a stop area
					RegistryTableType tableId(decodeTableId(id));
					if(tableId == StopAreaTableSync::TABLE.ID)
					{
						boost::shared_ptr<TimetableRowGroupItem> item(new TimetableRowGroupItem);
						item->set<StopArea>(*Env::GetOfficialEnv().getEditable<StopArea>(id));
						item->set<Rank>(rank++);
						item->set<TimetableRowGroup>(*rowGroup);
						_rowGroupItemsOwner.push_back(item);
						rowGroup->addItem(*item);
					}
					else if(tableId == CityTableSync::TABLE.ID)
					{
						const City& city(*Env::GetOfficialEnv().get<City>(id));
						BOOST_FOREACH(
							const City::PlacesMatcher::Map::value_type& stop,
							city.getLexicalMatcher(StopArea::FACTORY_KEY).entries()
						){
							boost::shared_ptr<TimetableRowGroupItem> item(new TimetableRowGroupItem);
							item->set<StopArea>(*const_cast<StopArea*>(dynamic_cast<const StopArea*>(stop.second.get())));
							item->set<Rank>(rank++);
							item->set<TimetableRowGroup>(*rowGroup);
							_rowGroupItemsOwner.push_back(item);
							rowGroup->addItem(*item);
						}
					}
				}
				catch(bad_lexical_cast&)
				{
				}
				catch(ObjectNotFoundException<StopArea>&)
				{
				}
				catch(ObjectNotFoundException<City>&)
				{
				}
			}
		}
}	}
