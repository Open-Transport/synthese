////////////////////////////////////////////////////////////////////////////////
/// AlarmAddLinkAction class implementation.
///	@file AlarmAddLinkAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "AlarmAddLinkAction.h"
#include "SentAlarm.h"
#include "AlarmTemplate.h"
#include "AlarmRecipient.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "Conversion.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace dblog;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, messages::AlarmAddLinkAction>::FACTORY_KEY("maala");
	
	namespace messages
	{
		const string AlarmAddLinkAction::PARAMETER_ALARM_ID = Action_PARAMETER_PREFIX + "a";
		const string AlarmAddLinkAction::PARAMETER_RECIPIENT_KEY = Action_PARAMETER_PREFIX + "r";
		const string AlarmAddLinkAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "o";


		ParametersMap AlarmAddLinkAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_ALARM_ID, _alarm.get() ? _alarm->getKey() : RegistryKeyType(0));
			map.insert(PARAMETER_RECIPIENT_KEY, _recipientKey);
			map.insert(PARAMETER_OBJECT_ID, _objectId);
			return map;
		}

		void AlarmAddLinkAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Recipient key
			_recipientKey = map.get<string>(PARAMETER_RECIPIENT_KEY);
			if (!Factory<AlarmRecipient>::contains(_recipientKey))
				throw ActionException("Specified recipient not found");

			// Alarm ID
			setAlarmId(map.get<RegistryKeyType>(PARAMETER_ALARM_ID));
			
			// Object ID
			setObjectId(map.get<RegistryKeyType>(PARAMETER_OBJECT_ID));
		}

		void AlarmAddLinkAction::run(Request& request)
		{
			// Action
			shared_ptr<AlarmObjectLink> aol(new AlarmObjectLink);
			aol->setRecipientKey(_recipientKey);
			aol->setAlarm(_alarm.get());
			aol->setObjectId(_objectId);
			AlarmObjectLinkTableSync::Save(aol.get());

			// Log
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get())
			{
				shared_ptr<const AlarmTemplate> alarmTemplate = dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm);
				MessagesLibraryLog::addUpdateEntry(alarmTemplate.get(), "Ajout de destinataire " + _recipientKey + " #" + Conversion::ToString(_objectId), request.getUser().get());
			}
			else
			{
				shared_ptr<const SentAlarm> sentAlarm = dynamic_pointer_cast<const SentAlarm, const Alarm>(_alarm);
				MessagesLog::addUpdateEntry(sentAlarm.get(), "Ajout de destinataire à message diffusé " + _recipientKey + " #" + Conversion::ToString(_objectId), request.getUser().get());
			}
		}

		void AlarmAddLinkAction::setRecipientKey( const std::string& key )
		{
			_recipientKey = key;
		}

		void AlarmAddLinkAction::setAlarmId(RegistryKeyType id)
		{
			try
			{
				_alarm = AlarmTableSync::GetEditable(id, *_env);
			}
			catch (ObjectNotFoundException<Alarm>)
			{
				throw ActionException("Specified alarm not found");
			}
		}

		void AlarmAddLinkAction::setObjectId(RegistryKeyType id )
		{
			_objectId = id;
		}



		bool AlarmAddLinkAction::isAuthorized(const Session* session
		) const {
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get() != NULL)
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
