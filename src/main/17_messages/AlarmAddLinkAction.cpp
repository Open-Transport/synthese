
/** AlarmAddLinkAction class implementation.
	@file AlarmAddLinkAction.cpp

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

#include "AlarmAddLinkAction.h"

#include "17_messages/SingleSentAlarm.h"
#include "17_messages/ScenarioSentAlarm.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/AlarmRecipient.h"
#include "17_messages/AlarmObjectLink.h"
#include "17_messages/AlarmObjectLinkTableSync.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesLibraryLog.h"
#include "17_messages/MessagesLog.h"

#include "01_util/Conversion.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action, messages::AlarmAddLinkAction>::FACTORY_KEY("maala");
	
	namespace messages
	{
		const string AlarmAddLinkAction::PARAMETER_ALARM_ID = Action_PARAMETER_PREFIX + "a";
		const string AlarmAddLinkAction::PARAMETER_RECIPIENT_KEY = Action_PARAMETER_PREFIX + "r";
		const string AlarmAddLinkAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "o";


		ParametersMap AlarmAddLinkAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_ALARM_ID, _alarm.get() ? _alarm->getId() : uid(0));
			map.insert(PARAMETER_RECIPIENT_KEY, _recipientKey);
			map.insert(PARAMETER_OBJECT_ID, _objectId);
			return map;
		}

		void AlarmAddLinkAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Recipient key
			_recipientKey = map.getString(PARAMETER_RECIPIENT_KEY, true, FACTORY_KEY);
			if (!Factory<AlarmRecipient>::contains(_recipientKey))
				throw ActionException("Specified recipient not found");

			// Alarm ID
			uid id(map.getUid(PARAMETER_ALARM_ID, true, FACTORY_KEY));
			try
			{
				_alarm.reset(AlarmTableSync::Get(id, true));
			}
			catch (...)
			{
				throw ActionException("Specified alarm not found");
			}
			
			// Object ID
			_objectId = map.getUid(PARAMETER_OBJECT_ID, true, FACTORY_KEY);
		}

		void AlarmAddLinkAction::run()
		{
			// Action
			shared_ptr<AlarmObjectLink> aol(new AlarmObjectLink);
			aol->setRecipientKey(_recipientKey);
			aol->setAlarmId(_alarm->getId());
			aol->setObjectId(_objectId);
			AlarmObjectLinkTableSync::save(aol.get());

			// Log
			if (dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm).get())
			{
				shared_ptr<const AlarmTemplate> alarmTemplate = dynamic_pointer_cast<const AlarmTemplate, const Alarm>(_alarm);
				MessagesLibraryLog::addUpdateEntry(alarmTemplate.get(), "Ajout de destinataire " + _recipientKey + " #" + Conversion::ToString(_objectId), _request->getUser().get());
			}
			else if (dynamic_pointer_cast<const SingleSentAlarm, const Alarm>(_alarm).get())
			{
				shared_ptr<const SingleSentAlarm> singleSentAlarm = dynamic_pointer_cast<const SingleSentAlarm, const Alarm>(_alarm);
				MessagesLog::addUpdateEntry(singleSentAlarm.get(), "Ajout de destinataire � message simple " + _recipientKey + " #" + Conversion::ToString(_objectId), _request->getUser().get());
			}
			else
			{
				shared_ptr<const ScenarioSentAlarm> scenarioSentAlarm = dynamic_pointer_cast<const ScenarioSentAlarm, const Alarm>(_alarm);
				MessagesLog::addUpdateEntry(scenarioSentAlarm.get(), "Ajout de destinataire � message de sc�nario " + _recipientKey + " #" + Conversion::ToString(_objectId), _request->getUser().get());
			}
		}

		void AlarmAddLinkAction::setRecipientKey( const std::string& key )
		{
			_recipientKey = key;
		}

		void AlarmAddLinkAction::setAlarm( boost::shared_ptr<const Alarm> alarm )
		{
			_alarm = alarm;
		}

		void AlarmAddLinkAction::setObjectId( uid id )
		{
			_objectId = id;
		}
	}
}
