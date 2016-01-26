
/** MessagesSection class header.
	@file MessagesSection.hpp

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

#ifndef SYNTHESE_messages_MessagesSection_hpp__
#define SYNTHESE_messages_MessagesSection_hpp__

#include "NumericField.hpp"
#include "Object.hpp"
#include "StringField.hpp"

#include "SchemaMacros.hpp"

namespace synthese
{
	FIELD_STRING(Color)
	FIELD_BOOL(AutoActivation)
	FIELD_BOOL(SetManualOverride)

	namespace messages
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Rank),
			FIELD(Code),
			FIELD(Color),
			FIELD(AutoActivation),
			FIELD(SetManualOverride)
		> MessagesSectionRecord;


		/** MessagesSection class.
			@ingroup m17
		*/
		class MessagesSection:
			public Object<MessagesSection, MessagesSectionRecord>
		{
		public:
			MessagesSection(util::RegistryKeyType id=0);

			//////////////////////////////////////////////////////////////////////////
			/// Updates the rank of other objects if the rank has been changed.
			virtual void beforeUpdate(
				const ObjectBase& existingObject,
				boost::optional<db::DBTransaction&> transaction
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Updates the rank of other objects.
			virtual void afterDelete(
				boost::optional<db::DBTransaction&> transaction
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Updates the rank of other objects.
			virtual void beforeCreate(
				boost::optional<db::DBTransaction&> transaction
			) const;

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_messages_MessagesSection_hpp__

