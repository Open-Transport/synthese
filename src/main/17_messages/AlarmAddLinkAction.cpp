
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
#include "17_messages/Alarm.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/AlarmRecipient.h"
#include "17_messages/AlarmObjectLink.h"
#include "17_messages/AlarmObjectLinkTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	
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
			if (!MessagesModule::getAlarms().contains(Conversion::ToLongLong(it->second)))
				throw ActionException("Specified alarm not found");
			_alarm = MessagesModule::getAlarms().get(Conversion::ToLongLong(it->second));
			
			// Object ID
			it = map.find(PARAMETER_OBJECT_ID);
			if (it == map.end())
				throw ActionException("Object to link not specified");
			_objectId = Conversion::ToLongLong(it->second);
		}

		void AlarmAddLinkAction::run()
		{
			shared_ptr<AlarmObjectLink<Registrable<uid, void> > > aol(new AlarmObjectLink<Registrable<uid, void> >);
			aol->setRecipientKey(_recipientKey);
			aol->setAlarm(_alarm);
			aol->setObjectId(_objectId);
			AlarmObjectLinkTableSync::save(aol.get());
		}

		void AlarmAddLinkAction::setRecipientKey( const std::string& key )
		{
			_recipientKey = key;
		}

		void AlarmAddLinkAction::setAlarm( boost::shared_ptr<const Alarm> alarm )
		{
			_alarm = alarm;
		}
	}
}
