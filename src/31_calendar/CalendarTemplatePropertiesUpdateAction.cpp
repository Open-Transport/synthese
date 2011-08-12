
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



		ParametersMap CalendarTemplatePropertiesUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_calendar.get())
			{
				map.insert(PARAMETER_CALENDAR_ID, _calendar->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_category)
			{
				map.insert(PARAMETER_CATEGORY, static_cast<int>(*_category));
			}
			if(_dataSourceLinks)
			{
				map.insert(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS, ImportableTableSync::SerializeDataSourceLinks(*_dataSourceLinks));
			}
			return map;
		}



		void CalendarTemplatePropertiesUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Calendar
			if(map.isDefined(PARAMETER_CALENDAR_ID))
			{ // Load
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
			}
			else
			{ // Creation
				_calendar.reset(new CalendarTemplate);
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
				_category = static_cast<CalendarTemplate::Category>(map.get<int>(PARAMETER_CATEGORY));
			}

			// Datasource
			if(map.isDefined(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS))
			{
				_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(map.get<string>(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS), *_env);
			}
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
			if(_dataSourceLinks)
			{
				_calendar->setDataSourceLinks(*_dataSourceLinks);
			}

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



		void CalendarTemplatePropertiesUpdateAction::setCalendar( boost::shared_ptr<CalendarTemplate> value )
		{
			_calendar = value;
		}
}	}
