
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

#include "04_time/TimeParseException.h"
#include "04_time/DateTime.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "17_messages/UpdateAlarmAction.h"
#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace time;
	
	namespace messages
	{
		const string UpdateAlarmAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "typ";
		const string UpdateAlarmAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sda";
		const string UpdateAlarmAction::PARAMETER_START_HOUR = Action_PARAMETER_PREFIX + "sho";
		const string UpdateAlarmAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "eda";
		const string UpdateAlarmAction::PARAMETER_END_HOUR = Action_PARAMETER_PREFIX + "eho";
		const string UpdateAlarmAction::PARAMETER_ENABLED = Action_PARAMETER_PREFIX + "ena";


		ParametersMap UpdateAlarmAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateAlarmAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				_alarm = MessagesModule::getAlarms().get(_request->getObjectId());

				it = map.find(PARAMETER_TYPE);
				if (it == map.end())
					throw ActionException("Type not specified");
				_type = (AlarmLevel) Conversion::ToInt(it->second);

				if (_alarm->getScenario() == NULL)
				{
					string date;
					it = map.find(PARAMETER_START_DATE);
					if (it == map.end())
						throw ActionException("Start date not specified");
					if (!it->second.empty())
					{
						date = it->second;
				
						it = map.find(PARAMETER_START_HOUR);
						if (it == map.end())
							throw ActionException("Start hour not specified");
						_startDate = it->second.empty()
							? DateTime::FromString(date + " 0:0")
							: DateTime::FromString(date + " " + it->second);
					}
				
					it = map.find(PARAMETER_END_DATE);
					if (it == map.end())
						throw ActionException("End date not specified");
					if (!it->second.empty())
					{
						date = it->second;
				
						it = map.find(PARAMETER_END_HOUR);
						if (it == map.end())
							throw ActionException("End hour not specified");
						_endDate = it->second.empty()
							? DateTime::FromString(date + " 23:59")
							: DateTime::FromString(date + " " + it->second);
					}
				
					// Enabled status
					it = map.find(PARAMETER_ENABLED);
					if (it == map.end())
						throw ActionException("Enabled status not specified");
					_enabled = Conversion::ToBool(it->second);
				}
			}
			catch (Alarm::RegistryKeyException e)
			{
				throw ActionException("Specified alarm not found");
			}
			catch(TimeParseException e)
			{
				throw ActionException("Une date ou une heure est mal formée");
			}
			catch(...)
			{
				throw ActionException("Unknown error");
			}
		}

		UpdateAlarmAction::UpdateAlarmAction()
			: Action()
			, _alarm(NULL)
			, _startDate(TIME_UNKNOWN), _endDate(TIME_UNKNOWN)
		{}

		void UpdateAlarmAction::run()
		{
			_alarm->setLevel(_type);
			if (_alarm->getScenario() == NULL)
			{
				_alarm->setPeriodStart(_startDate);
				_alarm->setPeriodEnd(_endDate);
				_alarm->setIsEnabled(_enabled);
			}
			AlarmTableSync::save(_alarm);
		}
	}
}