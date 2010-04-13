
//////////////////////////////////////////////////////////////////////////
/// ServiceApplyCalendarAction class implementation.
/// @file ServiceApplyCalendarAction.cpp
/// @author Hugues Romain
/// @date 2010
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
#include "ServiceApplyCalendarAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "SchedulesBasedService.h"
#include "CalendarTemplateTableSync.h"
#include "Fetcher.h"

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
			}
			return map;
		}
		
		
		
		void ServiceApplyCalendarAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_service = Fetcher<SchedulesBasedService>::FetchEditable(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID), *_env);
			}
			catch(ObjectNotFoundException<SchedulesBasedService>)
			{
				throw ActionException("No such service");
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

			_startDate = date_from_iso_string(map.get<string>(PARAMETER_START_DATE));
			_endDate = date_from_iso_string(map.get<string>(PARAMETER_END_DATE));

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

			Calendar result(*_service);
			for (date d = _startDate; d <= _endDate; d += _period)
			{
				result.setActive(d);
			}
			result = _calendarTemplate->getResult(result);

			if(_add)
			{
				*_service |= result;
			}
			else
			{
				_service->subDates(result);
			}

			Fetcher<SchedulesBasedService>::FetchSave(*_service);

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
		{

		}
	}
}
