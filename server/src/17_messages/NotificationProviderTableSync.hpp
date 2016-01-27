/** NotificationProviderTableSync class header.
	@file NotificationProviderTableSync.hpp
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
#ifndef SYNTHESE_messages_NotificationProviderTableSync_hpp__
#define SYNTHESE_messages_NotificationProviderTableSync_hpp__

#include <DBDirectTableSyncTemplate.hpp>
#include <NotificationProvider.hpp>

namespace synthese
{
	namespace messages
	{
		class NotificationProviderTableSync:
			public db::DBDirectTableSyncTemplate<NotificationProviderTableSync, NotificationProvider>
		{
		public:
			virtual ~NotificationProviderTableSync();

			static SearchResult Search(
				util::Env& env,
				std::string name = std::string(),
				boost::optional<util::RegistryKeyType> parentId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByName = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			template<class OutputIterator>
			static 
				void Search(
				util::Env& env,
				OutputIterator result,
				std::string name = std::string(),
				boost::optional<util::RegistryKeyType> parentId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByName = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			)
			{
				db::SelectQuery<NotificationProviderTableSync> query;
				if (!name.empty())
				{
					query.addWhereField(SimpleObjectFieldDefinition<Name>::FIELD.name, name, db::ComposedExpression::OP_LIKE);
				}
				if(parentId)
				{
					query.addWhereField(
						ComplexObjectFieldDefinition<NotificationProviderTreeNode>::FIELDS[1].name,
						*parentId,
						db::ComposedExpression::OP_EQ
					);
				}
				if (orderByName)
				{
					query.addOrderField(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
				}
				if (number)
				{
					query.setNumber(*number + 1);
				}
				if (first > 0)
				{
					query.setFirst(first);
				}

				SearchResult searchResult =
					LoadFromQuery(query.toString(), env, linkLevel);
				std::copy(searchResult.begin(), searchResult.end(), result);
		}			
			
		};

	} /* namespace messages */
} /* namespace synthese */

#endif /* SYNTHESE_messages_NotificationProviderTableSync_hpp__ */
