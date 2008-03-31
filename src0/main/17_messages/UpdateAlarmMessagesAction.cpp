
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

#include "UpdateAlarmMessagesAction.h"

#include "17_messages/MessagesModule.h"
#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	
	template<> const string util::FactorableTemplate<Action, messages::UpdateAlarmMessagesAction>::FACTORY_KEY("muama");

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
			try
			{
				_alarm.reset(AlarmTableSync::GetUpdateable(_request->getObjectId(), true));
			}
			catch (...)
			{
				throw ActionException("Specified alarm not found");
			}
			
			_shortMessage = map.getString(PARAMETER_SHORT_MESSAGE, true, FACTORY_KEY);
			_longMessage = map.getString(PARAMETER_LONG_MESSAGE, true, FACTORY_KEY);
		}

		void UpdateAlarmMessagesAction::run()
		{
			_alarm->setShortMessage(_shortMessage);
			_alarm->setLongMessage(_longMessage);
			AlarmTableSync::Save(_alarm.get());
		}
	}
}
