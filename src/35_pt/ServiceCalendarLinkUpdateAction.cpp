
//////////////////////////////////////////////////////////////////////////
/// ServiceCalendarLinkUpdateAction class implementation.
/// @file ServiceCalendarLinkUpdateAction.cpp
/// @author Hugues Romain
/// @date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ServiceCalendarLinkUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "Fetcher.h"
#include "NonPermanentService.h"
#include "CalendarTemplateTableSync.h"
#include "ServiceCalendarLink.hpp"
#include "ServiceCalendarLinkTableSync.hpp"

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
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceCalendarLinkUpdateAction>::FACTORY_KEY("ServiceCalendarLinkUpdate");
	}

	namespace pt
	{
		const string ServiceCalendarLinkUpdateAction::PARAMETER_LINK_ID = Action_PARAMETER_PREFIX + "link_id";
		const string ServiceCalendarLinkUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "service_id";
		const string ServiceCalendarLinkUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "start_date";
		const string ServiceCalendarLinkUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "end_date";
		const string ServiceCalendarLinkUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID = Action_PARAMETER_PREFIX + "calendar_id";
		const string ServiceCalendarLinkUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID2 = Action_PARAMETER_PREFIX + "calendar_id2";

		const string ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_START_DATE("service_admin_start_date");
		const string ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_END_DATE("service_admin_end_date");
		const string ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID("service_admin_calendar_template_id");
		const string ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID2("service_admin_calendar_template_id2");


		ParametersMap ServiceCalendarLinkUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_link.get())
			{
				map.insert(PARAMETER_LINK_ID, _link->getKey());
			}
			if(_service)
			{
				map.insert(PARAMETER_SERVICE_ID, _service->get() ? (*_service)->getKey() : RegistryKeyType(0));
			}
			if(_minDate)
			{
				map.insert(PARAMETER_START_DATE, *_minDate);
			}
			if(_maxDate)
			{
				map.insert(PARAMETER_END_DATE, *_maxDate);
			}
			if(_calendar)
			{
				map.insert(PARAMETER_CALENDAR_TEMPLATE_ID, _calendar->get() ? (*_calendar)->getKey() : RegistryKeyType(0));
			}
			if(_calendar2)
			{
				map.insert(PARAMETER_CALENDAR_TEMPLATE_ID2, _calendar2->get() ? (*_calendar2)->getKey() : RegistryKeyType(0));
			}
			return map;
		}



		void ServiceCalendarLinkUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Link
			if(map.isDefined(PARAMETER_LINK_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_LINK_ID));
				try
				{
					_link = ServiceCalendarLinkTableSync::GetEditable(id, *_env);
				}
				catch(ObjectNotFoundException<ServiceCalendarLink>&)
				{
					throw ActionException("No such calendar link");
				}
			}
			else
			{
				_link = shared_ptr<ServiceCalendarLink>(new ServiceCalendarLink);
			}

			// Service
			if(map.isDefined(PARAMETER_SERVICE_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID));
				if(id > 0) try
				{
					_service = dynamic_pointer_cast<NonPermanentService,Service>(
						Fetcher<Service>::FetchEditable(id,	*_env)
					);
					if(!_service.get())
					{
						throw ActionException("Bad service type");
					}
				}
				catch(ObjectNotFoundException<Service>&)
				{
					throw ActionException("No such service");
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
					_calendar = CalendarTemplateTableSync::GetEditable(id, *_env);
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
					_calendar2 = CalendarTemplateTableSync::GetEditable(id, *_env);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw ActionException("No such calendar template 2");
				}
			}
		}



		void ServiceCalendarLinkUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			// Service
			if(_service)
			{
				_link->setService(_service->get());
			}

			// Start date
			if(_minDate)
			{
				_link->setStartDate(*_minDate);
				if(!_minDate->is_not_a_date())
				{
					request.getSession()->setSessionVariable(SESSION_VARIABLE_SERVICE_ADMIN_START_DATE, to_iso_extended_string(*_minDate));
				}
			}

			// End date
			if(_maxDate)
			{
				_link->setEndDate(*_maxDate);
				if(!_maxDate->is_not_a_date())
				{
					request.getSession()->setSessionVariable(SESSION_VARIABLE_SERVICE_ADMIN_END_DATE, to_iso_extended_string(*_maxDate));
				}
			}

			// Calendar template
			if(_calendar)
			{
				_link->setCalendarTemplate(_calendar->get());
				if(_calendar->get())
				{
					request.getSession()->setSessionVariable(SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID, lexical_cast<string>((*_calendar)->getKey()));
				}
			}
			
			// Calendar template 2
			if(_calendar2)
			{
				_link->setCalendarTemplate2(_calendar2->get());
				if(_calendar2->get())
				{
					request.getSession()->setSessionVariable(SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID2, lexical_cast<string>((*_calendar2)->getKey()));
				}
			}

			// Saving
			ServiceCalendarLinkTableSync::Save(_link.get());

			// Return of the created object id
			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_link->getKey());
			}

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool ServiceCalendarLinkUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
}	}
