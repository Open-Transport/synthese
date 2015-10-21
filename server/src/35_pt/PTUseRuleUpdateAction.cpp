
//////////////////////////////////////////////////////////////////////////
/// PTUseRuleUpdateAction class implementation.
/// @file PTUseRuleUpdateAction.cpp
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

#include "PTUseRuleUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "PTUseRuleTableSync.h"
#include "Fare.hpp"
#include "FareTableSync.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace fare;
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::PTUseRuleUpdateAction>::FACTORY_KEY("PTUseRuleUpdateAction");
	}

	namespace pt
	{
		const string PTUseRuleUpdateAction::PARAMETER_RULE_ID = Action_PARAMETER_PREFIX + "id";
		const string PTUseRuleUpdateAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "ti";
		const string PTUseRuleUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string PTUseRuleUpdateAction::PARAMETER_CAPACITY = Action_PARAMETER_PREFIX + "ca";
		const string PTUseRuleUpdateAction::PARAMETER_ORIGIN_IS_REFERENCE = Action_PARAMETER_PREFIX + "or";
		const string PTUseRuleUpdateAction::PARAMETER_FARE_ID = Action_PARAMETER_PREFIX + "fi";
		const string PTUseRuleUpdateAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";
		const string PTUseRuleUpdateAction::PARAMETER_MAX_DELAY_DAYS = Action_PARAMETER_PREFIX + "mx";
		const string PTUseRuleUpdateAction::PARAMETER_MIN_DELAY_DAYS = Action_PARAMETER_PREFIX + "md";
		const string PTUseRuleUpdateAction::PARAMETER_MIN_DELAY_MINUTES = Action_PARAMETER_PREFIX + "mm";
		const string PTUseRuleUpdateAction::PARAMETER_MIN_DELAY_MINUTES_EXTERNAL = Action_PARAMETER_PREFIX + "me";
		const string PTUseRuleUpdateAction::PARAMETER_HOUR_DEADLINE = Action_PARAMETER_PREFIX + "dl";
		const string PTUseRuleUpdateAction::PARAMETER_RESERVATION_MIN_DEPARTURE_TIME = Action_PARAMETER_PREFIX + "_reservation_min_departure_time";
		const string PTUseRuleUpdateAction::PARAMETER_RESERVATION_FORBIDDEN_DAYS = Action_PARAMETER_PREFIX + "_reservation_forbidden_days";
		const string PTUseRuleUpdateAction::PARAMETER_FORBIDDEN_IN_DEPARTURE_BOARDS = Action_PARAMETER_PREFIX + "fb";
		const string PTUseRuleUpdateAction::PARAMETER_FORBIDDEN_IN_TIMETABLES = Action_PARAMETER_PREFIX + "ft";
		const string PTUseRuleUpdateAction::PARAMETER_FORBIDDEN_IN_JOURNEY_PLANNER = Action_PARAMETER_PREFIX + "fj";

		ParametersMap PTUseRuleUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_rule.get())
			{
				map.insert(PARAMETER_RULE_ID, _rule->getKey());
			}
			if(_template.get())
			{
				map.insert(PARAMETER_TEMPLATE_ID, _template->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_capacity)
			{
				if(*_capacity)
				{
					map.insert(PARAMETER_CAPACITY, static_cast<int>(**_capacity));
				}
				else
				{
					map.insert(PARAMETER_CAPACITY, string());
				}
			}
			if(_originIsReference)
			{
				map.insert(PARAMETER_ORIGIN_IS_REFERENCE, *_originIsReference);
			}
			if(_fare)
			{
				map.insert(PARAMETER_FARE_ID, _fare->get() ? (*_fare)->getKey() : RegistryKeyType(0));
			}
			if(_type)
			{
				map.insert(PARAMETER_TYPE, static_cast<int>(*_type));
			}
			if(_maxDelayDays)
			{
				if(*_maxDelayDays)
				{
					map.insert(PARAMETER_MAX_DELAY_DAYS, static_cast<int>((*_maxDelayDays)->days()));
				}
				else
				{
					map.insert(PARAMETER_MAX_DELAY_DAYS, string());
				}
			}
			if(_minDelayDays)
			{
				map.insert(PARAMETER_MIN_DELAY_DAYS, static_cast<int>(_minDelayDays->days()));
			}
			if(_minDelayMinutes)
			{
				map.insert(PARAMETER_MIN_DELAY_MINUTES, _minDelayMinutes->total_seconds() / 60);
			}
			if(_minDelayMinutesExternal)
			{
				map.insert(PARAMETER_MIN_DELAY_MINUTES_EXTERNAL, _minDelayMinutesExternal->total_seconds() / 60);
			}
			if(_hourDeadLine)
			{
				if(!_hourDeadLine->is_not_a_date_time())
				{
					map.insert(PARAMETER_HOUR_DEADLINE, to_simple_string(*_hourDeadLine));
				}
				else
				{
					map.insert(PARAMETER_HOUR_DEADLINE, string());
				}
			}

			// Reservation min departure time
			if(_reservationMinDepartureTime)
			{
				if(!_reservationMinDepartureTime->is_not_a_date_time())
				{
					map.insert(PARAMETER_RESERVATION_MIN_DEPARTURE_TIME, to_simple_string(*_reservationMinDepartureTime));
				}
				else
				{
					map.insert(PARAMETER_RESERVATION_MIN_DEPARTURE_TIME, string());
				}
			}

			// Reservation forbidden days
			if(_reservationForbiddenDays)
			{
				map.insert(PARAMETER_RESERVATION_FORBIDDEN_DAYS, PTUseRule::SerializeForbiddenDays(*_reservationForbiddenDays));
			}

			if(_forbiddenInDepartureBoards)
			{
				map.insert(PARAMETER_FORBIDDEN_IN_DEPARTURE_BOARDS, *_forbiddenInDepartureBoards);
			}
			if(_forbiddenInTimetables)
			{
				map.insert(PARAMETER_FORBIDDEN_IN_TIMETABLES, *_forbiddenInTimetables);
			}
			if(_forbiddenInJourneyPlanner)
			{
				map.insert(PARAMETER_FORBIDDEN_IN_JOURNEY_PLANNER, *_forbiddenInJourneyPlanner);
			}
			return map;
		}



		void PTUseRuleUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(PARAMETER_RULE_ID) && map.get<RegistryKeyType>(PARAMETER_RULE_ID))
			{
				try
				{
					_rule = PTUseRuleTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_RULE_ID), *_env);
				}
				catch(ObjectNotFoundException<PTUseRule>&)
				{
					throw ActionException("No such use rule");
				}
			}
			else
			{
				// Template
				RegistryKeyType tid(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID, 0));
				if(tid > 0)	try
				{
					_template = PTUseRuleTableSync::Get(tid, *_env);
					_rule.reset(new PTUseRule(*_template));
					_rule->setKey(0);
				}
				catch(ObjectNotFoundException<PTUseRule>&)
				{
					throw ActionException("No such template");
				}
				else
				{
					_rule.reset(new PTUseRule);
				}
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.getDefault<string>(PARAMETER_NAME);
			}

			if(map.isDefined(PARAMETER_CAPACITY))
			{
				_capacity = map.getOptional<size_t>(PARAMETER_CAPACITY);
				if(_capacity && *_capacity == 0)
				{
					_capacity = UseRule::AccessCapacity();
				}
			}

			if(map.isDefined(PARAMETER_ORIGIN_IS_REFERENCE))
			{
				_originIsReference = map.getDefault<bool>(PARAMETER_ORIGIN_IS_REFERENCE, false);
			}

			if(map.isDefined(PARAMETER_FARE_ID))
			{
				RegistryKeyType fid(map.getDefault<RegistryKeyType>(PARAMETER_FARE_ID, 0));
				if(fid > 0) try
				{
					_fare = FareTableSync::GetEditable(fid, *_env);
				}
				catch(ObjectNotFoundException<Fare>&)
				{
					throw ActionException("No such fare");
				}
				else
				{
					_fare = boost::shared_ptr<Fare>();
				}
			}

			if(map.isDefined(PARAMETER_TYPE))
			{
				_type = static_cast<pt::ReservationRuleType>(map.getDefault<int>(PARAMETER_TYPE, 0));
			}

			if(map.isDefined(PARAMETER_MIN_DELAY_MINUTES))
			{
				_minDelayMinutes = minutes(map.getDefault<long>(PARAMETER_MIN_DELAY_MINUTES, 0));
			}
			
			if(map.isDefined(PARAMETER_MIN_DELAY_MINUTES_EXTERNAL))
			{
				_minDelayMinutesExternal = minutes(map.getDefault<long>(PARAMETER_MIN_DELAY_MINUTES_EXTERNAL, 0));
			}

			// Minimal delay (days)
			if(map.isDefined(PARAMETER_MIN_DELAY_DAYS))
			{
				_minDelayDays = days(map.getDefault<long>(PARAMETER_MIN_DELAY_DAYS, 0));
			}

			// Maximal delay (days)
			if(map.isDefined(PARAMETER_MAX_DELAY_DAYS))
			{
				_maxDelayDays = map.getDefault<long>(PARAMETER_MAX_DELAY_DAYS, 0) > 0 ?
					days(map.get<long>(PARAMETER_MAX_DELAY_DAYS)) :
					optional<date_duration>();
			}

			if(map.isDefined(PARAMETER_HOUR_DEADLINE))
			{
				if(!map.getDefault<string>(PARAMETER_HOUR_DEADLINE).empty())
				{
					_hourDeadLine = duration_from_string(map.get<string>(PARAMETER_HOUR_DEADLINE));
				}
				else
				{
					_hourDeadLine = time_duration(not_a_date_time);
				}
			}

			if(map.isDefined(PARAMETER_RESERVATION_MIN_DEPARTURE_TIME))
			{
				if(!map.getDefault<string>(PARAMETER_RESERVATION_MIN_DEPARTURE_TIME).empty())
				{
					_reservationMinDepartureTime = duration_from_string(map.get<string>(PARAMETER_RESERVATION_MIN_DEPARTURE_TIME));
				}
				else
				{
					_reservationMinDepartureTime = time_duration(not_a_date_time);
				}
			}

			if(map.isDefined(PARAMETER_RESERVATION_FORBIDDEN_DAYS))
			{
				_reservationForbiddenDays = PTUseRule::UnserializeForbiddenDays(map.get<string>(PARAMETER_RESERVATION_FORBIDDEN_DAYS));
			}

			if(map.isDefined(PARAMETER_FORBIDDEN_IN_DEPARTURE_BOARDS))
			{
				_forbiddenInDepartureBoards = map.get<bool>(PARAMETER_FORBIDDEN_IN_DEPARTURE_BOARDS);
			}

			if(map.isDefined(PARAMETER_FORBIDDEN_IN_TIMETABLES))
			{
				_forbiddenInTimetables = map.get<bool>(PARAMETER_FORBIDDEN_IN_TIMETABLES);
			}

			if(map.isDefined(PARAMETER_FORBIDDEN_IN_JOURNEY_PLANNER))
			{
				_forbiddenInJourneyPlanner = map.get<bool>(PARAMETER_FORBIDDEN_IN_JOURNEY_PLANNER);
			}
		}



		void PTUseRuleUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_name)
			{
				_rule->setName(*_name);
			}

			if(_capacity)
			{
				_rule->setAccessCapacity(*_capacity);
			}

			if(_originIsReference)
			{
				_rule->setOriginIsReference(*_originIsReference);
			}

			if(_fare)
			{
				_rule->setDefaultFare(_fare->get());
			}

			if(_type)
			{
				_rule->setReservationType(*_type);
			}

			if(_hourDeadLine)
			{
				_rule->setHourDeadLine(*_hourDeadLine);
			}

			if(_maxDelayDays)
			{
				_rule->setMaxDelayDays(*_maxDelayDays);
			}

			if(_minDelayDays)
			{
				_rule->setMinDelayDays(*_minDelayDays);
			}

			if(_minDelayMinutes)
			{
				_rule->setMinDelayMinutes(*_minDelayMinutes);
			}
			
			if(_minDelayMinutesExternal)
			{
				_rule->setMinDelayMinutesExternal(*_minDelayMinutesExternal);
			}

			if(_reservationMinDepartureTime)
			{
				_rule->setReservationMinDepartureTime(*_reservationMinDepartureTime);
			}

			if(_reservationForbiddenDays)
			{
				_rule->setReservationForbiddenDays(*_reservationForbiddenDays);
			}

			if(_forbiddenInDepartureBoards)
			{
				_rule->setForbiddenInDepartureBoards(*_forbiddenInDepartureBoards);
			}

			if(_forbiddenInTimetables)
			{
				_rule->setForbiddenInTimetables(*_forbiddenInTimetables);
			}

			if(_forbiddenInJourneyPlanner)
			{
				_rule->setForbiddenInJourneyPlanning(*_forbiddenInJourneyPlanner);
			}

			PTUseRuleTableSync::Save(_rule.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_rule->getKey());
			}
		}



		bool PTUseRuleUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
}	}
