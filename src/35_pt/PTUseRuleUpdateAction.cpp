
//////////////////////////////////////////////////////////////////////////
/// PTUseRuleUpdateAction class implementation.
/// @file PTUseRuleUpdateAction.cpp
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
#include "PTUseRuleUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "PTUseRuleTableSync.h"
#include "FareTableSync.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
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
		const string PTUseRuleUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string PTUseRuleUpdateAction::PARAMETER_CAPACITY = Action_PARAMETER_PREFIX + "ca";
		const string PTUseRuleUpdateAction::PARAMETER_ORIGIN_IS_REFERENCE = Action_PARAMETER_PREFIX + "or";
		const string PTUseRuleUpdateAction::PARAMETER_FARE_ID = Action_PARAMETER_PREFIX + "fi";
		const string PTUseRuleUpdateAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";
		const string PTUseRuleUpdateAction::PARAMETER_MAX_DELAY_DAYS = Action_PARAMETER_PREFIX + "mx";
		const string PTUseRuleUpdateAction::PARAMETER_MIN_DELAY_DAYS = Action_PARAMETER_PREFIX + "md";
		const string PTUseRuleUpdateAction::PARAMETER_MIN_DELAY_MINUTES = Action_PARAMETER_PREFIX + "mm";
		const string PTUseRuleUpdateAction::PARAMETER_HOUR_DEADLINE = Action_PARAMETER_PREFIX + "dl";

		
		
		ParametersMap PTUseRuleUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_rule.get())
			{
				map.insert(PARAMETER_RULE_ID, _rule->getKey());
				map.insert(PARAMETER_NAME, _name);
				if(_capacity && *_capacity)
				{
					map.insert(PARAMETER_CAPACITY, static_cast<int>(*_capacity));
				}
				map.insert(PARAMETER_ORIGIN_IS_REFERENCE, _originIsReference);
				map.insert(PARAMETER_FARE_ID, _fare.get() ? _fare->getKey() : RegistryKeyType(0));
				map.insert(PARAMETER_TYPE, static_cast<int>(_type));
				if(_maxDelayDays)
				{
					map.insert(PARAMETER_MAX_DELAY_DAYS, static_cast<int>(_maxDelayDays->days()));
				}
				map.insert(PARAMETER_MIN_DELAY_DAYS, static_cast<int>(_minDelayDays.days()));
				map.insert(PARAMETER_MIN_DELAY_MINUTES, _minDelayMinutes.total_seconds() / 60);
				if(!_hourDeadLine.is_not_a_date_time())
				{
					map.insert(PARAMETER_HOUR_DEADLINE, to_simple_string(_hourDeadLine));
				}
			}
			return map;
		}
		
		
		
		void PTUseRuleUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_rule = PTUseRuleTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_RULE_ID), *_env);
			}
			catch(ObjectNotFoundException<PTUseRule>&)
			{
				throw ActionException("No such use rule");
			}

			_name = map.getDefault<string>(PARAMETER_NAME);
			_capacity = map.getOptional<size_t>(PARAMETER_NAME);
			if(_capacity && *_capacity == 0)
			{
				_capacity = UseRule::AccessCapacity();
			}
			_originIsReference = map.getDefault<bool>(PARAMETER_ORIGIN_IS_REFERENCE, false);

			RegistryKeyType fid(map.getDefault<RegistryKeyType>(PARAMETER_FARE_ID, 0));
			if(fid > 0) try
			{
				_fare = FareTableSync::Get(fid, *_env);
			}
			catch(ObjectNotFoundException<Fare>&)
			{
				throw ActionException("No such fare");
			}
			_type = static_cast<PTUseRule::ReservationRuleType>(map.getDefault<int>(PARAMETER_TYPE, 0));
			_minDelayMinutes = minutes(map.getDefault<long>(PARAMETER_MIN_DELAY_MINUTES, 0));
			_minDelayDays = days(map.getDefault<long>(PARAMETER_MIN_DELAY_DAYS, 0));
			_maxDelayDays = (map.getDefault<long>(PARAMETER_MAX_DELAY_DAYS, 0) > 0 ? days(map.get<long>(PARAMETER_MAX_DELAY_DAYS)) : optional<date_duration>());
			if(!map.getDefault<string>(PARAMETER_HOUR_DEADLINE).empty())
			{
				_hourDeadLine = duration_from_string(map.get<string>(PARAMETER_HOUR_DEADLINE));
			}
		}
		
		
		
		void PTUseRuleUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			_rule->setName(_name);
			_rule->setAccessCapacity(_capacity);
			_rule->setOriginIsReference(_originIsReference);
			_rule->setDefaultFare(_fare.get());
			_rule->setReservationType(_type);
			_rule->setHourDeadLine(_hourDeadLine);
			_rule->setMaxDelayDays(_maxDelayDays);
			_rule->setMinDelayDays(_minDelayDays);
			_rule->setMinDelayMinutes(_minDelayMinutes);
			
			PTUseRuleTableSync::Save(_rule.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool PTUseRuleUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
