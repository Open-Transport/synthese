
/** MessageType class implementation.
	@file MessageType.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "MessageType.hpp"

#include "MessagesRight.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"

namespace synthese
{
	using namespace messages;
	using namespace util;

	CLASS_DEFINITION(MessageType, "t101_message_types", 101)
	FIELD_DEFINITION_OF_OBJECT(MessageType, "message_type_id", "message_type_ids")

	FIELD_DEFINITION_OF_TYPE(MaxLength, "max_length", SQL_INTEGER)

	namespace messages
	{
		MessageType::MessageType(
			RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<MessageType, MessageTypeRecord>(
		Schema(
			FIELD_VALUE_CONSTRUCTOR(Key, id),
			FIELD_DEFAULT_CONSTRUCTOR(Name),
			FIELD_VALUE_CONSTRUCTOR(MaxLength, 0)
		)	)
		{
		}



		bool MessageType::isAcceptable( const std::string& message ) const
		{
			if(get<MaxLength>())
			{
				if(message.size() > (size_t)get<MaxLength>())
				{
					return false;
				}
			}

			return true;
		}


		bool MessageType::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool MessageType::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool MessageType::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::DELETE_RIGHT);
		}
}	}
