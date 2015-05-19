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

#include <NotificationProvider.hpp>
#include <NotificationChannel.hpp>
#include <NotificationEvent.hpp>

#include <Alarm.h>
#include <AlarmObjectLink.h>
#include <BroadcastPointAlarmRecipient.hpp>
#include <Env.h>
#include <Factory.h>
#include <Field.hpp>
#include <ParametersMap.h>
#include <Registry.h>
#include <UtilTypes.h>

#include <boost/foreach.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/optional/optional.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <map>
#include <utility>


/**
	@defgroup refFile 17 Notification providers
	@ingroup ref
*/

namespace synthese
{
	using namespace util;
	using namespace messages;

	namespace util
	{
		template<>
		const std::string FactorableTemplate<BroadcastPoint, NotificationProvider>::FACTORY_KEY = "NotificationProvider";
	}

	CLASS_DEFINITION(messages::NotificationProvider, "t199_notification_providers", 199)  // TODO Change to 107 at mailing list removal

	namespace messages {
		FIELD_DEFINITION_OF_OBJECT(NotificationProvider, "notification_provider_id", "notification_provider_ids")

		FIELD_DEFINITION_OF_TYPE(NotificationChannelKey, "channel", SQL_TEXT)
		FIELD_DEFINITION_OF_TYPE(SubscribeAllBegin, "subscribe_all_begin", SQL_BOOLEAN)
		FIELD_DEFINITION_OF_TYPE(SubscribeAllEnd, "subscribe_all_end", SQL_BOOLEAN)
		FIELD_DEFINITION_OF_TYPE(RetryAttemptDelay, "retry_attempt_delay", SQL_INTEGER)
		FIELD_DEFINITION_OF_TYPE(MaximumRetryAttempts, "maximum_retry_attempts", SQL_INTEGER)


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
					FIELD_VALUE_CONSTRUCTOR(SubscribeAllBegin, false),
					FIELD_VALUE_CONSTRUCTOR(SubscribeAllEnd, false),
					FIELD_DEFAULT_CONSTRUCTOR(RetryAttemptDelay),
					FIELD_DEFAULT_CONSTRUCTOR(MaximumRetryAttempts),
					FIELD_DEFAULT_CONSTRUCTOR(MessageType),
					FIELD_DEFAULT_CONSTRUCTOR(Parameters)
			)	)
		{}



		void NotificationProvider::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix) const
		{
			map.merge(get<Parameters>());
		}


		//////////////////////////////////////////////////////////////////////////
		/// Check which message type should be used when publishing
		/// an notification event
		std::set<messages::MessageType*> NotificationProvider::getMessageTypes() const
		{
			return get<MessageType>() ? &*get<MessageType>() : NULL;
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



		//////////////////////////////////////////////////////////////////////////
		/// Check of a message should be sent to the mailing list according to its recipients.
		/// A notification can only be sent if it is explicitly linked to the message.
		/// @param recipients the recipients list to check
		/// @param parameters the broadcast parameters
		bool NotificationProvider::displaysMessage(
			const Alarm::LinkedObjects& recipients,
			const util::ParametersMap& parameters
		) const	{

			// if global subscribe all options is set
			if(get<SubscribeAllBegin>() || get<SubscribeAllEnd>())
			{
				return true;
			}

			// in broad cast points recipients
			Alarm::LinkedObjects::const_iterator it(
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
				const Alarm::LinkedObjects::mapped_type::value_type& link,
				it->second
			){
				// Search for explicitly specified notification provider
				if( link->getObjectId() == getKey())
				{
					return true;
				}
			}

			// The notification provider was not found
			return false;
		}


		/*
			Notify the event according to NotificationProvider parameters
			available with "channel" prefix.
		*/
		bool NotificationProvider::notify(const boost::shared_ptr<NotificationEvent>& event) {
			// Create corresponding NotificationChannel
			// Invoke attemptNotification(event)
			// return true only if completely successful
			return true;
		}

	}
}

