
//////////////////////////////////////////////////////////////////////////////////////////
///	CalendarsListService class implementation.
///	@file CalendarsListService.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "CalendarsListService.hpp"

#include "CalendarTemplateTableSync.h"
#include "ParametersMap.h"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,calendar::CalendarsListService>::FACTORY_KEY = "CalendarsListService";
	
	namespace calendar
	{
		const string CalendarsListService::TAG_CALENDAR = "calendar";
		const string CalendarsListService::TAG_CALENDARS = "calendars";
		const string CalendarsListService::PARAMETER_PARENT_ID = "parent_id";

		ParametersMap CalendarsListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void CalendarsListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Calendar
			if(map.isDefined(Request::PARAMETER_OBJECT_ID))
			{
				try
				{
					_calendar = Env::GetOfficialEnv().get<CalendarTemplate>(
						map.get<RegistryKeyType>(
							Request::PARAMETER_OBJECT_ID
					)	);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("No such calendar");
				}
			}

			// Parent
			if(map.isDefined(PARAMETER_PARENT_ID))
			{
				try
				{
					_parent = Env::GetOfficialEnv().get<CalendarTemplate>(
						map.get<RegistryKeyType>(
							PARAMETER_PARENT_ID
					)	);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("No such parent calendar");
				}
			}
		}

		ParametersMap CalendarsListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			vector<const CalendarTemplate*> calendars;
			if(_calendar.get())
			{
				calendars.push_back(_calendar.get());
			}
			else if(_parent)
			{
				if(_parent->get())
				{
					BOOST_FOREACH(
						const CalendarTemplate::ChildrenType::value_type& child,
						(*_parent)->getChildren()
					){
						calendars.push_back(child.second);
					}
				}
				else
				{
					BOOST_FOREACH(
						const boost::shared_ptr<CalendarTemplate>& cal,
						CalendarTemplateTableSync::Search(
							Env::GetOfficialEnv(),
							optional<string>(),
							optional<RegistryKeyType>(),
							true,
							true,
							0,
							optional<size_t>(),
							UP_LINKS_LOAD_LEVEL
					)	){
						calendars.push_back(cal.get());
					}
				}
			}
			else
			{
				BOOST_FOREACH(const CalendarTemplate::Registry::Map::value_type& it, Env::GetOfficialEnv().getRegistry<CalendarTemplate>())
				{
					calendars.push_back(it.second.get());
				}
			}

			BOOST_FOREACH(const CalendarTemplate* it, calendars)
			{
				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);

				it->toParametersMap(*calendarPM, true);

				map.insert(TAG_CALENDAR, calendarPM);
			}

			return map;
		}
		
		
		
		bool CalendarsListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string CalendarsListService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
