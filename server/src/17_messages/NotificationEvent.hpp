/** NotificationEvent class header.
	@file NotificationEvent.hpp
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

#ifndef SYNTHESE_messages_NotificationEvent_hpp__
#define SYNTHESE_messages_NotificationEvent_hpp__

#include "Object.hpp"

#include "NumericField.hpp"
#include "StringField.hpp"
#include "EnumObjectField.hpp"
#include "SchemaMacros.hpp"
#include "Alarm.h"
#include "NotificationProvider.hpp"
#include "MessageType.hpp"

namespace synthese
{
	namespace messages
	{
		typedef enum
		{
			READY = 0,
			IN_PROGRESS = 1,
			FAILED = 2,
			SUCCESS = 3
		} NotificationStatus;

		typedef enum
		{
			NONE = 0,
			BEGIN = 1,
			END = 2
		} NotificationType;

		FIELD_PTIME(Expiration)
		FIELD_PTIME(LastAttempt)
		FIELD_INT(Attempts)
		FIELD_ENUM(Status, NotificationStatus)
		FIELD_ENUM(Type, NotificationType)
		FIELD_ENUM(Reminder, NotificationType)

		typedef boost::fusion::map<
			FIELD(Key),

			// Reference to the alarm message
			FIELD(Alarm),

			// Expiration timestamp of the notification
			FIELD(Expiration),

			// Timestamp when the notification was attempted last time
			FIELD(LastAttempt),

			// Number of performed attempts
			FIELD(Attempts),

			// Reference to the provider responsible for notification
			FIELD(NotificationProvider),

			// Technical status of the notification
			FIELD(Status),

			// Type of notification: begin or end
			FIELD(Type),

			// Reminder  // TODO: to be redefined later on
			FIELD(Reminder)
		> NotificationEventRecord;

		/**
		@class NotificationEvent
		@ingroup m17

		Notification event.
		*/
		class NotificationEvent :
			public Object<NotificationEvent, NotificationEventRecord>
		{
		public:
			
			/**
			* NotificationEvent key in parameters map
			*/
			static const std::string ATTR_KEY;

			NotificationEvent(
				util::RegistryKeyType id = 0
				);

			NotificationEvent(
				util::RegistryKeyType id,
				Alarm& alarm,
				const boost::posix_time::ptime& expiration,
				NotificationProvider& notificationProvider
				);

			virtual ~NotificationEvent() { };
		};
	}
}

#endif // SYNTHESE_messages_NotificationEvent_hpp__
