
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

#include "01_util/Conversion.h"

#include "30_server/ActionException.h"

#include "17_messages/AlarmAddLinkAction.h"
#include "17_messages/SingleSentAlarm.h"
#include "17_messages/ScenarioSentAlarm.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/AlarmRecipient.h"
#include "17_messages/AlarmObjectLink.h"
#include "17_messages/AlarmObjectLinkTableSync.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesLibraryLog.h"
#include "17_messages/MessagesLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace dblog;
	
	namespace messages
	{
		const string AlarmAddLinkAction::PARAMETER_ALARM_ID = Action_PARAMETER_PREFIX + "a";
		const string AlarmAddLinkAction::PARAMETER_RECIPIENT_KEY = Action_PARAMETER_PREFIX + "r";
		const string AlarmAddLinkAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "o";


		ParametersMap AlarmAddLinkAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_ALARM_ID, _alarm.get() ? Conversion::ToString(_alarm->getId()) : "0"));
			map.insert(make_pair(PARAMETER_RECIPIENT_KEY, _recipientKey));
			map.insert(make_pair(PARAMETER_OBJECT_ID, Conversion::ToString(_objectId)));
			return map;
		}

		void AlarmAddLinkAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Recipient key
			it = map.find(PARAMETER_RECIPIENT_KEY);
			if (it == map.end())
				throw ActionException("Parameter recipient key not found");
			if (!Factory<AlarmRecipient>::contains(it->second))
				throw ActionException("Specified recipient not found");
			_recipientKey = it->second;

			// Alarm ID
			it = map.find(PARAMETER_ALARM_ID);
			if (it == map.end())
				throw ActionException("Alarm not specified");
			try
			{
				_alarm = AlarmTableSync::getAlarm(Conversion::ToLongLong(it->second));
			}
			catch (...)
			{
				throw ActionException("Specified alarm not found");
			}
			
			// Object ID
			it = map.find(PARAMETER_OBJECT_ID);
			if (it == map.end())
				throw ActionException("Object to link not specified");
			_objectId = Conversion::ToLongLong(it->second);
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
				shared_ptr<MessagesLibraryLog> log = Factory<DBLog>::create<MessagesLibraryLog>();
				log->addUpdateEntry(alarmTemplate, "Ajout de destinataire " + _recipientKey + " #" + Conversion::ToString(_objectId), _request->getUser() );
			}
			else if (dynamic_pointer_cast<const SingleSentAlarm, const Alarm>(_alarm).get())
			{
				shared_ptr<const SingleSentAlarm> singleSentAlarm = dynamic_pointer_cast<const SingleSentAlarm, const Alarm>(_alarm);
				shared_ptr<MessagesLog> log = Factory<DBLog>::create<MessagesLog>();
				log->addUpdateEntry(singleSentAlarm, "Ajout de destinataire à message simple " + _recipientKey + " #" + Conversion::ToString(_objectId), _request->getUser());
			}
			else
			{
				shared_ptr<const ScenarioSentAlarm> scenarioSentAlarm = dynamic_pointer_cast<const ScenarioSentAlarm, const Alarm>(_alarm);
				shared_ptr<MessagesLog> log = Factory<DBLog>::create<MessagesLog>();
				log->addUpdateEntry(scenarioSentAlarm, "Ajout de destinataire à message de scénario " + _recipientKey + " #" + Conversion::ToString(_objectId), _request->getUser());
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
