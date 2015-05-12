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

#include <FactorableTemplate.h>
#include <BroadcastPoint.hpp>
#include <MessageType.hpp>
#include <NumericField.hpp>
#include <Object.hpp>
#include <ParametersMapField.hpp>
#include <PointerField.hpp>
#include <SchemaMacros.hpp>
#include <StringField.hpp>
#include <string>
#include <vector>

/** @defgroup refFile 17 Notification providers
	@ingroup ref
*/

namespace synthese
{
	namespace messages {

		FIELD_BOOL(SubscribeAllBegin)
		FIELD_BOOL(SubscribeAllEnd)
		FIELD_INT(RetryAttemptDelay)
		FIELD_INT(MaximumRetryAttempts)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),

			// Flag to force processing of all sent alarms for begin events
			FIELD(SubscribeAllBegin),

			// Flag to force processing of all sent alarms for end events
			FIELD(SubscribeAllEnd),

			// Delay in milliseconds between notification event retry
			FIELD(RetryAttemptDelay),

			// Maximum number of retry attemps before giving up notify the event
			FIELD(MaximumRetryAttempts),

			// Message type for content... TODO requires one for begin, another for end
			FIELD(MessageType),

			// Implementation specific parameters
			FIELD(Parameters)
		> NotificationProviderRecord;

		/**
			@class NotificationProvider
			@ingroup m17

			Notification provider factorable base class.

			Each NotificationProvider subclass provides a way to
			broadcast message out of Synthese system.

			Message broadcasting

			Each provider is notify by MessagesActivationThread
			about begin and end period for each active SentAlarm.

			It has to create a NotificationEvent which will persist for
			relevant event (begin or end).

			The NotificationThread will submit back events to NotificationProvider
			so that the technical job of broadcasting is achieved. In case of
			failure an exception must be thrown.

			The NotificationThread implements error handling and retry mechanism
			for all notification providers.
		 */
		class NotificationProvider:
				public Object<NotificationProvider, NotificationProviderRecord>,
				public util::FactorableTemplate<BroadcastPoint, NotificationProvider>
		{
		public:
			/**
			 * NotificationProvider key in parameters map
			 */
			static const std::string ATTR_KEY;

			NotificationProvider(util::RegistryKeyType id=0);

			virtual ~NotificationProvider() {};

			/**
			 * Map of notification providers:
			 * - provider name as key
			 * - NotificationBroadcastPoint implementation as value
			 */
			typedef std::vector<std::string> NotificationProvidersKeyList;

			/**
			 * Provides the list of registered NotificationBroadcastPoint implementation
			 */
			static NotificationProvidersKeyList GetNotificationProviders();

			/// @name BroadcastPoint virtual methods
			//@{
				virtual messages::MessageType* getMessageType() const;

				virtual bool displaysMessage(
					const messages::Alarm::LinkedObjects& recipients,
					const util::ParametersMap& parameters
				) const;

				virtual void getBroadcastPoints(BroadcastPoints& result) const;
			//@}
		};
	}
}

#endif /* SYNTHESE_messages_NotificationProvider_hpp__ */
