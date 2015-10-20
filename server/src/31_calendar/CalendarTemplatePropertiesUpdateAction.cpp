
/** CalendarTemplatePropertiesUpdateAction class implementation.
	@file CalendarTemplatePropertiesUpdateAction.cpp
	@author Hugues
	@date 2009

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

#include "CalendarTemplatePropertiesUpdateAction.h"

#include "ActionException.h"
#include "CalendarTemplateElementTableSync.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "CalendarRight.h"
#include "Request.h"
#include "CalendarTemplateTableSync.h"
#include "ImportableAdmin.hpp"
#include "ImportableTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<Action, calendar::CalendarTemplatePropertiesUpdateAction>::FACTORY_KEY("CalendarTemplatePropertiesUpdateAction");
	}

	namespace calendar
	{
		const string CalendarTemplatePropertiesUpdateAction::PARAMETER_CALENDAR_ID = Action_PARAMETER_PREFIX + "ca";
		const string CalendarTemplatePropertiesUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string CalendarTemplatePropertiesUpdateAction::PARAMETER_CATEGORY = Action_PARAMETER_PREFIX + "cc";
		const string CalendarTemplatePropertiesUpdateAction::PARAMETER_PARENT_ID = Action_PARAMETER_PREFIX + "parent_id";



		ParametersMap CalendarTemplatePropertiesUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_calendar.get())
			{
				map.insert(PARAMETER_CALENDAR_ID, _calendar->getKey());
			}
			if(_parent)
			{
				map.insert(PARAMETER_PARENT_ID, (*_parent)->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_category)
			{
				map.insert(PARAMETER_CATEGORY, static_cast<int>(*_category));
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void CalendarTemplatePropertiesUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Calendar
			if(map.isDefined(PARAMETER_CALENDAR_ID))
			{ // Load
				try
				{
					RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID));
					_calendar = CalendarTemplateTableSync::GetEditable(id, *_env);
					CalendarTemplateElementTableSync::Search(*_env, id);
				}
				catch(ObjectNotFoundException<CalendarTemplate>& e)
				{
					throw ActionException("No such calendar", e, *this);
				}
			}
			else
			{ // Creation
				_calendar.reset(new CalendarTemplate);
			}

			// Parent
			if(map.isDefined(PARAMETER_PARENT_ID))
			{
				try
				{
					if(map.get<RegistryKeyType>(PARAMETER_PARENT_ID) != 0)
					{
						RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_PARENT_ID));
						_parent = CalendarTemplateTableSync::GetEditable(id, *_env);
						CalendarTemplateElementTableSync::Search(*_env, id);
					}
					else
					{
						// Calendars root
						_parent = boost::optional<boost::shared_ptr<CalendarTemplate> >();
					}
				}
				catch(ObjectNotFoundException<CalendarTemplate>& e)
				{
					throw ActionException("No such parent calendar", e, *this);
				}
			}
			// Name
			if(map.isDefined(PARAMETER_NAME) || !_calendar->getKey())
			{
				_name = map.getDefault<string>(PARAMETER_NAME);
				if (_name->empty())
				{
					throw ActionException("Bad value for name parameter : name must be non empty.");
				}

				CalendarTemplateTableSync::SearchResult r(
					CalendarTemplateTableSync::Search(*_env, *_name, _calendar->getKey(), false, true, 0, 1)
				);
				if(!r.empty())
				{
					throw ActionException("A calendar named "+ *_name +" already exists.");
				}
			}

			// Category
			if(map.isDefined(PARAMETER_CATEGORY))
			{
				_category = static_cast<CalendarTemplateCategory>(map.get<int>(PARAMETER_CATEGORY));
			}

			// Importable
			_setImportableUpdateFromParametersMap(*_env, map);
		}



		void CalendarTemplatePropertiesUpdateAction::run(Request& request)
		{
			if(_name)
			{
				_calendar->setName(*_name);
			}
			if(_category)
			{
				_calendar->setCategory(*_category);
			}
			if(_parent)
			{
				_calendar->setParent(_parent->get());
			}

			// Importable
			_doImportableUpdate(*_calendar, request);

			CalendarTemplateTableSync::Save(_calendar.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_calendar->getKey());
			}
		}



		bool CalendarTemplatePropertiesUpdateAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CalendarRight>(WRITE);
		}
}	}
