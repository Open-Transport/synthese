
/** DeleteAlarmAction class implementation.
	@file DeleteAlarmAction.cpp

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

#include "17_messages/DeleteAlarmAction.h"
#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string DeleteAlarmAction::PARAMETER_ALARM = Action_PARAMETER_PREFIX + "ala";


		ParametersMap DeleteAlarmAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_ALARM, Conversion::ToString(_alarm->getKey())));
			return map;
		}

		void DeleteAlarmAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_ALARM);
				if (it == map.end())
					throw ActionException("Alarm not specified");

				_alarm = MessagesModule::getAlarms().get(Conversion::ToLongLong(it->second));
			}
			catch (Alarm::RegistryKeyException e)
			{
				throw ActionException("Specified alarm not found");
			}
		}

		DeleteAlarmAction::DeleteAlarmAction()
			: Action()
			, _alarm(NULL)
		{}

		void DeleteAlarmAction::run()
		{
			/// @todo Delete alarm broadcast list
			AlarmTableSync::remove(_alarm->getKey());
		}
	}
}