
/** TimetableInterfacePage class implementation.
	@file TimetableInterfacePage.cpp
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

#include "TimetableInterfacePage.h"
#include "Timetable.h"
#include "TimetableGenerator.h"
#include "TimetableNoteInterfacePage.h"
#include "TimetableServiceColInterfacePage.hpp"
#include "TimetableServiceRowInterfacePage.h"
#include "Env.h"
#include "TimetableTableSync.h"
#include "Service.h"
#include "TimetableResult.hpp"
#include "CalendarTemplate.h"
#include "StaticFunctionRequest.h"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace timetables;
	using namespace graph;
	using namespace server;
	using namespace cms;

	namespace timetables
	{
		const std::string TimetableInterfacePage::DATA_GENERATOR_TYPE("generator_type");
		const std::string TimetableInterfacePage::DATA_TITLE("title");
		const std::string TimetableInterfacePage::DATA_NOTES("notes");
		const std::string TimetableInterfacePage::DATA_CALENDAR_NAME("calendar_name");
		const std::string TimetableInterfacePage::DATA_AT_LEAST_A_NOTE("at_least_a_note");
		const std::string TimetableInterfacePage::DATA_CONTENT("content");

		const std::string TimetableInterfacePage::DATA_SERVICES_IN_COLS_LINES_ROW("lines_row");
		const std::string TimetableInterfacePage::DATA_SERVICES_IN_COLS_SCHEDULES_ROWS("schedules_rows");
		const std::string TimetableInterfacePage::DATA_SERVICES_IN_COLS_ROLLING_STOCK_ROW("rolling_stock_row");
		const std::string TimetableInterfacePage::DATA_SERVICES_IN_COLS_RESERVATIONS_ROW("reservation_row");
		const std::string TimetableInterfacePage::DATA_SERVICES_IN_COLS_NOTES_ROW("notes_row");

		const std::string TimetableInterfacePage::DATA_SERVICES_IN_ROWS_SCHEDULES_ROWS("schedules_rows");
		
		

		void TimetableInterfacePage::Display(
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
			const timetables::TimetableResult& result
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);


			// Common parameters
			pm.insert(DATA_GENERATOR_TYPE, GetTimetableTypeCode(object.getContentType()));
			pm.insert(DATA_TITLE, object.getTitle());
			pm.insert(Request::PARAMETER_OBJECT_ID, object.getKey());

			// 2 : Notes
			if(notePage.get())
			{
				stringstream notes;
				BOOST_FOREACH(const TimetableResult::Warnings::value_type& warning, result.getWarnings())
				{
					TimetableNoteInterfacePage::Display(notes, notePage, noteCalendarPage, request, *warning.second);
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
							}
							catch(Timetable::ImpossibleGenerationException&)
							{
								continue;
							}

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
								result
							);
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
					BOOST_FOREACH(const TimetableGenerator::Rows::value_type& row, generator.getRows())
					{
						const TimetableResult::RowTimesVector times(result.getRowSchedules(row.getRank()));
						TimetableServiceColInterfacePage::DisplaySchedulesRow(
							timesContent,
							rowPage,
							cellPage,
							request,
							row,
							times
						);
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
						notes
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

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		std::string TimetableInterfacePage::GetTimetableTypeCode(
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
	}
}
