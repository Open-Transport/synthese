/** NotificationProvider implementation.
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

#include "Alarm.h"
#include "AlarmObjectLink.h"
#include "BroadcastPointAlarmRecipient.hpp"
#include "Factory.h"
#include "Field.hpp"
#include "MessagesRight.h"
#include "NotificationChannel.hpp"
#include "NotificationEvent.hpp"
#include "NotificationLog.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Registry.h"
#include "Session.h"
#include "SimpleObjectFieldDefinition.hpp"
#include "User.h"
#include "UtilTypes.h"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/optional/optional.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <map>
#include <set>
#include <utility>

/**
	@defgroup refFile 17 Notification providers
	@ingroup ref
*/

using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace messages;

	namespace util
	{
		template<>
		const std::string FactorableTemplate<BroadcastPoint, NotificationProvider>::FACTORY_KEY = "NotificationProvider";
	}

	CLASS_DEFINITION(messages::NotificationProvider, "t107_notification_providers", 107)

	FIELD_DEFINITION_OF_OBJECT(NotificationProvider, "notification_provider_id", "notification_provider_ids")

	FIELD_DEFINITION_OF_TYPE(NotificationChannelKey, "channel", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SubscribeAllBegin, "subscribe_all_begin", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(SubscribeAllEnd, "subscribe_all_end", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(SubscribeUpdates, "subscribe_updates", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(SetEventsHold, "set_events_hold", SQL_BOOLEAN)

	FIELD_DEFINITION_OF_TYPE(RetryAttemptDelay, "retry_attempt_delay", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaximumRetryAttempts, "maximum_retry_attempts", SQL_INTEGER)

	template<> const Field ComplexObjectFieldDefinition<NotificationProviderTreeNode>::FIELDS[] = {
		Field("root_id", SQL_INTEGER),
		Field("up_id", SQL_INTEGER),
		Field("rank", SQL_INTEGER),
	Field() };

	template<> const Field SimpleObjectFieldDefinition<MessageTypeBegin>::FIELD = Field("begin_message_type_id", SQL_INTEGER);
	template<> const Field SimpleObjectFieldDefinition<MessageTypeEnd>::FIELD = Field("end_message_type_id", SQL_INTEGER);

	namespace messages {

		// Get list of registered notification providers, implementation of NotificationProvider
		NotificationProvider::NotificationChannelsList NotificationProvider::GetNotificationChannels()
		{
			Factory<NotificationChannel>::Keys keys(Factory<NotificationChannel>::GetKeys());
			NotificationProvider::NotificationChannelsList vec;
			BOOST_FOREACH(const Factory<NotificationChannel>::Keys::value_type& key, keys)
			{
				vec.push_back(key);
			}
			return vec;
		}



		void NotificationProvider::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix)
		const {
			// Make Parameters available for direct CMS rendering
			map.merge(get<Parameters>(), prefix);
		}



		/// Default constructor as a Registrable
		/// @param id registry key type
		NotificationProvider::NotificationProvider(
			RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<NotificationProvider, NotificationProviderRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(NotificationChannelKey),
					FIELD_DEFAULT_CONSTRUCTOR(MessageTypeBegin),
					FIELD_DEFAULT_CONSTRUCTOR(MessageTypeEnd),
					FIELD_VALUE_CONSTRUCTOR(SubscribeAllBegin, false),
					FIELD_VALUE_CONSTRUCTOR(SubscribeAllEnd, false),
					FIELD_VALUE_CONSTRUCTOR(SubscribeUpdates, true),
					FIELD_VALUE_CONSTRUCTOR(SetEventsHold, false),
					FIELD_DEFAULT_CONSTRUCTOR(RetryAttemptDelay),
					FIELD_DEFAULT_CONSTRUCTOR(MaximumRetryAttempts),
					FIELD_DEFAULT_CONSTRUCTOR(Parameters),
					FIELD_DEFAULT_CONSTRUCTOR(NotificationProviderTreeNode)
			)	)
		{}



		//////////////////////////////////////////////////////////////////////////
		/// Check which message type should be used when publishing
		/// an notification event
		std::set<messages::MessageType*> NotificationProvider::getMessageTypes() const
		{
			std::set<MessageType*> result;
			if(get<MessageTypeBegin>())
			{
				result.insert(&(*(get<MessageTypeBegin>())));
			}
			if(get<MessageTypeEnd>())
			{
				result.insert(&(*(get<MessageTypeEnd>())));
			}
			return result;

		}



		//////////////////////////////////////////////////////////////////////////
		/// Builds the list of the broadcast points for displaying in admin pages.
		/// @result result the list to populate
		void NotificationProvider::getBroadcastPoints( BroadcastPoints& result ) const
		{
			BOOST_FOREACH(
				const NotificationProvider::Registry::value_type& it,
				Env::GetOfficialEnv().getRegistry<messages::NotificationProvider>()
			){
				result.push_back(it.second.get());
			}
		}


		bool NotificationProvider::isRecipient(const AlarmLinkedObjects& recipients) const
		{
			// in broad cast points recipients
			AlarmLinkedObjects::const_iterator it(
				recipients.find(
					BroadcastPointAlarmRecipient::FACTORY_KEY
			)	);

			// No broadcast recipient = no display
			if( it == recipients.end())
			{
				return false;
			}

			// Loop on each recipient
			BOOST_FOREACH(
				const AlarmLinkedObjects::mapped_type::value_type& link,
				it->second
			){
				// Search for explicitly specified notification provider
				if( link->getObjectId() == getKey())
				{
					return true;
				}
			}
			return false;
		}

		/**
			Check of a message should be notified according to its recipients.
			A notification can be sent if it is explicitly linked to the message

			@param recipients the recipients list to check
			@param parameters the broadcast parameters
		*/
		bool NotificationProvider::displaysMessage(
			const Alarm::LinkedObjects& recipients,
			const util::ParametersMap& parameters
		) const	{
			// if global subscribe all options is set
			if(get<SubscribeAllBegin>() || get<SubscribeAllEnd>())
			{
				return true;
			}
			// Look for notification provider in recipients
			return isRecipient(recipients);
		}



		boost::posix_time::ptime NotificationProvider::nextAttemptTime(
			const boost::shared_ptr<NotificationEvent>& event
		) const {
			// Already failed or success, or maximum retry attempts reached
			if (event->get<Status>() >= FAILED
				|| event->get<Attempts>() >= this->get<MaximumRetryAttempts>())
			{
				return posix_time::not_a_date_time;
			}
			else
			{
				if (!event->get<LastAttempt>().is_not_a_date_time())
				{
					return event->get<LastAttempt>() + posix_time::seconds(this->get<RetryAttemptDelay>());
				}
				else
				{
					return posix_time::second_clock::local_time();
				}
			}
		}



		/**
			Get or create the notification channel instance.

			@return a pointer to NotificationChannel derived class instance or NULL
		 */
		boost::shared_ptr<NotificationChannel> NotificationProvider::getNotificationChannel()
		{
			std::string channelKey = get<NotificationChannelKey>();
			if (!Factory<NotificationChannel>::contains(channelKey))
			{
				// Inconsistent runtime and database
				// NotificationChannel implementation is lacking
				NotificationLog::AddNotificationProviderFailure(this,
					"No channel available for key " + channelKey
				);
			}
			else if (!_notificationChannel)
			{
				_notificationChannel = boost::shared_ptr<NotificationChannel>(
					Factory<NotificationChannel>::create(channelKey)
				);
			}

			return _notificationChannel;
		}



		boost::optional<util::ParametersMap> NotificationProvider::generateScriptFields(
			const Alarm* message,
			const int type
		) {
			boost::optional<ParametersMap> result;
			// Create corresponding NotificationChannel if not available
			if (getNotificationChannel()) {
				// Invoke attemptNotification(event)
				// return true only if completely successful
				result = _notificationChannel->generateScriptFields(
						this,
						message,
						(NotificationType)type,
						boost::posix_time::second_clock::local_time()
				);
			}
			return result;
		}



		/*
			Notify the event according to NotificationProvider parameters
			available with "channel" prefix.
		*/
		bool NotificationProvider::notify(const boost::shared_ptr<NotificationEvent>& event) {
			// Create corresponding NotificationChannel if not available
			if (getNotificationChannel()) {
				// Invoke attemptNotification(event)
				// return true only if completely successful
				return _notificationChannel->notifyEvent(event);

				// TODO: discard notification channel after successive failures
			}
			// if NULL channel key / protocol is not available
			return false;
		}



		/// Function invoked when the message display ends.
		/// @param message the Alarm to hide
		void NotificationProvider::onDisplayStart(
			const Alarm& message
		) const {
			// From FactorableTemplate<AlarmRecipient, BroadcastPointAlarmRecipient>::FACTORY_KEY
			//Alarm::LinkedObjects recipients = message.getLinkedObjects("displayscreen");
			// if global subscribe or explicit recipient
			if(get<SubscribeAllBegin>() || isRecipient(message.getLinkedObjects()))
			{
				const bool holdEvent = this->get<SetEventsHold>();
				NotificationEvent::findOrCreateEvent(message, this, BEGIN, holdEvent);
			}
		}



		/// Function invoked when the message display ends.
		/// @param message the Alarm to hide
		void NotificationProvider::onDisplayEnd(
			const Alarm& message
		) const {
			// if global subscribe or explicit recipient
			if(get<SubscribeAllEnd>() || isRecipient(message.getLinkedObjects()))
			{
				const bool holdEvent = this->get<SetEventsHold>();
				NotificationEvent::findOrCreateEvent(message, this, END, holdEvent);
			}
		}



		/// Function invoked before the message is updated.
		/// @param message the Alarm being updated
		void NotificationProvider::beforeMessageUpdate(
			const Alarm& message
		) const {
			// If this NotificationProvider registers to message updates and it is an explicit recipient of this message
			if(get<SubscribeUpdates>()
				&& message.isActivated()
				&& isRecipient(message.getLinkedObjects()))
			{
				// NotificationProvider does not process Alarm, it requires a NotificationEvent
				// Since we don't want to add another NotificationEvent into the database, we create a fake one that is only used to generate the message
				// NotificationEvent ctor requires non const Alarm and NotificationProvider, hence the const_cast usage
				NotificationProvider* self = const_cast<NotificationProvider*>(this);
				const boost::shared_ptr<NotificationEvent> fakeEvent(new NotificationEvent(0, const_cast<Alarm&>(message), *self, BEFORE_UPDATE));
				self->notify(fakeEvent);
			}
		}


		/// Function invoked after the message is updated.
		/// @param message the updated Alarm
		void NotificationProvider::afterMessageUpdate(
			const Alarm& message
		) const {
			// If this NotificationProvider registers to message updates and it is an explicit recipient of this message
			if(get<SubscribeUpdates>()
				&& message.isActivated()
				&& isRecipient(message.getLinkedObjects()))
			{
				boost::shared_ptr<NotificationEvent> beginEvent = NotificationEvent::findLastEvent(message, this, BEGIN);
				bool updateRequired = true;
				NotificationProvider* self = const_cast<NotificationProvider*>(this);
				boost::shared_ptr<NotificationChannel> channel = self->getNotificationChannel();
				if (channel) {
					updateRequired = channel->checkForUpdate(&message, beginEvent);
				}
				if (updateRequired) {
					const bool holdEvent = this->get<SetEventsHold>();
					NotificationEvent::findOrCreateEvent(message, this, UPDATE, holdEvent);
				}
			}
		}


		bool NotificationProvider::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool NotificationProvider::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool NotificationProvider::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::DELETE_RIGHT);
		}
	}
}

