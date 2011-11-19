
//////////////////////////////////////////////////////////////////////////
/// ServiceApplyCalendarAction class implementation.
/// @file ServiceApplyCalendarAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ServiceApplyCalendarAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "CalendarTemplateTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "Fetcher.h"
#include "JourneyPatternTableSync.hpp"
#include "ServiceCalendarLinkUpdateAction.hpp"

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
		template<> const string FactorableTemplate<Action, pt::ServiceApplyCalendarAction>::FACTORY_KEY("ServiceApplyCalendarAction");
	}

	namespace pt
	{
		const string ServiceApplyCalendarAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "id";
		const string ServiceApplyCalendarAction::PARAMETER_CALENDAR_TEMPLATE_ID = Action_PARAMETER_PREFIX + "ct";
		const string ServiceApplyCalendarAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sd";
		const string ServiceApplyCalendarAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "ed";
		const string ServiceApplyCalendarAction::PARAMETER_PERIOD = Action_PARAMETER_PREFIX + "pe";
		const string ServiceApplyCalendarAction::PARAMETER_ADD = Action_PARAMETER_PREFIX + "ad";



		ParametersMap ServiceApplyCalendarAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			if(_journeyPattern.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _journeyPattern->getKey());
			}
			map.insert(PARAMETER_ADD, _add);
			if(!_period.is_special())
			{
				map.insert(PARAMETER_PERIOD, static_cast<int>(_period.days()));
			}
			if(_calendarTemplate.get())
			{
				map.insert(PARAMETER_CALENDAR_TEMPLATE_ID, _calendarTemplate->getKey());
			}
			if(!_startDate.is_not_a_date())
			{
				map.insert(PARAMETER_START_DATE, _startDate);
			}
			if(!_endDate.is_not_a_date())
			{
				map.insert(PARAMETER_END_DATE, _endDate);
			}
			return map;
		}



		void ServiceApplyCalendarAction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID));
			RegistryTableType tableId(decodeTableId(id));
			if(	tableId == ScheduledServiceTableSync::TABLE.ID ||
				tableId == ContinuousServiceTableSync::TABLE.ID
			){
				try
				{
					_service = Fetcher<SchedulesBasedService>::FetchEditable(id, *_env);
				}
				catch(ObjectNotFoundException<SchedulesBasedService>)
				{
					throw ActionException("No such service");
				}
			}
			else if(tableId == JourneyPatternTableSync::TABLE.ID)
			{
				try
				{
					_journeyPattern = JourneyPatternTableSync::Get(id, *_env);
					ScheduledServiceTableSync::Search(*_env, _journeyPattern->getKey());
				}
				catch(ObjectNotFoundException<JourneyPattern>)
				{
					throw ActionException("No such journey pattern");
				}
			}
			else
			{
				throw ActionException("Forbidden table for the specified id");
			}

			RegistryKeyType cid(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_TEMPLATE_ID, 0));
			if(cid > 0) try
			{
				_calendarTemplate = CalendarTemplateTableSync::Get(cid, *_env);
			}
			catch(ObjectNotFoundException<CalendarTemplate>&)
			{
				throw ActionException("No such calendar");
			}

			_period = days(map.getDefault<size_t>(PARAMETER_PERIOD, 1));
			if(_period.days() < 1)
			{
				throw ActionException("Period must be at least one day long");
			}

			_startDate = from_string(map.get<string>(PARAMETER_START_DATE));
			_endDate = from_string(map.get<string>(PARAMETER_END_DATE));

			if(_startDate > _endDate)
			{
				throw ActionException("The start date must be after the end date");
			}

			_add = map.getDefault<bool>(PARAMETER_ADD, true);
		}



		void ServiceApplyCalendarAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			// Selection of the services to update
			ServiceSet services;
			if(_journeyPattern.get())
			{
				services = _journeyPattern->getServices();
			}
			else
			{
				services.insert(_service.get());
			}

			// Loop on each service
			DBTransaction transaction;
			BOOST_FOREACH(Service* itService, services)
			{
				SchedulesBasedService* service(dynamic_cast<SchedulesBasedService*>(itService));
				if(service == NULL)
				{
					continue;
				}

				Calendar result;
				for (date d = _startDate; d <= _endDate; d += _period)
				{
					result.setActive(d);
				}
				if(_calendarTemplate.get())
				{
					result = _calendarTemplate->getResult(result);
				}

				if(_add)
				{
					*service |= result;
				}
				else
				{
					service->subDates(result);
				}

				Fetcher<SchedulesBasedService>::FetchSave(*service, transaction);
			}
			transaction.run();

			// Stores dates in session variable
			if(_startDate != _endDate)
			{
				if(!_startDate.is_not_a_date() && !_startDate.is_special())
				{
					request.getSession()->setSessionVariable(ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_START_DATE, to_iso_extended_string(_startDate));
				}
				if(!_endDate.is_not_a_date() && !_endDate.is_special())
				{
					request.getSession()->setSessionVariable(ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_END_DATE, to_iso_extended_string(_endDate));
				}
			}
			if(_calendarTemplate.get())
			{
				request.getSession()->setSessionVariable(ServiceCalendarLinkUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID, lexical_cast<string>(_calendarTemplate->getKey()));
			}

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool ServiceApplyCalendarAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}



		ServiceApplyCalendarAction::ServiceApplyCalendarAction():
			FactorableTemplate<server::Action, ServiceApplyCalendarAction>(),
			_add(true)
		{}
}	}
