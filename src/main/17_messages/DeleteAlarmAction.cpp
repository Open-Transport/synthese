
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

#include "DeleteAlarmAction.h"

#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesModule.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	
	template<> const string util::FactorableTemplate<Action, messages::DeleteAlarmAction>::FACTORY_KEY("deletealarm");

	namespace messages
	{
		const string DeleteAlarmAction::PARAMETER_ALARM = Action_PARAMETER_PREFIX + "ala";


		ParametersMap DeleteAlarmAction::getParametersMap() const
		{
			ParametersMap map;
			if (_alarm.get())
				map.insert(PARAMETER_ALARM, _alarm->getId());
			return map;
		}

		void DeleteAlarmAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_ALARM, true, FACTORY_KEY));
			try
			{
				_alarm = Alarm::Get(id);
			}
			catch (...)
			{
				throw ActionException("Alarm not specified");
			}
		}

		void DeleteAlarmAction::run()
		{
			/// @todo Delete alarm broadcast list
			AlarmTableSync::remove(_alarm->getId());
		}
	}
}