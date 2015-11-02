
/** AlarmObjectLink class implementation.
	@file AlarmObjectLink.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "AlarmObjectLink.h"

#include "Alarm.h"
#include "AlarmRecipient.h"
#include "Factory.h"
#include "MessagesRight.h"
#include "Profile.h"
#include "Registry.h"
#include "Session.h"
#include "User.h"

#include <boost/foreach.hpp>

namespace synthese
{
	using namespace messages;
	using namespace util;

	CLASS_DEFINITION(AlarmObjectLink, "t040_alarm_object_links", 40)
	FIELD_DEFINITION_OF_OBJECT(AlarmObjectLink, "alarm_object_link_id", "alarm_object_link_ids")

	FIELD_DEFINITION_OF_TYPE(RecipientKey, "recipient_key", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ObjectId, "object_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LinkedAlarm, "alarm_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Parameter, "parameter", SQL_TEXT)

	namespace messages
	{
		AlarmObjectLink::AlarmObjectLink(
			RegistryKeyType key
		):	Registrable(key),
			Object<AlarmObjectLink, AlarmObjectLinkSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(RecipientKey),
					FIELD_DEFAULT_CONSTRUCTOR(ObjectId),
					FIELD_DEFAULT_CONSTRUCTOR(LinkedAlarm),
					FIELD_DEFAULT_CONSTRUCTOR(Parameter)
			)	)
		{}



		void AlarmObjectLink::setRecipient( const std::string& key )
		{
			_recipient.reset(
				Factory<AlarmRecipient>::create(key)
			);

			set<RecipientKey>(key);
		}

		Alarm* AlarmObjectLink::getAlarm() const
		{
			return get<LinkedAlarm>() ?
				get<LinkedAlarm>().get_ptr() :
				NULL;
		}

		void AlarmObjectLink::setAlarm(Alarm* value)
		{
			set<LinkedAlarm>(value
				? boost::optional<Alarm&>(*value)
				: boost::none);
		}

		void AlarmObjectLink::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if (!get<RecipientKey>().empty())
			{
				_recipient.reset(
					Factory<AlarmRecipient>::create(get<RecipientKey>())
				);
			}
			if (get<LinkedAlarm>())
			{
				get<LinkedAlarm>()->addLinkedObject(*this);
				get<LinkedAlarm>()->clearBroadcastPointsCache();
			}
		}

		void AlarmObjectLink::unlink()
		{
			if(get<LinkedAlarm>())
			{
				get<LinkedAlarm>()->removeLinkedObject(*this);
				get<LinkedAlarm>()->clearBroadcastPointsCache();
			}
		}

		bool AlarmObjectLink::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool AlarmObjectLink::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool AlarmObjectLink::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::DELETE_RIGHT);
		}
}	}

/** @class AlarmObjectLink
	@ingroup m17

	Link between an alarm and an object.
*/
