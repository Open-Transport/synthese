
/** CalendarTemplatePropertiesUpdateAction class implementation.
	@file CalendarTemplatePropertiesUpdateAction.cpp
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "CalendarTemplatePropertiesUpdateAction.h"
#include "CalendarRight.h"
#include "Request.h"
#include "CalendarTemplateTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, calendar::CalendarTemplatePropertiesUpdateAction>::FACTORY_KEY("CalendarTemplatePropertiesUpdateAction");
	}

	namespace calendar
	{
		const string CalendarTemplatePropertiesUpdateAction::PARAMETER_CALENDAR_ID = Action_PARAMETER_PREFIX + "ca";
		const string CalendarTemplatePropertiesUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string CalendarTemplatePropertiesUpdateAction::PARAMETER_CATEGORY = Action_PARAMETER_PREFIX + "cc";

		
		
		ParametersMap CalendarTemplatePropertiesUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_calendar.get()) map.insert(PARAMETER_CALENDAR_ID, _calendar->getKey());
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_CATEGORY, static_cast<int>(_category));
			return map;
		}
		
		
		
		void CalendarTemplatePropertiesUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_calendar = CalendarTemplateTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID),
					*_env
				);
			}
			catch(ObjectNotFoundException<CalendarTemplate>& e)
			{
				throw ActionException("No such calendar", e, *this);
			}

			_name = map.get<string>(PARAMETER_NAME);
			if (_name.empty())
				throw ActionException("Bad value for name parameter ");	
			
			CalendarTemplateTableSync::SearchResult r(
				CalendarTemplateTableSync::Search(*_env, _name, _calendar->getKey(), false, true, 0, 1)
			);
			if(!r.empty())
				throw ActionException("A calendar named "+ _name +" already exists.");

			_category = static_cast<CalendarTemplate::Category>(map.get<int>(PARAMETER_CATEGORY));
		}
		
		
		
		void CalendarTemplatePropertiesUpdateAction::run(Request& request)
		{
			_calendar->setText(_name);
			_calendar->setCategory(_category);

			CalendarTemplateTableSync::Save(_calendar.get());
		}
		
		
		
		bool CalendarTemplatePropertiesUpdateAction::isAuthorized(const Profile& profile
		) const {
			return profile.isAuthorized<CalendarRight>(WRITE);
		}



		void CalendarTemplatePropertiesUpdateAction::setCalendar( boost::shared_ptr<CalendarTemplate> value )
		{
			_calendar = value;
		}
	}
}
