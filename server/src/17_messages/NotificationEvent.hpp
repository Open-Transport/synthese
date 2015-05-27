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

#include <Alarm.h>
#include <EnumObjectField.hpp>
#include <NotificationProvider.hpp>
#include <NumericField.hpp>
#include <Object.hpp>
#include <PointerField.hpp>
#include <PtimeField.hpp>
#include <SchemaMacros.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/support/pair.hpp>

#include <string>

namespace synthese
{
	namespace messages
	{
		/**
			Notification event type enumeration.
			BEGIN and END are created by NotificationProvider.
			REMINDER may be created by a NotificationChannel derived class.
		 */
		typedef enum
		{
			NONE = 0,
			BEGIN = 1,
			END = 2,
			REMINDER = 3
		} NotificationType;

		/**
			Notification event status enumeration.
			READY: event has just been created, nothing happened yet.
			IN_PROGRESS: since first attempt and during retry attempt periods.
			FAILED: after maximum attempt count has been reached.
			SUCCESS: notification event has been processed successfully.

			Order is relevant. Event with status < FAILED is considered active.
			Event with status >= FAILED is considered over.
		*/
		typedef enum
		{
			READY = 0,
			IN_PROGRESS = 1,
			FAILED = 2,
			SUCCESS = 3
		} NotificationStatus;

		// Specific field declarations
		FIELD_ENUM(EventType, NotificationType)
		FIELD_ENUM(Status, NotificationStatus)
		FIELD_PTIME(Expiration)
		FIELD_INT(Attempts)
		FIELD_PTIME(LastAttempt)

		/** Entity framework persistent field declaration. */
		typedef boost::fusion::map<
			FIELD(Key),

			// Reference to the alarm message
			FIELD(Alarm),

			// Reference to the provider responsible for notification
			FIELD(NotificationProvider),

			// Type of notification: begin or end
			FIELD(EventType),

			// Technical status of the notification
			FIELD(Status),

			// Expiration timestamp of the notification
			FIELD(Expiration),

			// Number of performed attempts
			FIELD(Attempts),

			// Timestamp when the notification was attempted last time
			FIELD(LastAttempt)

		> NotificationEventRecord;

		/**
		@class NotificationEvent
		@ingroup m17

		Notification event.

		Allow asynchronous multiple notification attempts of an alarm message,
		implemented for a specific protocol thanks to a NotificationChannel instance,
		associated to a registered NotificationProvider.

		*/
		class NotificationEvent :
			public Object<NotificationEvent, NotificationEventRecord>
		{
		public:
			
			/**
			* NotificationEvent key in parameters map
			*/
			static const std::string ATTR_KEY;

			/** Default constructor for registry. */
			NotificationEvent(
				util::RegistryKeyType id = 0
			);

			/**
				Constructor.
				@param id registry key id
				@param alarm reference to notified alarm
				@param notificationProvider reference to source provider
				@param expiration timestamp for event expiry (defaults to not_a_date_time)
			 */
			NotificationEvent(
				util::RegistryKeyType id,
				Alarm& alarm,
				NotificationProvider& notificationProvider,
				const NotificationType eventType,
				const boost::posix_time::ptime& expiration = boost::posix_time::not_a_date_time
			);

			/** Virtual destructor. */
			virtual ~NotificationEvent() { };



			/**
				Test if the event has expired
				@return true if expired
			*/
			bool isExpired() const;



			/**
				Mark this event as successfuly notified.
				Useful for async notification channel derived class.
			*/
			void markSuccessful();

			/**
				Create and store a notification event.
				@param env reference to environment
				@param alarm reference to notified alarm
				@param provider reference to source provider
				@param type event type
				@param saveNow flag to delay save operation
			*/
			static boost::shared_ptr<NotificationEvent> createEvent(
				util::Env& env,
				const SentAlarm& alarm,
				const NotificationProvider* provider,
				const NotificationType type,
				const bool saveNow = true
			);

			/**
				Search for an already existing notification event for the same
				alarm and type. If none, create one.

				If necessary expiration will be refreshed if alarm end period
				has been changed.

				@param reference to notified alarm
				@param provider reference to source provider
				@param type event type
			*/
			static boost::shared_ptr<NotificationEvent> findOrCreateEvent(
				const SentAlarm& alarm,
				const NotificationProvider* provider,
				const NotificationType type
			);

		};
	}
}

#endif // SYNTHESE_messages_NotificationEvent_hpp__
