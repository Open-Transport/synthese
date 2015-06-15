/** NotificationProvider class header
	@file NotificationProvider.hpp
	@author Yves Martin
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
#ifndef SYNTHESE_messages_NotificationProvider_hpp__
#define SYNTHESE_messages_NotificationProvider_hpp__

#include <BroadcastPoint.hpp>
#include <FactorableTemplate.h>
#include <MessageType.hpp>
#include <NumericField.hpp>
#include <Object.hpp>
#include <ParametersMapField.hpp>
#include <PointerField.hpp>
#include <SchemaMacros.hpp>
#include <StringField.hpp>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <set>
#include <string>
#include <vector>

/** @defgroup refFile 17 Notification provider broadcast point
	@ingroup ref
*/

namespace synthese
{
	namespace messages
	{
		// Forward declarations
		class NotificationChannel;
		class NotificationEvent;
		class AlarmObjectLink;

		FIELD_STRING(NotificationChannelKey)
		FIELD_POINTER(MessageTypeBegin, MessageType)
		FIELD_POINTER(MessageTypeEnd, MessageType)

		FIELD_BOOL(SubscribeAllBegin)
		FIELD_BOOL(SubscribeAllEnd)
		FIELD_BOOL(SetEventsHold)
		FIELD_INT(RetryAttemptDelay)
		FIELD_INT(MaximumRetryAttempts)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),

			// Notification channel key, aka protocol
			FIELD(NotificationChannelKey),

			// Message type content for begin event
			FIELD(MessageTypeBegin),

			// Message type content for end event
			FIELD(MessageTypeEnd),

			// Flag to force processing of all sent alarms for begin events
			FIELD(SubscribeAllBegin),

			// Flag to force processing of all sent alarms for end events
			FIELD(SubscribeAllEnd),

			// Flag to force events to be set to HOLD state for manual confirmation
			FIELD(SetEventsHold),

			// Delay in seconds between notification event retry
			FIELD(RetryAttemptDelay),

			// Maximum number of retry attemps before giving up notify the event
			FIELD(MaximumRetryAttempts),

			// Implementation specific parameters
			FIELD(Parameters)
		> NotificationProviderRecord;

/**
	@class NotificationProvider
	@ingroup m17

	A NotificationProvider broadcasts message out of Synthese system. It is
	BroadcastPoint implementation that can be associated to alarms.

		The SubscribeAllBegin and SubscribeAllEnd flags allow to subscribe all
		alarms even if user does not enlist the registered provider
		specifically.

	Message broadcasting

	Each registered provider is notified by MessagesActivationThread about
	begin and end period for each active Alarm.

	NotificationProvider creates a NotificationEvent which will persist for
	relevant event (begin or end).

	The NotificationThread will submit back events to corresponding
	NotificationChannel implementation so that the technical job of
	broadcasting is achieved. In case of failure an exception must be
	thrown.

	The NotificationThread implements error handling and retry mechanism
	for all notification providers.
 */
		class NotificationProvider:
				public Object<NotificationProvider, NotificationProviderRecord>,
				public util::FactorableTemplate<BroadcastPoint, NotificationProvider>
		{
		public:
			NotificationProvider(util::RegistryKeyType id=0);

			virtual ~NotificationProvider() {};

			/**
				Map of notification providers:
				- provider name as key
				- NotificationBroadcastPoint implementation as value
			*/
			typedef std::vector<std::string> NotificationChannelsList;

			/**
				Provides the list of registered NotificationBroadcastPoint implementation
			*/
			static NotificationChannelsList GetNotificationChannels();

			/**
				Support for NotificationChannel specific parameters
			*/
			virtual void addAdditionalParameters(
				util::ParametersMap& map,
				std::string prefix = std::string()
			) const;

			/// @name BroadcastPoint virtual methods
			//@{
				virtual std::set<messages::MessageType*> getMessageTypes() const;

				virtual bool displaysMessage(
					const messages::Alarm::LinkedObjects& recipients,
					const util::ParametersMap& parameters
				) const;

				virtual void getBroadcastPoints(BroadcastPoints& result) const;

				virtual void onDisplayStart(const Alarm& message) const;

				virtual void onDisplayEnd(const Alarm& message) const;
			//@}

			/**
				Generates channel's script fields values based on an alarm
				@param message source message to generate fields
				@param type notification type
				@return map with field name and generated values
			 */
			boost::optional<util::ParametersMap> generateScriptFields(
				const Alarm* message,
				const int type
			);

			/**
				Computes next attempt timestamp for a NotificationEvent.
				Value posix_time::not_a_date_time means no more attempt is expected.

				@param event NotificationEvent to process
				@return time stamp for next attempt or posix_time::not_a_date_time if none
			*/
			boost::posix_time::ptime nextAttemptTime(
				const boost::shared_ptr<NotificationEvent>& event
			) const;

			/**
				Notify the event according to NotificationProvider parameters
				available with "channel" prefix.

				@param event NotificationEvent to process
				@return true only if notification succeeded
			*/
			bool notify(const boost::shared_ptr<NotificationEvent>& event);

			virtual std::string getName() const { return get<Name>(); }


		private:
			/// Reference to a notification channel instance
			boost::shared_ptr<NotificationChannel> _notificationChannel;

			/// Factory for notification channel instance based on provider type
			boost::shared_ptr<NotificationChannel> getNotificationChannel();

			/// Copy of Alarm::LinkedObjects because of circular ref declarations
			typedef std::map<std::string, std::set<const AlarmObjectLink*> > AlarmLinkedObjects;

			/// Test if this instance is recipient of an alarm
			bool isRecipient(const AlarmLinkedObjects& recipients) const;
		};
	}
}

#endif /* SYNTHESE_messages_NotificationProvider_hpp__ */
