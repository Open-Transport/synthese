
/** MessagesSection class implementation.
	@file MessagesSection.cpp

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

#include "MessagesSection.hpp"

#include "MessagesRight.h"
#include "MessagesSectionTableSync.hpp"
#include "Profile.h"
#include "RankUpdateQuery.hpp"
#include "User.h"

namespace synthese
{
	CLASS_DEFINITION(messages::MessagesSection, "t109_messages_sections", 109)
	FIELD_DEFINITION_OF_OBJECT(messages::MessagesSection, "messages_section_id", "messages_section_ids")

	FIELD_DEFINITION_OF_TYPE(Color, "color", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(AutoActivation, "auto_activation", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(SetManualOverride, "set_manual_override", SQL_BOOLEAN)

	using namespace db;
	
	namespace messages
	{
		MessagesSection::MessagesSection(
			util::RegistryKeyType id/*=0*/
		):	Registrable(id),
			Object<MessagesSection, MessagesSectionRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_VALUE_CONSTRUCTOR(Rank, 0),
					FIELD_DEFAULT_CONSTRUCTOR(Code),
					FIELD_DEFAULT_CONSTRUCTOR(Color),
					FIELD_VALUE_CONSTRUCTOR(AutoActivation, false),
					FIELD_VALUE_CONSTRUCTOR(SetManualOverride, true)
			)	)
		{}



		void MessagesSection::beforeUpdate(
			const ObjectBase& existingObject,
			boost::optional<db::DBTransaction&> transaction
		) const	{

			const MessagesSection& existingSection(static_cast<const MessagesSection&>(existingObject));

			// Rank has been decreased
			if(get<Rank>() < existingSection.get<Rank>())
			{
				RankUpdateQuery<MessagesSectionTableSync> query(
					Rank::FIELD.name,
					1,
					get<Rank>(),
					existingSection.get<Rank>() - 1
				);
				query.execute(transaction);
			}
			else if(get<Rank>() > existingSection.get<Rank>()) // Rank has been increased
			{
				RankUpdateQuery<MessagesSectionTableSync> query(
					Rank::FIELD.name,
					-1,
					existingSection.get<Rank>() + 1,
					get<Rank>()
				);
				query.execute(transaction);
			}
		}



		void MessagesSection::afterDelete( boost::optional<db::DBTransaction&> transaction ) const
		{
			// Shift other ranks
			RankUpdateQuery<MessagesSectionTableSync> query(
				Rank::FIELD.name,
				-1,
				get<Rank>() + 1
			);
			query.execute(transaction);
		}



		void MessagesSection::beforeCreate( boost::optional<db::DBTransaction&> transaction ) const
		{
			// Shift other ranks
			RankUpdateQuery<MessagesSectionTableSync> query(
				Rank::FIELD.name,
				1,
				get<Rank>()
			);
			query.execute(transaction);
		}


		bool MessagesSection::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool MessagesSection::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool MessagesSection::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::DELETE_RIGHT);
		}
}	}

