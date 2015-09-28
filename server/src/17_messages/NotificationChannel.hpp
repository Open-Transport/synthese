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

#include "FactoryBase.h"
#include "NotificationEvent.hpp"
#include "StringField.hpp"

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
	Alarm.

	Each provider uses an instance of corresponding NotificationChannel
	implementation to attempt to notify an event.

	The method generateScriptFields provides a generic support to implement
	CMS script fields. If an empty value goes out of a required parameter for
	notification, the channel should not notify the event.

	A NotificationChannel (derived class) has to implement timeout support
	and error detection so that the NotificationThread does not remain stuck in
	notifyEvent method.

	A NotificationChannel can have an internal state to optimize
	communication, for instance connection reuse and pooling,
	asynchronously with its own background threads if necessary. It is
	responsible to reset its internal state in case of definitive failure
	of internal components that may prevent subsequent notification
	attempts.

	A NotificationChannel may create additional REMINDER events between received
	BEGIN and END event based on custom provider parameters.

	The NotificationThread is responsible for NotificationEvent life cycle
	and implements error handling and retry mechanism for all notification
	providers.

 */
		class NotificationChannel:
			public util::FactoryBase<NotificationChannel>
		{
		public:
			static const std::string ATTR_KEY;

			static const std::string TIME_FACET_FORMAT;

			// Variable available in CMS fields for any channel
			static const std::string VARIABLE_ALARM_ID;
			static const std::string VARIABLE_SHORT_MESSAGE;
			static const std::string VARIABLE_MESSAGE;
			static const std::string VARIABLE_STOP_IDS;
			static const std::string VARIABLE_LINE_IDS;
			static const std::string VARIABLE_ID_SEPARATOR;
			static const std::string VARIABLE_URL;
			static const std::string VARIABLE_APPLICATION_BEGIN;
			static const std::string VARIABLE_APPLICATION_BEGIN_ISO;
			static const std::string VARIABLE_APPLICATION_END;
			static const std::string VARIABLE_APPLICATION_END_ISO;
			static const std::string VARIABLE_EVENT_TYPE;
			static const std::string VARIABLE_EVENT_TIME;
			static const std::string VARIABLE_EVENT_TIME_ISO;

			NotificationChannel() { };

			virtual ~NotificationChannel() { };

			/// Factory information
			void toParametersMap(util::ParametersMap& pm) const;

			/**
				Generate script fields from an alarm.

				@param provider reference to source notification provider
				@param alarm to use for test
				@param eventType notification type
				@param eventTime notificaction event creation time
				@return generated fields
			*/
			util::ParametersMap generateScriptFields(
				const NotificationProvider* provider,
				const Alarm* alarm,
				const NotificationType eventType,
				const boost::posix_time::ptime& eventTime
			) const;

			/**
				Notify an event for a Terminus alarm.
				May throw exception, will be caught and written to notification log.

				In case of an async notification, false is returned so that retry mechanism
				in NotificationThread will check expiration. The notification event
				Status must be changed by async implementation to SUCCESS.
				Synchronization must be used to protect against concurrent multiple attempts.

				@param event notification event to publish
				@return true if notification succeeded or no content available for notification
			*/
			virtual bool notifyEvent(const boost::shared_ptr<NotificationEvent> event) = 0;

			/**
				Check if an alarm requires for an update event.
				Compare BEGIN event timestamps with Alarm and MessageAlternative update timestamps.
				May throw exception, will be caught and written to notification log.

				This method allows a NotificationChannel derived class to either enforce
				update event or to add specific checks.

				@param alarm update alarm
				@param beginEvent BEGIN notification event for the corresponding alarm and provider
				@return true if an UPDATE notification event must be created
			*/
			virtual bool checkForUpdate(const Alarm* alarm, boost::shared_ptr<NotificationEvent> beginEvent);

		protected:
			/**
				Get a list of parameter names that are implemented as script fields
				@return list of parameter names
			*/
			virtual std::vector<std::string> _getScriptParameterNames() const;

			/**
				Add variables into parameters map used to render script fields.
				@param variables parameters map to feed with variables
			*/
			virtual void _addVariables(util::ParametersMap& variables) const { };

			/**
				Retrieve the message corresponding to the notification provider message type
				@param alarm the messages holder
				@param type message type
				@param[out] messageTypeFound true if message alternative for type was found
				@return message alternative if it was found, alarm long message otherwise
			*/
			static std::string _getMessageAlternative(
				const Alarm* alarm,
				const boost::optional<MessageType&> type,
				bool& messageTypeFound
			);

			/**
				Retrieve the message update timestamp corresponding to the notification provider message type
				@param alarm the messages holder
				@param type message type
				@param[out] messageTypeFound true if message alternative for type was found
				@return message alternative if it was found, alarm long message otherwise
			*/
			static boost::posix_time::ptime _getMessageUpdate(
				const Alarm* alarm,
				const boost::optional<MessageType&> type,
				bool& messageTypeFound
			);

		private:
			/**
				Set message variable according to message type
				@param variables parameters map to feed with message variable
				@param alarm the messages' holder
				@param type the message type to extract
				@return false if message alternative for type has not been found
			*/
			static bool _setMessageVariable(
				util::ParametersMap& variables,
				const Alarm* alarm,
				const boost::optional<MessageType&> type
			);

			/**
				Format ptime to string according to TIME_FACET_FORMAT.
				@param time date time to format
				@return formatted date time
			*/
			static std::string _formatTime(const boost::posix_time::ptime time);

			/**
				Set application date variables from sent alarm.
				@param variables parameters map to feed with date variables
				@param alarm the sent alarm with application dates
				@param eventType type of the notification event (begin/end)
				@param eventTime notification event creation time
			*/
			static void _setApplicationDateVariables(
				util::ParametersMap& variables,
				const Alarm* alarm,
				const NotificationType eventType,
				const boost::posix_time::ptime& eventTime
			);


			/**
				Set fake application date variables for testing script fields.
				@param variables parameters map to feed with date variables
				@param eventTime notification event creation time
			*/
			static void _setTestApplicationDateVariables(
				util::ParametersMap& variables,
				const boost::posix_time::ptime& eventTime
			);

		};
	}
}

#endif /* SYNTHESE_messages_NotificationChannel_hpp__ */
