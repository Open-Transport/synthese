////////////////////////////////////////////////////////////////////////////////
/// AlarmRemoveLinkAction class implementation.
///	@file AlarmRemoveLinkAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "AlarmRemoveLinkAction.h"

#include "SentScenario.h"
#include "AlarmTableSync.h"
#include "ActionException.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "ParametersMap.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "AlarmObjectLinkTableSync.h"
#include "MessagesLog.h"
#include "MessagesLibraryLog.h"
#include "Alarm.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, messages::AlarmRemoveLinkAction>::FACTORY_KEY("marla");

	namespace messages
	{
		const string AlarmRemoveLinkAction::PARAMETER_ALARM_ID = Action_PARAMETER_PREFIX + "aid";
		const string AlarmRemoveLinkAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "oid";


		ParametersMap AlarmRemoveLinkAction::getParametersMap() const
		{
			ParametersMap map;
			if (_alarm.get() != NULL) map.insert(PARAMETER_ALARM_ID, _alarm->getKey());
			map.insert(PARAMETER_OBJECT_ID, _objectId);
			return map;
		}

		void AlarmRemoveLinkAction::_setFromParametersMap(
			const ParametersMap& map
		) throw(ActionException)
		{
			try
			{
				setAlarmId(map.get<RegistryKeyType>(PARAMETER_ALARM_ID));
				setObjectId(map.get<RegistryKeyType>(PARAMETER_OBJECT_ID));
			}
			catch (ParametersMap::MissingParameterException& e)
			{
				throw ActionException(e, *this);
			}
		}

		void AlarmRemoveLinkAction::run(
			Request& request
		) throw (ActionException) {

			AlarmObjectLinkTableSync::RemoveByMessage(_alarm->getKey(), _objectId);

			if (_alarm->belongsToTemplate())
			{
				MessagesLibraryLog::addUpdateEntry(_alarm.get(), "Suppression de la destination "+ lexical_cast<string>(_objectId), request.getUser().get());
			}
			else
			{
				MessagesLog::addUpdateEntry(_alarm.get(), "Suppression de la destination "+ lexical_cast<string>(_objectId), request.getUser().get());
			}
		}

		void AlarmRemoveLinkAction::setAlarmId(RegistryKeyType id ) throw(ActionException)
		{
			try
			{
				_alarm = dynamic_pointer_cast<Alarm, util::Registrable>(db::DBModule::GetEditableObject(id, *_env));
			}
			catch(ObjectNotFoundException<Alarm>& e)
			{
				throw ActionException("message", e, *this);
			}
		}

		void AlarmRemoveLinkAction::setObjectId(RegistryKeyType id )
		{
			_objectId = id;
		}



		bool AlarmRemoveLinkAction::isAuthorized(const Session* session
		) const {
			if (_alarm->belongsToTemplate())
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
			}
		}
	}
}
