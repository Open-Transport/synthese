/** NotificationEvent class implementation.
	@file NotificationEvent.cpp
	@author Ivan Ivanov
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include <NotificationEvent.hpp>
#include <Field.hpp>
#include <PointerField.hpp>
#include <Registry.h>
#include <UtilTypes.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/optional/optional.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace util;

	CLASS_DEFINITION(NotificationEvent, "t121_notification_events", 121)

	namespace messages {
		FIELD_DEFINITION_OF_OBJECT(NotificationEvent, "notification_event_id", "notification_event_ids")

		FIELD_DEFINITION_OF_TYPE(AlarmMessage, "alarm_id", SQL_INTEGER)
		FIELD_DEFINITION_OF_TYPE(Expiration, "expiration", SQL_DATETIME)
		FIELD_DEFINITION_OF_TYPE(LastAttempt, "last_attempt", SQL_DATETIME)
		FIELD_DEFINITION_OF_TYPE(Attempts, "attempts", SQL_INTEGER)
		FIELD_DEFINITION_OF_TYPE(Status, "status", SQL_INTEGER)

		/// Default constructor as a Registrable
		/// @param id registry key type
		NotificationEvent::NotificationEvent(
			RegistryKeyType id /*= 0*/
		) : Registrable(id),
			Object<NotificationEvent, NotificationEventRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(AlarmMessage),
					FIELD_VALUE_CONSTRUCTOR(Expiration, posix_time::not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(LastAttempt, posix_time::not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(Attempts, 0),
					FIELD_DEFAULT_CONSTRUCTOR(NotificationProvider),
					FIELD_VALUE_CONSTRUCTOR(Status, READY)
				)	)
		{ };

		NotificationEvent::NotificationEvent(
			RegistryKeyType id,
			const Alarm& alarm,
			const boost::posix_time::ptime& expiration,
			const NotificationProvider& notificationProvider
		) : Registrable(id),
			Object<NotificationEvent, NotificationEventRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(AlarmMessage, &alarm),
					FIELD_VALUE_CONSTRUCTOR(Expiration, expiration),
					FIELD_VALUE_CONSTRUCTOR(LastAttempt, posix_time::not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(Attempts, 0),
					FIELD_DEFAULT_CONSTRUCTOR(NotificationProvider),
					FIELD_VALUE_CONSTRUCTOR(Status, READY)
				)	)
		{ };
	
	}
}
