
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
#include "JourneyPatternCopy.hpp"
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

		const string TimetableBuildService::TAG_COL = "col";
		
		const string TimetableBuildService::TAG_CELL = "cell";
		const string TimetableBuildService::ATTR_TIME = "time";
		const string TimetableBuildService::ATTR_SERVICE_ID = "service_id";



		TimetableBuildService::TimetableBuildService():
			FactorableTemplate<Function,TimetableBuildService>(),
			_timetableRank(0),
			_warnings(new TimetableResult::Warnings)
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
					if(_timetable->getBaseCalendar())
					{
						if(_timetable->getBaseCalendar()->getKey())
						{
							map.insert(PARAMETER_CALENDAR_ID, _timetable->getBaseCalendar()->getKey());
						}
						else if(_timetable->getBaseCalendar()->isLimited())
						{
							map.insert(PARAMETER_DAY, _timetable->getBaseCalendar()->getResult().getFirstActiveDate());
						}
					}
				}
				else if(_line.get())
				{
					map.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
					if(_timetable->getBaseCalendar())
					{
						if(_timetable->getBaseCalendar()->getKey())
						{
							map.insert(PARAMETER_CALENDAR_ID, _timetable->getBaseCalendar()->getKey());
						}
						else if(_timetable->getBaseCalendar()->isLimited())
						{
							map.insert(PARAMETER_DAY, _timetable->getBaseCalendar()->getResult().getFirstActiveDate());
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
						if(_timetable->getBaseCalendar())
						{
							map.insert(PARAMETER_CALENDAR_ID, _timetable->getBaseCalendar()->getKey());
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
				_calendarTemplate = shared_ptr<CalendarTemplate>(new CalendarTemplate(curDate));
			}

			// Ignore past dates
			_ignorePastDates = map.getOptional<bool>(PARAMETER_IGNORE_PAST_DATES);

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
					shared_ptr<Timetable> timetableCopy(new Timetable);
					timetableCopy->setBaseCalendar(_timetable->getBaseCalendar());
					timetableCopy->setWaybackFilter(_timetable->getWaybackFilter());
					timetableCopy->setTitle(_timetable->getTitle());
					timetableCopy->setBookId(_timetable->getBookId());
					timetableCopy->setContentType(_timetable->getContentType());
					timetableCopy->setRank(_timetable->getRank());
					timetableCopy->setAuthorizedLines(_timetable->getAuthorizedLines());
					timetableCopy->setAuthorizedPhysicalStops(_timetable->getAuthorizedPhysicalStops());
					
					// Rows before
					if(rowsBeforeStr.empty())
					{
						timetableCopy->setTransferTimetableBefore(
							const_cast<Timetable&>(*_timetable).getTransferTimetableBefore(1)
						);
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
						shared_ptr<TimetableRowGroup> rowGroupCopy(new TimetableRowGroup);
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
							shared_ptr<TimetableRowGroupItem> itemCopy(new TimetableRowGroupItem);
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
						timetableCopy->setTransferTimetableAfter(
							const_cast<Timetable&>(*_timetable).getTransferTimetableAfter(1)
						);
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
				shared_ptr<Timetable> timetable(new Timetable);
				if(!_calendarTemplate.get())
				{
					date curDate(day_clock::local_day());
					_calendarTemplate = shared_ptr<CalendarTemplate>(new CalendarTemplate(curDate));
				}
				if(!_calendarTemplate->isLimited())
				{
					throw RequestException("Calendar must be limited");
				}
				timetable->setBaseCalendar(_calendarTemplate.get());

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
					shared_ptr<const StopArea> place;
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
					shared_ptr<const StopPoint> stop;
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

					_waybackFilter = map.getOptional<bool>(PARAMETER_WAYBACK_FILTER);

					if(_waybackFilter)
					{ // Way 5 : line and wayback
						AddLineDirectionToTimetable(*timetable, *_commercialLine, *_waybackFilter);
					}
					else
					{
						// Timetable properties
						timetable->setContentType(Timetable::CONTAINER);

						shared_ptr<Timetable> tt1(new Timetable);
						tt1->setBaseCalendar(timetable->getBaseCalendar());
						AddLineDirectionToTimetable(*tt1, *_commercialLine, false);

						shared_ptr<Timetable> tt2(new Timetable);
						tt2->setBaseCalendar(timetable->getBaseCalendar());
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
				*_timetable,
				_timetableRank
			);

			// Adding the notes to the parameters map
			BOOST_FOREACH(const TimetableResult::Warnings::value_type& warning, *_warnings)
			{
				shared_ptr<ParametersMap> notePM(new ParametersMap);
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
			const timetables::Timetable& object,
			std::size_t rank
		) const {

			object.toParametersMap(pm);
			pm.insert(ATTR_TIMETABLE_RANK, rank);

			// Content
			if(object.getContentType() == Timetable::CONTAINER)
			{
				size_t ttRank(0);
				BOOST_FOREACH(const shared_ptr<Timetable>& tt, _containerContent)
				{
					// New parameters map
					shared_ptr<ParametersMap> subPM(new ParametersMap);

					// Recursive call
					_outputResult(
						*subPM,
						*tt,
						ttRank++
					);

					// Adding the new map to the current map
					pm.insert(TAG_TIMETABLE, subPM);
				}
			}
			else if(
				object.getContentType() == Timetable::TABLE_SERVICES_IN_ROWS ||
				object.getContentType() == Timetable::TABLE_SERVICES_IN_COLS
			){
				// Generator construction
				auto_ptr<TimetableGenerator> generator(
					object.getGenerator(
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

				// Timetable build
				TimetableResult result(generator->build(true, _warnings));

				if(object.getContentType() == Timetable::TABLE_SERVICES_IN_COLS)
				{
					// 5.1 : Transfers rows before schedules
					stringstream transfersBeforeContent;
					size_t globalRank(0);
					for(size_t depth(object.getBeforeTransferTimetablesNumber()); depth > 0; --depth)
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
							if(object.getIgnoreEmptyRows() && !result.getBeforeTransferTimetable(depth).hasSchedules(row.getRank()))
							{
								continue;
							}

							const TimetableResult::RowTimesVector times(
								result.getBeforeTransferTimetable(depth).getRowSchedules(row.getRank())
							);

							shared_ptr<ParametersMap> rowPM(new ParametersMap);

							_outputStopRow(
								*rowPM,
								row,
								times,
								services,
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
						if(object.getIgnoreEmptyRows() && !result.hasSchedules(row.getRank()))
						{
							continue;
						}

						const TimetableResult::RowTimesVector times(result.getRowSchedules(row.getRank()));

						shared_ptr<ParametersMap> rowPM(new ParametersMap);

						_outputStopRow(
							*rowPM,
							row,
							times,
							services,
							false,
							0
						);

						pm.insert(TAG_STOP_ROW, rowPM);
					}

					// 5.3 : Transfers rows after schedules
					for(size_t depth(1); depth <= object.getAfterTransferTimetablesNumber(); ++depth)
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
							if(object.getIgnoreEmptyRows() && !result.getAfterTransferTimetable(depth).hasSchedules(row.getRank()))
							{
								continue;
							}

							const TimetableResult::RowTimesVector times(
								result.getAfterTransferTimetable(depth).getRowSchedules(row.getRank())
							);

							shared_ptr<ParametersMap> rowPM(new ParametersMap);

							_outputStopRow(
								*rowPM,
								row,
								times,
								services,
								false,
								depth
							);

							pm.insert(TAG_STOP_ROW, rowPM);
						}
					}
				}

				// Columns
				BOOST_FOREACH(const TimetableResult::Columns::value_type& col, result.getColumns())
				{
					shared_ptr<ParametersMap> colPM(new ParametersMap);
					col.toParametersMap(
						*colPM,
						object.getContentType() == Timetable::TABLE_SERVICES_IN_ROWS
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
				shared_ptr<ParametersMap> placePM(new ParametersMap);
				dynamic_cast<const StopArea*>(place.getPlace())->toParametersMap(*placePM);
				pm.insert(TAG_PLACE, placePM);
			}

			// Loop on cells
			size_t rank(0);
			BOOST_FOREACH(time_duration duration, times)
			{
				// New parameters map
				shared_ptr<ParametersMap> cellPM(new ParametersMap);

				// Time
				if(!duration.is_not_a_date_time())
				{
					cellPM->insert(ATTR_TIME, duration);
				}

				// Service ID
				if(services.at(rank))
				{
					cellPM->insert(ATTR_SERVICE_ID, services.at(rank)->getKey());
				}

				// Link to the main parameters map
				pm.insert(TAG_CELL, cellPM);

				++rank;
			}
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
				if(timetable.getBaseCalendar()->isLimited())
				{
					bool result(jp->hasAtLeastOneCommonDateWith(timetable.getBaseCalendar()->getResult()));
					if(!result)
					{
						BOOST_FOREACH(JourneyPatternCopy* subline, jp->getSubLines())
						{
							if(subline->hasAtLeastOneCommonDateWith(timetable.getBaseCalendar()->getResult()))
							{
								result = true;
								break;
							}
						}
					}
					if(!result)
					{
						continue;
					}
				}

				PlacesListConfiguration::List jlist;

				// Build of the places list of the route
				BOOST_FOREACH(Edge* edge, jp->getAllEdges())
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
			shared_ptr<TimetableRowGroup> rowGroup(
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
						shared_ptr<TimetableRowGroupItem> item(new TimetableRowGroupItem);
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
							shared_ptr<TimetableRowGroupItem> item(new TimetableRowGroupItem);
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
