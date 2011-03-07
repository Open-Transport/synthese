
/** TimetableGenerateFunction class implementation.
	@file TimetableGenerateFunction.cpp
	@author Hugues
	@date 2009

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

#include "RequestException.h"
#include "Request.h"
#include "TimetableGenerateFunction.h"
#include "TimetableTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "TimetableRow.h"
#include "Env.h"
#include "City.h"
#include "StopArea.hpp"
#include "JourneyPattern.hpp"
#include "CalendarTemplate.h"
#include "Calendar.h"
#include "LinePhysicalStop.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "Webpage.h"
#include "TimetableWarning.h"
#include "CalendarDateInterfacePage.hpp"
#include "CommercialLine.h"
#include "Webpage.h"
#include "RollingStock.h"
#include "JourneyPattern.hpp"
#include "PTObjectsCMSExporters.hpp"
#include "SchedulesBasedService.h"
#include "TimetableServiceRowInterfacePage.h"

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
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,timetables::TimetableGenerateFunction>::FACTORY_KEY("TimetableGenerateFunction");
	
	namespace timetables
	{
		const string TimetableGenerateFunction::PARAMETER_CALENDAR_ID("cid");
		const string TimetableGenerateFunction::PARAMETER_STOP_PREFIX("stop");
		const string TimetableGenerateFunction::PARAMETER_CITY_PREFIX("city");
		const string TimetableGenerateFunction::PARAMETER_DAY("day");

		const string TimetableGenerateFunction::PARAMETER_PAGE_ID("page_id");
		const string TimetableGenerateFunction::PARAMETER_NOTE_PAGE_ID("note_page_id");
		const string TimetableGenerateFunction::PARAMETER_NOTE_CALENDAR_PAGE_ID("note_calendar_page_id");
		const string TimetableGenerateFunction::PARAMETER_ROW_PAGE_ID("row_page_id");
		const string TimetableGenerateFunction::PARAMETER_CELL_PAGE_ID("cell_page_id");
		const string TimetableGenerateFunction::PARAMETER_PAGE_FOR_SUB_TIMETABLE_ID("page_for_sub_timetable_id");

		const std::string TimetableGenerateFunction::DATA_GENERATOR_TYPE("generator_type");
		const std::string TimetableGenerateFunction::DATA_TITLE("title");
		const std::string TimetableGenerateFunction::DATA_NOTES("notes");
		const std::string TimetableGenerateFunction::DATA_CALENDAR_NAME("calendar_name");
		const std::string TimetableGenerateFunction::DATA_AT_LEAST_A_NOTE("at_least_a_note");
		const std::string TimetableGenerateFunction::DATA_CONTENT("content");
		const std::string TimetableGenerateFunction::DATA_TIMETABLE_RANK("timetable_rank");

		const std::string TimetableGenerateFunction::DATA_SERVICES_IN_COLS_LINES_ROW("lines_row");
		const std::string TimetableGenerateFunction::DATA_SERVICES_IN_COLS_SCHEDULES_ROWS("schedules_rows");
		const std::string TimetableGenerateFunction::DATA_SERVICES_IN_COLS_ROLLING_STOCK_ROW("rolling_stock_row");
		const std::string TimetableGenerateFunction::DATA_SERVICES_IN_COLS_RESERVATIONS_ROW("reservation_row");
		const std::string TimetableGenerateFunction::DATA_SERVICES_IN_COLS_NOTES_ROW("notes_row");

		const std::string TimetableGenerateFunction::DATA_SERVICES_IN_ROWS_SCHEDULES_ROWS("schedules_rows");

		const string TimetableGenerateFunction::DATA_NUMBER("number");
		const string TimetableGenerateFunction::DATA_TEXT("text");
		const string TimetableGenerateFunction::DATA_CALENDAR("calendar");
		const string TimetableGenerateFunction::DATA_FIRST_YEAR("first_year");
		const string TimetableGenerateFunction::DATA_FIRST_MONTH("first_month");
		const string TimetableGenerateFunction::DATA_FIRST_DAY("first_day");
		const string TimetableGenerateFunction::DATA_LAST_YEAR("last_year");
		const string TimetableGenerateFunction::DATA_LAST_MONTH("last_month");
		const string TimetableGenerateFunction::DATA_LAST_DAY("last_day");

		const std::string TimetableGenerateFunction::TYPE_LINE("line");
		const std::string TimetableGenerateFunction::TYPE_TIME("time");
		const std::string TimetableGenerateFunction::TYPE_NOTE("note");
		const std::string TimetableGenerateFunction::TYPE_BOOKING("booking");
		const std::string TimetableGenerateFunction::TYPE_ROLLING_STOCK("rollingstock");

		const string TimetableGenerateFunction::DATA_TYPE("type");
		const string TimetableGenerateFunction::DATA_CELLS_CONTENT("cells_content");
		const string TimetableGenerateFunction::DATA_ROW_RANK("row_rank");
		const string TimetableGenerateFunction::DATA_ROW_RANK_IS_ODD("row_rank_is_odd");
		const string TimetableGenerateFunction::DATA_CELL_RANK("cell_rank");
		const string TimetableGenerateFunction::DATA_CITY_ID("city_id");
		const string TimetableGenerateFunction::DATA_CITY_NAME("city_name");
		const string TimetableGenerateFunction::DATA_PLACE_ID("place_id");
		const string TimetableGenerateFunction::DATA_PLACE_NAME("place_name");
		const string TimetableGenerateFunction::DATA_HOURS("hours");
		const string TimetableGenerateFunction::DATA_MINUTES("minutes");
		const string TimetableGenerateFunction::DATA_NOTE_NUMBER("note_number");
		const string TimetableGenerateFunction::DATA_NOTE_TEXT("note_text");
		const string TimetableGenerateFunction::DATA_ROLLING_STOCK_NAME("rolling_stock_name");
		const string TimetableGenerateFunction::DATA_ROLLING_STOCK_ALIAS("rolling_stock_alias");
		const string TimetableGenerateFunction::DATA_IS_BEFORE_TRANSFER("is_before_transfer");
		const string TimetableGenerateFunction::DATA_TRANSFER_DEPTH("transfer_depth");
		const string TimetableGenerateFunction::DATA_GLOBAL_RANK("global_rank");
		const string TimetableGenerateFunction::DATA_BLOCK_MAX_RANK("block_max_rank");
		const string TimetableGenerateFunction::DATA_IS_ARRIVAL("is_arrival");
		const string TimetableGenerateFunction::DATA_IS_DEPARTURE("is_departure");
		const string TimetableGenerateFunction::DATA_STOP_NAME_26("stop_name_26");
		const string TimetableGenerateFunction::DATA_TRANSPORT_MODE_ID("transport_mode_id");
		const string TimetableGenerateFunction::DATA_SERVICE_ID("service_id");




		TimetableGenerateFunction::TimetableGenerateFunction():
			FactorableTemplate<Function,TimetableGenerateFunction>(),
			_timetableRank(0),
			_warnings(new TimetableResult::Warnings)
		{
			setEnv(shared_ptr<Env>(new Env));
		}



		ParametersMap TimetableGenerateFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_timetable.get())
			{
				if(_line.get())
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
						size_t rank(0);
						BOOST_FOREACH(const TimetableRow& row, _timetable->getRows())
						{
							map.insert(
								PARAMETER_CITY_PREFIX + lexical_cast<string>(rank++),
								static_cast<const StopArea*>(row.getPlace())->getCity()->getName()
							);
							map.insert(
								PARAMETER_STOP_PREFIX + lexical_cast<string>(rank++),
								static_cast<const StopArea*>(row.getPlace())->getName()
							);
						}
					}
				}
			}
			return map;
		}



		void TimetableGenerateFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Way 1 : pre-configured timetable
			if(decodeTableId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == TimetableTableSync::TABLE.ID)
			{
				try
				{
					_timetable = TimetableTableSync::Get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), *_env);
				}
				catch(ObjectNotFoundException<Timetable>)
				{
					throw RequestException("No such timetable");
				}
			}
			else
			{
				shared_ptr<Timetable> timetable(new Timetable);

				if(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_ID, 0))
				{
					try
					{
						timetable->setBaseCalendar(
							Env::GetOfficialEnv().get<CalendarTemplate>(map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID)).get()
						);
					}
					catch(ObjectNotFoundException<CalendarTemplate>&)
					{
						throw RequestException("No such calendar");
					}
				}
				else
				{
					date curDate(day_clock::local_day());
					if(map.getOptional<string>(PARAMETER_DAY))
					{
						curDate = from_string(map.get<string>(PARAMETER_DAY));
					}

					CalendarTemplate* calendarTemplate(new CalendarTemplate);
					CalendarTemplateElement element;
					element.setCalendar(calendarTemplate);
					element.setInterval(days(1));
					element.setMinDate(curDate);
					element.setMaxDate(curDate);
					element.setOperation(CalendarTemplateElement::ADD);
					element.setRank(0);
					calendarTemplate->addElement(element);
					calendarTemplate->setText(
						lexical_cast<string>(curDate.day()) + "/" + lexical_cast<string>(static_cast<int>(curDate.month())) + "/" + lexical_cast<string>(curDate.year())
					);
					timetable->setBaseCalendar(calendarTemplate);
					_calendarTemplate.reset(calendarTemplate);
				}
				

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
					TimetableRow row;
					row.setIsArrival(false);
					row.setIsDeparture(true);
					row.setPlace(stop->getConnectionPlace());
					row.setRank(0);
					timetable->addRow(row);
					timetable->addAuthorizedPhysicalStop(stop.get());
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
			
			// Display templates

			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
				if(id)
				{
					_page = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_NOTE_PAGE_ID));
				if(id) 
				{
					_notePage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such note row page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_NOTE_CALENDAR_PAGE_ID));
				if(id)
				{
					_noteCalendarPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such note calendar page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_ROW_PAGE_ID));
				if(id)
				{
					_rowPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such row page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_CELL_PAGE_ID));
				if(id) 
				{
					_cellPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such cell page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_FOR_SUB_TIMETABLE_ID));
				if(id) 
				{
					_pageForSubTimetable = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such page for sub timetable : "+ e.getMessage());
			}
		}



		void TimetableGenerateFunction::run( std::ostream& stream, const Request& request ) const
		{
			auto_ptr<TimetableGenerator> generator(_timetable->getGenerator(Env::GetOfficialEnv()));
			TimetableResult result(generator->build(true, _warnings));
			_display(
				stream,
				_page,
				request,
				*_timetable,
				*generator,
				result,
				_timetableRank
			);
		}
		
		
		
		bool TimetableGenerateFunction::isAuthorized(const server::Session* session) const
		{
			return true;
		}



		std::string TimetableGenerateFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/plain";
		}


		void TimetableGenerateFunction::_display(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const Timetable& object,
			const timetables::TimetableGenerator& generator,
			const timetables::TimetableResult& result,
			size_t rank
		) const {
			ParametersMap pm(_savedParameters);

			// Common parameters
			pm.insert(DATA_GENERATOR_TYPE, GetTimetableTypeCode(object.getContentType()));
			pm.insert(DATA_TITLE, object.getTitle());
			pm.insert(Request::PARAMETER_OBJECT_ID, object.getKey());
			pm.insert(DATA_TIMETABLE_RANK, rank);

			// 2 : Notes
			if(_notePage.get())
			{
				stringstream notes;
				BOOST_FOREACH(const TimetableResult::Warnings::value_type& warning, result.getWarnings())
				{
					_displayNote(notes, request, *warning.second);
				}
				pm.insert(DATA_NOTES, notes.str()); //2
			}

			if(object.getBaseCalendar())
			{
				pm.insert(DATA_CALENDAR_NAME, object.getBaseCalendar()->getText()); //3
			}


			// Specific parameters
			switch(object.getContentType())
			{
			case Timetable::CONTAINER:
				{
					if(_pageForSubTimetable.get())
					{
						stringstream content;
						Env env;
						TimetableTableSync::SearchResult contents(
							TimetableTableSync::Search(
								env,
								object.getKey()
						)	);
						BOOST_FOREACH(shared_ptr<Timetable> tt, contents)
						{
							try
							{
								auto_ptr<TimetableGenerator> g(tt->getGenerator(Env::GetOfficialEnv()));
								size_t ttRank(0);
								_display(
									content,
									_pageForSubTimetable,
									request,
									*tt,
									*g,
									result,
									ttRank++
								);
							}
							catch(Timetable::ImpossibleGenerationException&)
							{
								continue;
							}
						}
						pm.insert(DATA_CONTENT, content.str()); //4
					}
				}
				break;

			case Timetable::CALENDAR:
				{

				}
				break;

			case Timetable::LINE_SCHEMA:
				{

				}
				break;
				
			case Timetable::TABLE_SERVICES_IN_COLS:
				if(	_rowPage.get()
				){
					// 4 : Lines row
					stringstream linesContent;
					_displayLinesRow(
						linesContent,
						request,
						result.getRowLines()
					);
					pm.insert(DATA_SERVICES_IN_COLS_LINES_ROW, linesContent.str()); //4

					// 5 : Time rows
					stringstream timesContent;
					size_t globalRank(0);
					for(size_t depth(object.getBeforeTransferTimetablesNumber()); depth > 0; --depth)
					{
						const TimetableResult::RowServicesVector services(
							result.getBeforeTransferTimetable(depth).getRowServices()
						);
						BOOST_FOREACH(const TimetableGenerator::Rows::value_type& row, generator.getBeforeTransferTimetable(depth).getRows())
						{
							const TimetableResult::RowTimesVector times(
								result.getBeforeTransferTimetable(depth).getRowSchedules(row.getRank())
							);
							_displaySchedulesRow(
								timesContent,
								request,
								row,
								times,
								services,
								globalRank++,
								true,
								depth
							);
						}
					}
					const TimetableResult::RowServicesVector services(result.getRowServices());
					BOOST_FOREACH(const TimetableGenerator::Rows::value_type& row, generator.getRows())
					{
						const TimetableResult::RowTimesVector times(result.getRowSchedules(row.getRank()));
						_displaySchedulesRow(
							timesContent,
							request,
							row,
							times,
							services,
							globalRank++,
							false,
							0
						);
					}
					for(size_t depth(1); depth <= object.getAfterTransferTimetablesNumber(); ++depth)
					{
						const TimetableResult::RowServicesVector services(
							result.getAfterTransferTimetable(depth).getRowServices()
						);
						BOOST_FOREACH(const TimetableGenerator::Rows::value_type& row, generator.getAfterTransferTimetable(depth).getRows())
						{
							const TimetableResult::RowTimesVector times(
								result.getAfterTransferTimetable(depth).getRowSchedules(row.getRank())
							);
							_displaySchedulesRow(
								timesContent,
								request,
								row,
								times,
								services,
								globalRank++,
								false,
								depth
							);
						}
					}

					pm.insert(DATA_SERVICES_IN_COLS_SCHEDULES_ROWS, timesContent.str()); //5
					
					// 6 : Rolling stock rows
					stringstream rollingStockContent;
					_displayRollingStockRow(
						rollingStockContent,
						request,
						result.getRowRollingStock()
					);
					pm.insert(DATA_SERVICES_IN_COLS_ROLLING_STOCK_ROW, rollingStockContent.str()); //6

					// 7 : Booking rows
					pm.insert(DATA_SERVICES_IN_COLS_RESERVATIONS_ROW, string()); //7

					// 8 : Note rows
					stringstream notesContent;
					const TimetableResult::RowNotesVector notes(result.getRowNotes());
					_displayNotesRow(
						notesContent,
						request,
						notes,
						result.getColumns()
					);
					pm.insert(DATA_SERVICES_IN_COLS_NOTES_ROW, notesContent.str()); //8

					// 9 : At least a note
					bool aNote(false);
					BOOST_FOREACH(const TimetableResult::RowNotesVector::value_type& note, notes)
					{
						if(note != NULL)
						{
							aNote = true;
							break;
						}
					}
					pm.insert(DATA_AT_LEAST_A_NOTE, aNote); //9
				}
				break;

			case Timetable::TABLE_SERVICES_IN_ROWS:
				if(_rowPage.get())
				{
					stringstream content;
					time_duration lastSchedule;
					size_t rank(0);
					size_t followingWithSameHour;
					for(TimetableResult::Columns::const_iterator it(result.getColumns().begin()); it != result.getColumns().end(); ++it)
					{
						const TimetableResult::Columns::value_type& column(*it);

						followingWithSameHour = 1;
						for(TimetableResult::Columns::const_iterator it2(it+1); 
							it2 != result.getColumns().end() && Service::GetTimeOfDay(it2->getContent().begin()->second).hours() == Service::GetTimeOfDay(it->getContent().begin()->second).hours();
							++it2
						){
							++followingWithSameHour;
						}

						TimetableServiceRowInterfacePage::Display(
							content,
							_rowPage,
							_cellPage,
							request,
							column,
							lastSchedule,
							rank++,
							followingWithSameHour
						);
						lastSchedule = column.getContent().begin()->second;
					}
					pm.insert(DATA_CONTENT, content.str()); //3

					// 9 : At least a note
					bool aNote(false);
					const TimetableResult::RowNotesVector notes(result.getRowNotes());
					BOOST_FOREACH(const TimetableResult::RowNotesVector::value_type& note, notes)
					{
						if(note != NULL)
						{
							aNote = true;
							break;
						}
					}
					pm.insert(DATA_AT_LEAST_A_NOTE, aNote); //9

				}
				break;

			case Timetable::TIMES_IN_COLS:

				break;

			case Timetable::TIMES_IN_ROWS:

				break;
			}

			_page->display(stream, request, pm);
		}



		std::string TimetableGenerateFunction::GetTimetableTypeCode(
			Timetable::ContentType value
		){
			switch(value)
			{
			case Timetable::CONTAINER: return "container";
			case Timetable::CALENDAR: return "calendar";
			case Timetable::LINE_SCHEMA: return "line_schema";
			case Timetable::TABLE_SERVICES_IN_COLS: return "services_in_cols";
			case Timetable::TABLE_SERVICES_IN_ROWS: return "services_in_rows";
			case Timetable::TIMES_IN_COLS: return "times_in_cols";
			case Timetable::TIMES_IN_ROWS: return "times_in_rows";
			}
			return string();
		}



		void TimetableGenerateFunction::_displayNote(
			std::ostream& stream,
			const server::Request& request,
			const TimetableWarning& object
		) const {
			ParametersMap pm(_savedParameters);
		
			pm.insert(DATA_NUMBER, object.getNumber());
			pm.insert(DATA_TEXT, object.getText());

			if(_noteCalendarPage.get())
			{
				stringstream calendarContent;
				const Calendar& calendar(object.getCalendar());
				date firstDate(calendar.getFirstActiveDate().year(), calendar.getFirstActiveDate().month(), 1);
				date lastDate(calendar.getLastActiveDate().end_of_month());
				for(date day(firstDate); day <lastDate; day += days(1))
				{
					CalendarDateInterfacePage::Display(calendarContent, _noteCalendarPage, request, day, calendar.isActive(day));
				}
				pm.insert(DATA_CALENDAR, calendarContent.str());
				pm.insert(DATA_FIRST_DAY, firstDate.day());
				pm.insert(DATA_FIRST_MONTH, firstDate.month());
				pm.insert(DATA_FIRST_YEAR, firstDate.year());
				pm.insert(DATA_LAST_DAY, lastDate.day());
				pm.insert(DATA_LAST_MONTH, lastDate.month());
				pm.insert(DATA_LAST_YEAR, lastDate.year());
			}

			_notePage->display(stream, request, pm);
		}


		void TimetableGenerateFunction::_displayLinesRow(
			std::ostream& stream,
			const server::Request& request,
			const TimetableResult::RowLinesVector& lines
		) const {
			ParametersMap pm(_savedParameters);

			pm.insert(DATA_TYPE, TYPE_LINE);
		
			if(_cellPage.get())
			{
				stringstream content;
				size_t colRank(0);
				BOOST_FOREACH(const CommercialLine* line, lines)
				{
					if(line)
					{
						_displayLineCell(content, request, *line, colRank++);
					}
					else
					{
						_displayEmptyLineCell(content, request, colRank++);
					}
				}
				pm.insert(DATA_CELLS_CONTENT, content.str());
			}

			_rowPage->display(stream, request, pm);
		}



		void TimetableGenerateFunction::_displayLineCell(
			std::ostream& stream,
			const server::Request& request,
			const pt::CommercialLine& object,
			std::size_t colRank
		) const {
			ParametersMap pm(_savedParameters);

			pm.insert(DATA_TYPE, TYPE_LINE); //0
			pm.insert(DATA_CELL_RANK, colRank); //1
			pm.insert(DATA_ROW_RANK, 0); //2
			pm.insert(Request::PARAMETER_OBJECT_ID, object.getKey()); //3
			PTObjectsCMSExporters::ExportLine(pm, object);

			_cellPage->display(stream, request, pm);
		}



		void TimetableGenerateFunction::_displayEmptyLineCell(
			std::ostream& stream,
			const server::Request& request,
			std::size_t colRank
		) const {
			ParametersMap pm(_savedParameters);

			pm.insert(DATA_TYPE, TYPE_LINE); //0
			pm.insert(DATA_CELL_RANK, colRank); //1
			pm.insert(DATA_ROW_RANK, 0); //2
			
			_cellPage->display(stream, request, pm);
		}



		void TimetableGenerateFunction::_displaySchedulesRow(
			std::ostream& stream,
			const server::Request& request,
			const TimetableRow& place,
			const TimetableResult::RowTimesVector& times,
			const TimetableResult::RowServicesVector& services,
			std::size_t globalRank,
			bool isBeforeTransfer,
			std::size_t depth
		) const {
			ParametersMap pm(_savedParameters);

			pm.insert(DATA_TYPE, TYPE_TIME); //0
			pm.insert(DATA_GLOBAL_RANK, globalRank);
			pm.insert(DATA_IS_BEFORE_TRANSFER, isBeforeTransfer);
			pm.insert(DATA_TRANSFER_DEPTH, depth);
		
			if(_cellPage.get())
			{
				stringstream content;
				size_t colRank(0);
				BOOST_FOREACH(time_duration duration, times)
				{
					_displayScheduleCell(
						content,
						request,
						duration,
						place.getRank(),
						colRank,
						services[colRank]
					);
					++colRank;
				}
				pm.insert(DATA_CELLS_CONTENT, content.str()); //1
			}

			pm.insert(DATA_ROW_RANK, place.getRank()); //2

			pm.insert(DATA_ROW_RANK_IS_ODD, place.getRank() % 2);
			pm.insert(DATA_IS_ARRIVAL, place.getIsArrival());
			pm.insert(DATA_IS_DEPARTURE, place.getIsDeparture());

			pm.insert(DATA_CITY_ID, place.getPlace()->getCity()->getKey()); //4
			pm.insert(DATA_PLACE_ID, place.getPlace()->getKey()); //5
			pm.insert(DATA_CITY_NAME, place.getPlace()->getCity()->getName()); //6
			pm.insert(DATA_PLACE_NAME, place.getPlace()->getName()); //7
			if(dynamic_cast<const StopArea*>(place.getPlace()))
			{
				pm.insert(DATA_STOP_NAME_26, static_cast<const StopArea*>(place.getPlace())->getTimetableName());
			}

			_rowPage->display(stream, request, pm);
		}



		void TimetableGenerateFunction::_displayScheduleCell(
			std::ostream& stream,
			const server::Request& request,
			boost::posix_time::time_duration object,
			std::size_t rowRank,
			std::size_t colRank,
			const pt::SchedulesBasedService* service
		) const {
			ParametersMap pm(_savedParameters);

			pm.insert(DATA_TYPE, TYPE_TIME);
			pm.insert(DATA_CELL_RANK, colRank);
			pm.insert(DATA_ROW_RANK, rowRank);
			if(!object.is_not_a_date_time())
			{
				pm.insert(DATA_HOURS, object.hours());
				pm.insert(DATA_MINUTES, object.minutes());
			}

			if(service)
			{
				pm.insert(DATA_SERVICE_ID, service->getKey());
			}

			_cellPage->display(stream, request, pm);
		}



		void TimetableGenerateFunction::_displayNotesRow(
			std::ostream& stream,
			const server::Request& request,
			const TimetableResult::RowNotesVector& notes,
			const TimetableResult::Columns& columns
		) const {
			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(DATA_TYPE, TYPE_NOTE);
		
			if(_cellPage.get())
			{
				stringstream content;
				size_t colRank(0);
				BOOST_FOREACH(const TimetableResult::Columns::value_type& col, columns)
				{
					_displayNoteCell(content, request, colRank++, col);
				}
				pm.insert(DATA_CELLS_CONTENT, content.str());
			}

			_rowPage->display(stream, request, pm);
		}



		void TimetableGenerateFunction::_displayNoteCell(
			std::ostream& stream,
			const server::Request& request,
			std::size_t colRank,
			const TimetableColumn& column
		) const {
			ParametersMap pm(_savedParameters);

			pm.insert(DATA_TYPE, TYPE_NOTE);
			pm.insert(DATA_CELL_RANK, colRank);
			if(column.getWarning().get())
			{
				pm.insert(DATA_NOTE_NUMBER, column.getWarning()->getNumber());
				pm.insert(DATA_NOTE_TEXT, column.getWarning()->getText());
			}
			if(column.getLine() && column.getLine()->getRollingStock())
			{
				pm.insert(DATA_TRANSPORT_MODE_ID, column.getLine()->getRollingStock()->getKey());
			}

			_cellPage->display(stream, request, pm);
		}



		void TimetableGenerateFunction::_displayRollingStockRow(
			std::ostream& stream,
			const server::Request& request,
			const TimetableResult::RowRollingStockVector& rollingStock
		) const {
			ParametersMap pm(_savedParameters);

			pm.insert(DATA_TYPE, TYPE_ROLLING_STOCK); //0

			if(_cellPage.get())
			{
				stringstream content;
				size_t colRank(0);
				BOOST_FOREACH(const RollingStock* object, rollingStock)
				{
					_displayRollingStockCell(content, request, object, colRank++);
				}
				pm.insert(DATA_CELLS_CONTENT, content.str()); //1
			}

			_rowPage->display(stream, request, pm);
		}



		void TimetableGenerateFunction::_displayRollingStockCell(
			std::ostream& stream,
			const server::Request& request,
			const pt::RollingStock* object,
			std::size_t colRank
		) const {
			ParametersMap pm(_savedParameters);

			pm.insert(DATA_TYPE, TYPE_ROLLING_STOCK); //0
			pm.insert(DATA_CELL_RANK, colRank); //1
			if(object)
			{
				pm.insert(Request::PARAMETER_OBJECT_ID, object->getKey()); //3
				pm.insert(DATA_ROLLING_STOCK_ALIAS, object->getIndicator()); //4
				pm.insert(DATA_ROLLING_STOCK_NAME, object->getName()); //5
			}

			_cellPage->display(stream, request, pm);
		}
}	}
