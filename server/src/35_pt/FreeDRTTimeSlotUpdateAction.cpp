
//////////////////////////////////////////////////////////////////////////
/// FreeDRTTimeSlotUpdateAction class implementation.
/// @file FreeDRTTimeSlotUpdateAction.cpp
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

#include "FreeDRTTimeSlotUpdateAction.hpp"

#include "ActionException.h"
#include "CommercialLineTableSync.h"
#include "FreeDRTAreaTableSync.hpp"
#include "FreeDRTTimeSlotTableSync.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTUseRuleTableSync.h"
#include "Request.h"
#include "RollingStock.hpp"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::FreeDRTTimeSlotUpdateAction>::FACTORY_KEY("FreeDRTTimeSlotUpdate");
	}

	namespace pt
	{
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_TIME_SLOT_ID = Action_PARAMETER_PREFIX + "id";
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_AREA_ID = Action_PARAMETER_PREFIX + "area_id";
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_SERVICE_NUMBER = Action_PARAMETER_PREFIX + "service_number";
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_FIRST_DEPARTURE = Action_PARAMETER_PREFIX + "first_departure";
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_LAST_ARRIVAL = Action_PARAMETER_PREFIX + "last_arrival";
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_MAX_CAPACITY = Action_PARAMETER_PREFIX + "max_capacity";
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_COMMERCIAL_SPEED = Action_PARAMETER_PREFIX + "commercial_speed";
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_MAX_SPEED = Action_PARAMETER_PREFIX + "max_speed";
		const string FreeDRTTimeSlotUpdateAction::PARAMETER_USE_RULES = Action_PARAMETER_PREFIX + "use_rules";

		ParametersMap FreeDRTTimeSlotUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_timeSlot.get())
			{
				map.insert(PARAMETER_TIME_SLOT_ID, _timeSlot->getKey());
			}
			if(_area.get())
			{
				map.insert(PARAMETER_AREA_ID, _area->getKey());
			}
			if(_serviceNumber)
			{
				map.insert(PARAMETER_SERVICE_NUMBER, *_serviceNumber);
			}
			if(_firstDeparture)
			{
				map.insert(PARAMETER_FIRST_DEPARTURE, *_firstDeparture);
			}
			if(_lastArrival)
			{
				map.insert(PARAMETER_LAST_ARRIVAL, *_lastArrival);
			}
			if(_maxCapacity)
			{
				if(*_maxCapacity)
				{
					map.insert(PARAMETER_MAX_CAPACITY, **_maxCapacity);
				}
				else
				{
					map.insert(PARAMETER_MAX_CAPACITY, string());
				}
			}
			if(_commercialSpeed)
			{
				map.insert(PARAMETER_COMMERCIAL_SPEED, *_commercialSpeed);
			}
			if(_maxSpeed)
			{
				map.insert(PARAMETER_MAX_SPEED, *_maxSpeed);
			}
			if(_useRules)
			{
				map.insert(PARAMETER_USE_RULES, PTUseRuleTableSync::SerializeUseRules(*_useRules));
			}

			// Calendar
			_getCalendarUpdateParametersMap(map);

			return map;
		}



		void FreeDRTTimeSlotUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// The object
			if(map.getOptional<RegistryKeyType>(PARAMETER_TIME_SLOT_ID)) try
			{
				_timeSlot = FreeDRTTimeSlotTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TIME_SLOT_ID), *_env);
			}
			catch (ObjectNotFoundException<FreeDRTTimeSlot>&)
			{
				throw ActionException("No such time slot");
			}
			else
			{
				_timeSlot.reset(new FreeDRTTimeSlot);
			}

			// Area
			if(map.getDefault<RegistryKeyType>(PARAMETER_AREA_ID, 0)) try
			{
				_area = FreeDRTAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_AREA_ID), *_env);
			}
			catch (ObjectNotFoundException<FreeDRTArea>&)
			{
				throw ActionException("No such area");
			}

			// Service number
			if(map.isDefined(PARAMETER_SERVICE_NUMBER))
			{
				_serviceNumber = map.get<string>(PARAMETER_SERVICE_NUMBER);
			}

			// First departure
			if(map.isDefined(PARAMETER_FIRST_DEPARTURE))
			{
				_firstDeparture = map.get<string>(PARAMETER_FIRST_DEPARTURE).empty() ?
					time_duration(not_a_date_time) :
					duration_from_string(map.get<string>(PARAMETER_FIRST_DEPARTURE));
			}

			// Last arrival
			if(map.isDefined(PARAMETER_LAST_ARRIVAL))
			{
				_lastArrival = map.get<string>(PARAMETER_LAST_ARRIVAL).empty() ?
					time_duration(not_a_date_time) :
					duration_from_string(map.get<string>(PARAMETER_LAST_ARRIVAL));
			}

			// Max capacity
			if(map.isDefined(PARAMETER_MAX_CAPACITY))
			{
				_maxCapacity = map.getOptional<size_t>(PARAMETER_MAX_CAPACITY);
			}

			// Commercial speed
			if(map.isDefined(PARAMETER_COMMERCIAL_SPEED))
			{
				_commercialSpeed = map.get<double>(PARAMETER_COMMERCIAL_SPEED);
			}

			// Max speed
			if(map.isDefined(PARAMETER_MAX_SPEED))
			{
				_maxSpeed =  map.get<double>(PARAMETER_MAX_SPEED);
			}

			// Use rules
			if(map.isDefined(PARAMETER_USE_RULES))
			{
				_useRules = PTUseRuleTableSync::UnserializeUseRules(
					map.get<string>(PARAMETER_USE_RULES),
					*_env
				);
			}

			// Calendar
			_setCalendarUpdateFromParametersMap(*_env, map);
		}



		void FreeDRTTimeSlotUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			if(_area.get())
			{
				_timeSlot->setArea(_area.get());
			}
			if(_serviceNumber)
			{
				_timeSlot->setServiceNumber(*_serviceNumber);
			}
			if(_firstDeparture)
			{
				_timeSlot->setFirstDeparture(*_firstDeparture);
			}
			if(_lastArrival)
			{
				_timeSlot->setLastArrival(*_lastArrival);
			}
			if(_maxCapacity)
			{
				_timeSlot->setMaxCapacity(*_maxCapacity);
			}
			if(_commercialSpeed)
			{
				_timeSlot->setCommercialSpeed(*_commercialSpeed);
			}
			if(_maxSpeed)
			{
				_timeSlot->setMaxSpeed(*_maxSpeed);
			}
			if(_useRules)
			{
				_timeSlot->setRules(*_useRules);
			}

			// Calendar
			_doCalendarUpdate(*_timeSlot, request);

			FreeDRTTimeSlotTableSync::Save(_timeSlot.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_timeSlot->getKey());
			}
		}



		bool FreeDRTTimeSlotUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
}	}
