
/** UpdateAlarmMessagesAction class implementation.
	@file UpdateAlarmMessagesAction.cpp

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
#include "30_server/Request.h"

#include "17_messages/UpdateAlarmMessagesAction.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	
	namespace messages
	{
		const string UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE = Action_PARAMETER_PREFIX + "sme";
		const string UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE = Action_PARAMETER_PREFIX + "lme";


		ParametersMap UpdateAlarmMessagesAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UpdateAlarmMessagesAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			try
			{
				_alarm = AlarmTableSync::get(_request->getObjectId());
			}
			catch (...)
			{
				throw ActionException("Specified alarm not found");
			}
			
			it = map.find(PARAMETER_SHORT_MESSAGE);
			if (it == map.end())
				throw ActionException("Short message not specified");
			_shortMessage = it->second;
			
			it = map.find(PARAMETER_LONG_MESSAGE);
			if (it == map.end())
				throw ActionException("Long message not specified");
			_longMessage = it->second;
		}

		void UpdateAlarmMessagesAction::run()
		{
			_alarm->setShortMessage(_shortMessage);
			_alarm->setLongMessage(_longMessage);
			AlarmTableSync::save(_alarm.get());
		}
	}
}
