
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
#include "TimetableServiceColRowInterfacePage.hpp"
#include "TimetableServiceRowInterfacePage.h"
#include "Interface.h"
#include "InterfacePageException.h"
#include "Env.h"
#include "TimetableTableSync.h"
#include "Service.h"
#include "TimetableResult.hpp"
#include "CalendarTemplate.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace timetables;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, timetables::TimetableInterfacePage>::FACTORY_KEY("timetable");
	}

	namespace timetables
	{
		TimetableInterfacePage::TimetableInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, TimetableInterfacePage>(),
			Registrable(0)
		{
		}
		
		

		void TimetableInterfacePage::display(
			std::ostream& stream,
			const Timetable& object,
			const timetables::TimetableGenerator& generator,
			const timetables::TimetableResult& result,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			// Common parameters
			pv.push_back(GetTimetableTypeCode(object.getContentType())); //0
			pv.push_back(object.getTitle()); //1

			// 2 : Notes
			stringstream notes;
			if(getInterface()->hasPage<TimetableNoteInterfacePage>())
			{
				const TimetableNoteInterfacePage* notePage(getInterface()->getPage<TimetableNoteInterfacePage>());
				BOOST_FOREACH(const TimetableResult::Warnings::value_type& warning, result.getWarnings())
				{
					notePage->display(notes, warning.second, variables, request);
				}
			}
			pv.push_back(notes.str()); //2

			pv.push_back(object.getBaseCalendar()->getText()); //3


			// Specific parameters
			switch(object.getContentType())
			{
			case Timetable::CONTAINER:
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
						const TimetableInterfacePage* page(
							tt->getInterface() ?
							tt->getInterface()->getPage<TimetableInterfacePage>() :
							this
						);
						auto_ptr<TimetableGenerator> g(tt->getGenerator(Env::GetOfficialEnv()));
						g->build();
						page->display(content, *tt, *g, result, variables, request);
					}
					pv.push_back(content.str()); //4
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
				{
					if(	getInterface()->hasPage<TimetableServiceColRowInterfacePage>()
					){
						const TimetableServiceColRowInterfacePage* page(getInterface()->getPage<TimetableServiceColRowInterfacePage>());

						// 4 : Lines row
						stringstream linesContent;
						page->display(linesContent, result.getRowLines(), variables, request);
						pv.push_back(linesContent.str()); //4

						// 5 : Time rows
						stringstream timesContent;
						BOOST_FOREACH(const TimetableGenerator::Rows::value_type& row, generator.getRows())
						{
							const TimetableResult::RowTimesVector times(result.getRowSchedules(row.getRank()));
							page->display(timesContent, row, times, variables, request);
						}
						pv.push_back(timesContent.str()); //5
						
						// 6 : Rolling stock rows
						stringstream rollingStockContent;
						page->display(rollingStockContent, result.getRowRollingStock(), variables, request);
						pv.push_back(rollingStockContent.str()); //6

						// 7 : Booking rows
						pv.push_back(string()); //7

						// 8 : Note rows
						stringstream notesContent;
						const TimetableResult::RowNotesVector notes(result.getRowNotes());
						page->display(notesContent, notes, variables, request);
						pv.push_back(notesContent.str()); //8

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
						pv.push_back(lexical_cast<string>(aNote)); //9
					}
					else
					{
						pv.push_back(string()); //3
						pv.push_back(string()); //4
						pv.push_back(string()); //5
						pv.push_back(string()); //6
						pv.push_back(string()); //7
					}
				}
				break;

			case Timetable::TABLE_SERVICES_IN_ROWS:
				{
					stringstream content;
					if(getInterface()->hasPage<TimetableServiceRowInterfacePage>())
					{
						const TimetableServiceRowInterfacePage* page(getInterface()->getPage<TimetableServiceRowInterfacePage>());
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

							page->display(content, column, lastSchedule, rank++, followingWithSameHour, variables, request);
							lastSchedule = column.getContent().begin()->second;
						}
					}
					pv.push_back(content.str()); //3
				}
				break;

			case Timetable::TIMES_IN_COLS:

				break;

			case Timetable::TIMES_IN_ROWS:

				break;
			}

		
			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&object)
				, request
			);
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
