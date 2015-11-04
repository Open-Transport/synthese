
/** MultipleTimetableGenerateFunction class implementation.
	@file MultipleTimetableGenerateFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "MultipleTimetableGenerateFunction.hpp"

#include "CalendarTemplateTableSync.h"
#include "RequestException.h"
#include "Request.h"
#include "StopArea.hpp"
#include "Timetable.h"
#include "TimetableGenerateFunction.h"
#include "TimetableTableSync.h"
#include "WebPageTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace calendar;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,timetables::MultipleTimetableGenerateFunction>::FACTORY_KEY("MultipleTimetableGenerateFunction");

	namespace timetables
	{
		const string MultipleTimetableGenerateFunction::PARAMETER_OBJECT_ID("roid_");
		const string MultipleTimetableGenerateFunction::PARAMETER_CALENDAR_ID("cid_");

		const string MultipleTimetableGenerateFunction::DATA_CONTENT("content_");


		ParametersMap MultipleTimetableGenerateFunction::_getParametersMap() const
		{
			ParametersMap map;
			size_t rank(0);
			BOOST_FOREACH(const Timetables::value_type& tt, _timetables)
			{
				if(tt.first.get())
				{
					map.insert(PARAMETER_OBJECT_ID + lexical_cast<string>(rank), tt.first->getKey());
				}
				if(tt.second.get())
				{
					map.insert(PARAMETER_CALENDAR_ID + lexical_cast<string>(rank), tt.second->getKey());
				}
				++rank;
			}
			if(_ignorePastDates)
			{
				map.insert(TimetableGenerateFunction::PARAMETER_IGNORE_PAST_DATES, *_ignorePastDates);
			}

			return map;
		}

		void MultipleTimetableGenerateFunction::_setFromParametersMap(const ParametersMap& map)
		{
			for(size_t rank(0); map.getOptional<RegistryKeyType>(PARAMETER_OBJECT_ID + lexical_cast<string>(rank)); ++rank)
			{
				try
				{
					Timetables::value_type tt;
					tt.first = TimetableTableSync::Get(
						map.get<RegistryKeyType>(PARAMETER_OBJECT_ID + lexical_cast<string>(rank)),
						Env::GetOfficialEnv()
					);
					if(map.getOptional<RegistryKeyType>(PARAMETER_CALENDAR_ID + lexical_cast<string>(rank)))
					{
						tt.second = CalendarTemplateTableSync::Get(
							map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID + lexical_cast<string>(rank)),
							Env::GetOfficialEnv()
						);
					}
					_timetables.push_back(tt);
				}
				catch (ObjectNotFoundException<Timetable>&)
				{
					throw RequestException("No such timetable");
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("No such calendar");
				}
			}

			_ignorePastDates = map.getOptional<bool>(TimetableGenerateFunction::PARAMETER_IGNORE_PAST_DATES);

			// Display templates
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(TimetableGenerateFunction::PARAMETER_PAGE_ID));
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
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(TimetableGenerateFunction::PARAMETER_NOTE_PAGE_ID));
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
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(TimetableGenerateFunction::PARAMETER_NOTE_CALENDAR_PAGE_ID));
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
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(TimetableGenerateFunction::PARAMETER_ROW_PAGE_ID));
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
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(TimetableGenerateFunction::PARAMETER_CELL_PAGE_ID));
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
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(TimetableGenerateFunction::PARAMETER_PAGE_FOR_SUB_TIMETABLE_ID));
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



		util::ParametersMap MultipleTimetableGenerateFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm(getTemplateParameters());

			boost::shared_ptr<TimetableResult::Warnings> warnings(new TimetableResult::Warnings);
			size_t timetableRank(0);
			BOOST_FOREACH(const Timetables::value_type& tt, _timetables)
			{
				// Containers are forbidden here
				if(tt.first->getContentType() == Timetable::CONTAINER)
				{
					continue;
				}

				stringstream content;
				TimetableGenerateFunction function;
				function.setTemplateParameters(_templateParameters);
				function.setTimetable(tt.first);
				function.setCalendarTemplate(tt.second);
				if(_ignorePastDates) function.setIgnorePastDates(*_ignorePastDates);
				function.setTimetableRank(timetableRank);
				function.setPage(_pageForSubTimetable);
				function.setRowPage(_rowPage);
				function.setCellPage(_cellPage);
				function.setWarnings(warnings);
				function.run(content, request);
				pm.insert(DATA_CONTENT + lexical_cast<string>(timetableRank), content.str());
				timetableRank++;
			}

			// Notes
			if(_notePage.get())
			{
				stringstream notes;
				TimetableGenerateFunction function;
				function.setTemplateParameters(_templateParameters);
				function.setNotePage(_notePage);
				function.setNoteCalendarPage(_noteCalendarPage);
				BOOST_FOREACH(const TimetableResult::Warnings::value_type& warning, *warnings)
				{
					function._displayNote(
						notes,
						request,
						*warning.second
					);
				}
				pm.insert(TimetableGenerateFunction::DATA_NOTES, notes.str()); //2
			}

			_page->display(stream, request, pm);

			return pm;
		}



		bool MultipleTimetableGenerateFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string MultipleTimetableGenerateFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/plain";
		}
	}
}
