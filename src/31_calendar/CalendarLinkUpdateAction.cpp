
//////////////////////////////////////////////////////////////////////////
/// CalendarLinkUpdateAction class implementation.
/// @file CalendarLinkUpdateAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "CalendarLinkUpdateAction.hpp"

#include "ActionException.h"
#include "CalendarTemplateElementTableSync.h"
#include "ParametersMap.h"
#include "Request.h"
#include "Session.h"
#include "Fetcher.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarLink.hpp"
#include "CalendarLinkTableSync.hpp"
#include "BaseCalendarUpdateAction.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<Action, CalendarLinkUpdateAction>::FACTORY_KEY("ServiceCalendarLinkUpdate");
	}

	namespace calendar
	{
		const string CalendarLinkUpdateAction::PARAMETER_LINK_ID = Action_PARAMETER_PREFIX + "link_id";
		const string CalendarLinkUpdateAction::PARAMETER_CALENDAR_ID = Action_PARAMETER_PREFIX + "calendar_id";
		const string CalendarLinkUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "start_date";
		const string CalendarLinkUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "end_date";
		const string CalendarLinkUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID = Action_PARAMETER_PREFIX + "calendar_id";
		const string CalendarLinkUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID2 = Action_PARAMETER_PREFIX + "calendar_id2";



		ParametersMap CalendarLinkUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_link.get())
			{
				map.insert(PARAMETER_LINK_ID, _link->getKey());
			}
			if(_calendar)
			{
				map.insert(PARAMETER_CALENDAR_ID, _calendar->get() ? (*_calendar)->getKey() : RegistryKeyType(0));
			}
			if(_minDate)
			{
				map.insert(PARAMETER_START_DATE, *_minDate);
			}
			if(_maxDate)
			{
				map.insert(PARAMETER_END_DATE, *_maxDate);
			}
			if(_calendarTpl)
			{
				map.insert(PARAMETER_CALENDAR_TEMPLATE_ID, _calendarTpl->get() ? (*_calendarTpl)->getKey() : RegistryKeyType(0));
			}
			if(_calendarTpl2)
			{
				map.insert(PARAMETER_CALENDAR_TEMPLATE_ID2, _calendarTpl2->get() ? (*_calendarTpl2)->getKey() : RegistryKeyType(0));
			}
			return map;
		}



		void CalendarLinkUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Link
			if(map.isDefined(PARAMETER_LINK_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_LINK_ID));
				try
				{
					_link = CalendarLinkTableSync::GetEditable(id, *_env);
				}
				catch(ObjectNotFoundException<CalendarLink>&)
				{
					throw ActionException("No such calendar link");
				}
			}
			else
			{
				_link = boost::shared_ptr<CalendarLink>(new CalendarLink);
			}

			// Service
			if(map.isDefined(PARAMETER_CALENDAR_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID));
				if(id > 0) try
				{
					_calendar = dynamic_pointer_cast<Calendar>(
						Fetcher<Calendar>::FetchEditable(id, *_env)
					);
					if(!_calendar.get())
					{
						throw ActionException("Bad calendar type");
					}
				}
				catch(ObjectNotFoundException<Calendar>&)
				{
					throw ActionException("No such calendar");
				}
			}

			// Min date
			if(map.isDefined(PARAMETER_START_DATE))
			{
				_minDate =
					map.get<string>(PARAMETER_START_DATE).empty() ?
					date(neg_infin) :
					from_string(map.get<string>(PARAMETER_START_DATE))
				;
			}

			// End date
			if(map.isDefined(PARAMETER_END_DATE))
			{
				_maxDate =
					map.get<string>(PARAMETER_END_DATE).empty() ?
					date(pos_infin) :
					from_string(map.get<string>(PARAMETER_END_DATE))
				;
			}

			// Calendar template
			if(map.isDefined(PARAMETER_CALENDAR_TEMPLATE_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_CALENDAR_TEMPLATE_ID));
				if(id > 0) try
				{
					_calendarTpl = CalendarTemplateTableSync::GetEditable(id, *_env);
					CalendarTemplateElementTableSync::Search(*_env, id);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw ActionException("No such calendar template");
				}
			}

			// Calendar template 2
			if(map.isDefined(PARAMETER_CALENDAR_TEMPLATE_ID2))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_CALENDAR_TEMPLATE_ID2));
				if(id > 0) try
				{
					_calendarTpl2 = CalendarTemplateTableSync::GetEditable(id, *_env);
					CalendarTemplateElementTableSync::Search(*_env, id);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw ActionException("No such calendar template 2");
				}
			}
		}



		void CalendarLinkUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			// Service
			if(_calendar)
			{
				_link->setCalendar(_calendar->get());
			}

			// Start date
			if(_minDate)
			{
				_link->setStartDate(*_minDate);
				if(!_minDate->is_not_a_date() && !_minDate->is_special())
				{
					request.getSession()->setSessionVariable(
						BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_START_DATE,
						to_iso_extended_string(*_minDate)
					);
				}
			}

			// End date
			if(_maxDate)
			{
				_link->setEndDate(*_maxDate);
				if(!_maxDate->is_not_a_date() && !_maxDate->is_special())
				{
					request.getSession()->setSessionVariable(
						BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_END_DATE,
						to_iso_extended_string(*_maxDate)
					);
				}
			}

			// Calendar template
			if(_calendarTpl)
			{
				_link->setCalendarTemplate(_calendarTpl->get());
				if(_calendarTpl->get())
				{
					request.getSession()->setSessionVariable(
						BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID,
						lexical_cast<string>((*_calendarTpl)->getKey())
					);
				}
			}

			// Calendar template 2
			if(_calendarTpl2)
			{
				_link->setCalendarTemplate2(_calendarTpl2->get());
				if(_calendarTpl2->get())
				{
					request.getSession()->setSessionVariable(
						BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID2,
						lexical_cast<string>((*_calendarTpl2)->getKey())
					);
				}
			}

			// Saving
			CalendarLinkTableSync::Save(_link.get());

			// Return of the created object id
			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_link->getKey());
			}

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool CalendarLinkUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true; // TODO session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
}	}
