
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
#include "Alarm.h"
#include "SentAlarm.h"
#include "AlarmTemplate.h"
#include "AlarmTableSync.h"
#include "AlarmObjectLinkTableSync.h"
#include "MessagesModule.h"
#include "MessagesLog.h"
#include "MessagesLibraryLog.h"
#include "MessagesRight.h"
#include "MessagesLibraryRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	
	template<> const string util::FactorableTemplate<Action, messages::DeleteAlarmAction>::FACTORY_KEY("deletealarm");

	namespace messages
	{
		const string DeleteAlarmAction::PARAMETER_ALARM = Action_PARAMETER_PREFIX + "ala";


		ParametersMap DeleteAlarmAction::getParametersMap() const
		{
			ParametersMap map;
			if (_alarm.get())
				map.insert(PARAMETER_ALARM, _alarm->getKey());
			return map;
		}

		void DeleteAlarmAction::_setFromParametersMap(const ParametersMap& map) throw(ActionException)
		{
			try
			{
				setAlarmId(map.getUid(PARAMETER_ALARM, true, FACTORY_KEY));
			}
			catch(ParametersMap::MissingParameterException& e)
			{
				throw ActionException(e, *this);
			}
		}

		void DeleteAlarmAction::run(
			Request& request
		) throw(ActionException) {
			// Action
			AlarmObjectLinkTableSync::Remove(_alarm->getKey());
			AlarmTableSync::Remove(_alarm->getKey());

			// Log
			if (dynamic_cast<const SentAlarm*>(_alarm.get()))
			{
				MessagesLog::AddDeleteEntry(static_cast<const SentAlarm*>(_alarm.get()), request.getUser().get());
			}
			else
			{
				MessagesLibraryLog::AddDeleteEntry(static_cast<const AlarmTemplate*>(_alarm.get()), request.getUser().get());
			}
		}



		void DeleteAlarmAction::setAlarmId(RegistryKeyType id ) throw(ActionException)
		{
			try
			{
				_alarm = AlarmTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw ActionException("Alarm not specified");
			}
		}



		bool DeleteAlarmAction::isAuthorized(const Profile& profile
		) const {
			if (dynamic_cast<const SentAlarm*>(_alarm.get()))
			{
				return profile.isAuthorized<MessagesRight>(DELETE_RIGHT);
			}
			else
			{
				return profile.isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
			}
		}
	}
}
