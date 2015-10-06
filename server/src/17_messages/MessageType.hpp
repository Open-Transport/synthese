
/** MessageType class header.
	@file MessageType.hpp

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

#ifndef SYNTHESE_messages_MessageType_hpp__
#define SYNTHESE_messages_MessageType_hpp__

#include "Object.hpp"

#include "NumericField.hpp"
#include "StringField.hpp"
#include "SchemaMacros.hpp"

namespace synthese
{
	namespace messages
	{
		FIELD_INT(MaxLength)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(MaxLength)
		> MessageTypeRecord;

		//////////////////////////////////////////////////////////////////////////
		/// Message Type class.
		/// @author Hugues Romain
		/// @date 2012
		///	@ingroup m17
		class MessageType:
			public Object<MessageType, MessageTypeRecord>
		{
		public:
			MessageType(
				util::RegistryKeyType id = 0
			);

			//////////////////////////////////////////////////////////////////////////
			/// Checks if a text is acceptable as a message of the current type,
			/// according to the rules defined in the object.
			/// @param message the text to check
			/// @return true if the text can be accepted as a message of the current type
			bool isAcceptable(
				const std::string& message
			) const;

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_messages_MessageType_hpp__

