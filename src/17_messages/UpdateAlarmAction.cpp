
/** UpdateAlarmAction class implementation.
	@file UpdateAlarmAction.cpp

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

#include "UpdateAlarmAction.h"

#include "SingleSentAlarm.h"
#include "AlarmTableSync.h"
#include "MessagesModule.h"

#include "TimeParseException.h"
#include "DateTime.h"
#include "MessagesRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace time;
	using namespace db;
	using namespace util;
	using namespace security;
	

	template<> const string util::FactorableTemplate<Action, messages::UpdateAlarmAction>::FACTORY_KEY("updatealarm");
	
	namespace messages
	{
		const string UpdateAlarmAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";
		const string UpdateAlarmAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sd";
		const string UpdateAlarmAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "ed";
		const string UpdateAlarmAction::PARAMETER_ENABLED = Action_PARAMETER_PREFIX + "en";
		const string UpdateAlarmAction::PARAMETER_ALARM_ID(Action_PARAMETER_PREFIX + "ai");


		ParametersMap UpdateAlarmAction::getParametersMap() const
		{
			ParametersMap map;
			if(_alarm.get())
			{
				map.insert(PARAMETER_ALARM_ID, _alarm->getKey());
			}
			return map;
		}
		
		void UpdateAlarmAction::setAlarmId(
			const util::RegistryKeyType id
		){
			try
			{
				_alarm = AlarmTableSync::GetEditable(id, _env);
				_singleSentAlarm = dynamic_pointer_cast<SingleSentAlarm, Alarm>(_alarm);
			}
			catch (ObjectNotFoundException<Alarm>& e)
			{
				throw ActionException("Message", id, FACTORY_KEY, e);
			}
		}

		void UpdateAlarmAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				setAlarmId(map.getUid(PARAMETER_ALARM_ID, true, FACTORY_KEY));
				_type = static_cast<AlarmLevel>(map.getInt(PARAMETER_TYPE, true, FACTORY_KEY));

				if (_singleSentAlarm.get())
				{
					_startDate = map.getDateTime(PARAMETER_START_DATE, true, FACTORY_KEY);
					_endDate = map.getDateTime(PARAMETER_END_DATE, true, FACTORY_KEY);
					_enabled = map.getBool(PARAMETER_ENABLED, true, false, FACTORY_KEY);
				}
			}
			catch(TimeParseException& e)
			{
				throw ActionException("Une date ou une heure est mal formée");
			}
		}

		UpdateAlarmAction::UpdateAlarmAction()
			: FactorableTemplate<Action,UpdateAlarmAction> ()
			, _startDate(TIME_UNKNOWN), _endDate(TIME_UNKNOWN)
		{}

		void UpdateAlarmAction::run()
		{
			_alarm->setLevel(_type);
			if (_singleSentAlarm.get())
			{
				_singleSentAlarm->setPeriodStart(_startDate);
				_singleSentAlarm->setPeriodEnd(_endDate);
				_singleSentAlarm->setIsEnabled(_enabled);
			}
			AlarmTableSync::Save(_alarm.get());
		}



		bool UpdateAlarmAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesRight>(WRITE);
		}
	}
}
