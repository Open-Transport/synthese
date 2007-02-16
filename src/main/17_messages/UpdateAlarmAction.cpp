
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

#include "30_server/ActionException.h"

#include "17_messages/UpdateAlarmAction.h"
#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string UpdateAlarmAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "typ";
		const string UpdateAlarmAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sda";
		const string UpdateAlarmAction::PARAMETER_START_HOUR = Action_PARAMETER_PREFIX + "sho";
		const string UpdateAlarmAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "eda";
		const string UpdateAlarmAction::PARAMETER_END_HOUR = Action_PARAMETER_PREFIX + "eho";
		

		Request::ParametersMap UpdateAlarmAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateAlarmAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_TYPE);
			if (it == map.end())
				throw ActionException("Type not specified");
			
			// _xxx = it->second;
			// map.erase(it);
			// if (_xxx <= 0)
			//	throw ActionException("Bad value for xxx parameter ");	
			// 
		}

		UpdateAlarmAction::UpdateAlarmAction()
			: Action()
			, _alarm(NULL)
		{}

		void UpdateAlarmAction::run()
		{
			_alarm->setLevel(_type);
			_alarm->setPeriodStart(_startDate);
			_alarm->setPeriodEnd(_endDate);
			AlarmTableSync::save(_alarm);
		}
	}
}