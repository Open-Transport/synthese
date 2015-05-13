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
#include <StringField.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;	// Forward declaration
	}

	namespace messages {
		FIELD_STRING(NotificationChannelKey)

/**
	@class NotificationChannel
	@ingroup m17

	Notification channel factorable base class.

	Each NotificationChannel subclass provides a way to broadcast messages
	out of Synthese system.

	Message broadcasting

	Each registered notification provider is notify by
	MessagesActivationThread about begin and end period for each active
	SentAlarm.

	Each provider uses an instance of corresponding NotificationChannel
	implementation to attempt to notify an event.

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

			NotificationChannel() { };

			virtual ~NotificationChannel() { };

			void toParametersMap(util::ParametersMap& pm) const;
		};
	}
}

#endif /* SYNTHESE_messages_NotificationChannel_hpp__ */
