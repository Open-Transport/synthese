
//////////////////////////////////////////////////////////////////////////
/// ServiceTimetableUpdateAction class implementation.
/// @file ServiceTimetableUpdateAction.cpp
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
#include "ServiceTimetableUpdateAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "SchedulesBasedService.h"
#include "Fetcher.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceTimetableUpdateAction>::FACTORY_KEY("ServiceTimetableUpdateAction");
	}

	namespace pt
	{
		const string ServiceTimetableUpdateAction::PARAMETER_UPDATE_ARRIVAL = Action_PARAMETER_PREFIX + "ar";
		const string ServiceTimetableUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "id";
		const string ServiceTimetableUpdateAction::PARAMETER_RANK = Action_PARAMETER_PREFIX + "rk";
		const string ServiceTimetableUpdateAction::PARAMETER_SHIFTING_DELAY = Action_PARAMETER_PREFIX + "sd";
		const string ServiceTimetableUpdateAction::PARAMETER_TIME = Action_PARAMETER_PREFIX + "ti";
		
		
		
		ParametersMap ServiceTimetableUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
				map.insert(PARAMETER_RANK, _rank);
				map.insert(PARAMETER_UPDATE_ARRIVAL, _updateArrival);
				if(!_shifting_delay.is_not_a_date_time())
				{
					map.insert(PARAMETER_SHIFTING_DELAY, _shifting_delay.total_seconds() / 60);
				}
				else if(!_time.is_not_a_date_time())
				{
					map.insert(PARAMETER_TIME, _time);
				}
			}
			return map;
		}
		
		
		
		void ServiceTimetableUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_service = Fetcher<SchedulesBasedService>::FetchEditable(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID),
					*_env,
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch(ObjectNotFoundException<SchedulesBasedService>&)
			{
				throw ActionException("No such service");
			}

			_rank = map.get<size_t>(PARAMETER_RANK);
			if(_rank >= _service->getDepartureSchedules(false).size())
			{
				throw ActionException("Invalid rank");
			}

			_updateArrival = map.get<bool>(PARAMETER_UPDATE_ARRIVAL);

			if(map.getOptional<int>(PARAMETER_SHIFTING_DELAY))
			{
				_shifting_delay = minutes(map.get<int>(PARAMETER_SHIFTING_DELAY));
			}
			else
			{
				_time = duration_from_string(map.get<string>(PARAMETER_TIME));
			}
		}
		
		
		
		void ServiceTimetableUpdateAction::run(
			Request& request
		){
			SchedulesBasedService::Schedules departureSchedules(_service->getDepartureSchedules(false));
			SchedulesBasedService::Schedules arrivalSchedules(_service->getArrivalSchedules(false));

			if(!_shifting_delay.is_not_a_date_time())
			{
				for(SchedulesBasedService::Schedules::iterator it(departureSchedules.begin() + _rank); it != departureSchedules.end(); ++it)
				{
					*it += _shifting_delay;
				}
				for(SchedulesBasedService::Schedules::iterator it(arrivalSchedules.begin() + _rank + (_updateArrival ? 0 : 1)); it != arrivalSchedules.end(); ++it)
				{
					*it += _shifting_delay;
				}
			}
			else
			{
				if(_updateArrival)
				{
					arrivalSchedules[_rank] = _time;
				}
				else
				{
					departureSchedules[_rank] = _time;
				}
			}

			_service->setSchedules(departureSchedules, arrivalSchedules);

			Fetcher<SchedulesBasedService>::FetchSave(*_service);

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool ServiceTimetableUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}



		ServiceTimetableUpdateAction::ServiceTimetableUpdateAction():
		FactorableTemplate<server::Action, ServiceTimetableUpdateAction>(),
		_time(not_a_date_time), _shifting_delay(not_a_date_time)
		{

		}
	}
}
