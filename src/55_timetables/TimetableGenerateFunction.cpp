
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
#include "Timetable.h"
#include "TimetableRow.h"
#include "Env.h"
#include "City.h"
#include "StopArea.hpp"
#include "JourneyPattern.hpp"
#include "CalendarTemplate.h"
#include "Calendar.h"
#include "LineStop.h"
#include "StopPoint.hpp"
#include "TimetableResult.hpp"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "Webpage.h"
#include "TimetableWarning.h"
#include "CalendarDateInterfacePage.hpp"
#include "TimetableServiceColInterfacePage.hpp"
#include "TimetableServiceRowInterfacePage.h"
#include "Service.h"

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

		TimetableGenerateFunction::TimetableGenerateFunction():
			FactorableTemplate<Function,TimetableGenerateFunction>()
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
						lexical_cast<string>(curDate.day()) + "/" + lexical_cast<string>(lexical_cast<int>(curDate.month())) + "/" + lexical_cast<string>(curDate.year())
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
						TimetableRow row;
						row.setIsArrival(edge->isArrival());
						row.setIsDeparture(edge->isDeparture());
						row.setPlace(static_cast<const LineStop*>(edge)->getPhysicalStop()->getConnectionPlace());
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
			TimetableResult result(generator->build(true, shared_ptr<TimetableResult::Warnings>()));
			Display(
				stream,
				_page,
				_notePage,
				_noteCalendarPage,
				_pageForSubTimetable,
				_rowPage,
				_cellPage,
				request,
				*_timetable,
				*generator,
				result,
				0
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


		void TimetableGenerateFunction::Display(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> notePage,
			boost::shared_ptr<const cms::Webpage> noteCalendarPage,
			boost::shared_ptr<const cms::Webpage> pageForSubTimetable,
			boost::shared_ptr<const cms::Webpage> rowPage,
			boost::shared_ptr<const cms::Webpage> cellPage,
			const server::Request& request,
			const Timetable& object,
			const timetables::TimetableGenerator& generator,
			const timetables::TimetableResult& result,
			size_t rank
		){
			ParametersMap pm(request.getFunction()->getSavedParameters());

			// Common parameters
			pm.insert(DATA_GENERATOR_TYPE, GetTimetableTypeCode(object.getContentType()));
			pm.insert(DATA_TITLE, object.getTitle());
			pm.insert(Request::PARAMETER_OBJECT_ID, object.getKey());
			pm.insert(DATA_TIMETABLE_RANK, rank);

			// 2 : Notes
			if(notePage.get())
			{
				stringstream notes;
				BOOST_FOREACH(const TimetableResult::Warnings::value_type& warning, result.getWarnings())
				{
					DisplayNote(notes, notePage, noteCalendarPage, request, *warning.second);
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
					if(pageForSubTimetable.get())
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
								Display(
									content,
									pageForSubTimetable,
									notePage,
									noteCalendarPage,
									pageForSubTimetable,
									rowPage,
									cellPage,
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
				if(	rowPage.get()
				){
					// 4 : Lines row
					stringstream linesContent;
					TimetableServiceColInterfacePage::DisplayLinesRow(
						linesContent,
						rowPage,
						cellPage,
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
							TimetableServiceColInterfacePage::DisplaySchedulesRow(
								timesContent,
								rowPage,
								cellPage,
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
						TimetableServiceColInterfacePage::DisplaySchedulesRow(
							timesContent,
							rowPage,
							cellPage,
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
							TimetableServiceColInterfacePage::DisplaySchedulesRow(
								timesContent,
								rowPage,
								cellPage,
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
					TimetableServiceColInterfacePage::DisplayRollingStockRow(
						rollingStockContent,
						rowPage,
						cellPage,
						request,
						result.getRowRollingStock()
					);
					pm.insert(DATA_SERVICES_IN_COLS_ROLLING_STOCK_ROW, rollingStockContent.str()); //6

					// 7 : Booking rows
					pm.insert(DATA_SERVICES_IN_COLS_RESERVATIONS_ROW, string()); //7

					// 8 : Note rows
					stringstream notesContent;
					const TimetableResult::RowNotesVector notes(result.getRowNotes());
					TimetableServiceColInterfacePage::DisplayNotesRow(
						notesContent,
						rowPage,
						cellPage,
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
				if(rowPage.get())
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
							rowPage,
							cellPage,
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

			page->display(stream, request, pm);
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



		void TimetableGenerateFunction::DisplayNote(
			std::ostream& stream,
			boost::shared_ptr<const Webpage> page,
			boost::shared_ptr<const cms::Webpage> calendarDatePage,
			const server::Request& request,
			const TimetableWarning& object
		){
			ParametersMap pm(request.getFunction()->getSavedParameters());
		
			pm.insert(DATA_NUMBER, object.getNumber());
			pm.insert(DATA_TEXT, object.getText());

			if(calendarDatePage.get())
			{
				stringstream calendarContent;
				const Calendar& calendar(object.getCalendar());
				date firstDate(calendar.getFirstActiveDate().year(), calendar.getFirstActiveDate().month(), 1);
				date lastDate(calendar.getLastActiveDate().end_of_month());
				for(date day(firstDate); day <lastDate; day += days(1))
				{
					CalendarDateInterfacePage::Display(calendarContent, calendarDatePage, request, day, calendar.isActive(day));
				}
				pm.insert(DATA_CALENDAR, calendarContent.str());
				pm.insert(DATA_FIRST_DAY, firstDate.day());
				pm.insert(DATA_FIRST_MONTH, firstDate.month());
				pm.insert(DATA_FIRST_YEAR, firstDate.year());
				pm.insert(DATA_LAST_DAY, lastDate.day());
				pm.insert(DATA_LAST_MONTH, lastDate.month());
				pm.insert(DATA_LAST_YEAR, lastDate.year());
			}

			page->display(stream, request, pm);
		}
}	}
