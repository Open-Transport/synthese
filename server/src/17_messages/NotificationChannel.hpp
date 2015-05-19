/** NotificationChannel class header
	@file NotificationChannel.hpp
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
#ifndef SYNTHESE_messages_NotificationChannel_hpp__
#define SYNTHESE_messages_NotificationChannel_hpp__

#include <FactoryBase.h>
#include <NotificationEvent.hpp>
#include <StringField.hpp>

#include <string>
#include <vector>

namespace synthese
{
	namespace util
	{
		class ParametersMap;			// Forward declaration
	}

	namespace messages {
		class Alarm;					// Forward declaration
		class NotificationProvider;		// Forward declaration

/**
	@class NotificationChannel
	@ingroup m17
	@author yves.martin
	@date 2015

	Notification channel factorable base class.

	Each NotificationChannel subclass provides a way to broadcast messages
	out of Synthese system.

	Message broadcasting

	Each registered notification provider is notified by
	MessagesActivationThread about begin and end period for each active
	SentAlarm.

	Each provider uses an instance of corresponding NotificationChannel
	implementation to attempt to notify an event.

	The method generateScriptFields provides a generic support to implement
	CMS script fields. If an empty value goes out of a required parameter for
	notification, the channel should not notify the event.

	A NotificationChannel has to implement timeout support and error
	detection so that the NotificationThread does not remain stuck in
	notify method.

	A NotificationChannel can have an internal state to optimize
	communication, for instance connection reuse and pooling,
	asynchronously with its own background threads if necessary. It is
	responsible to reset its internal state in case of definitive failure
	of internal components that may prevent subsequent notification
	attempts.

	The NotificationThread is responsible for NotificationEvent management
	and implements error handling and retry mechanism for all notification
	providers.
 */
		class NotificationChannel:
			public util::FactoryBase<NotificationChannel>
		{
		public:
			static const std::string ATTR_KEY;

			static const std::string VARIABLE_SHORT_MESSAGE;
			static const std::string VARIABLE_MESSAGE;
			static const std::string VARIABLE_STOP_IDS;
			static const std::string VARIABLE_LINE_IDS;
			static const std::string VARIABLE_ID_SEPARATOR;

			NotificationChannel() { };

			virtual ~NotificationChannel() { };

			/// Factory information
			void toParametersMap(util::ParametersMap& pm) const;

			/**
				Generate script fields from an alarm
				@param alarm to use for test
				@return generated fields
			*/
			util::ParametersMap generateScriptFields(
				const boost::shared_ptr<NotificationProvider> provider,
				const boost::shared_ptr<Alarm> alarm,
				const NotificationType eventType
			) const;

			/**
				Notify an event for a Terminus alarm
				@param event notification event to publish
				@return true if notification succeeded
			*/
			virtual bool notify(const boost::shared_ptr<NotificationEvent> event) = 0;

		protected:
			/**
				Get a list of parameter names that are implemented as script fields
				@return list of parameter names
			*/
			virtual std::vector<std::string> _getScriptParameterNames() const;

		private:
			/**
				Set message variable according to message type
				@param scriptParameters parameters map to feed with message
				@param alarm the messages' holder
				@param type the message type to extract
			 */
			void _setMessageAlternative(
				util::ParametersMap& scriptParameters,
				const boost::shared_ptr<Alarm> alarm,
				boost::optional<MessageType&> type
			) const;

		};
	}
}

#endif /* SYNTHESE_messages_NotificationChannel_hpp__ */
