
/** TreeFolderTableSync class header.
	@file TreeFolderTableSync.h
	@author Hugues
	@date 2010

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_TreeFolderTableSync_H__
#define SYNTHESE_TreeFolderTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "DBNoSyncTableSyncTemplate.hpp"
#include "TreeFolderRoot.hpp"

namespace synthese
{
	namespace tree
	{
		/** TreeFolder table synchronizer.
			@ingroup m20LS refLS
		*/
		class TreeFolderTableSync:
			public db::DBNoSyncTableSyncTemplate<TreeFolderTableSync,TreeFolderRoot>
		{
		public:
			static const std::string COL_PARENT_ID;
			static const std::string COL_NAME;

			TreeFolderTableSync();

			template<class ObjectType_>
			static boost::shared_ptr<typename ObjectType_::TreeFolderType> GetTreeFolder(
				util::RegistryKeyType id,
				util::Env& env,
				util::LinkLevel linkLevel
			){
/*				if(env.getEditableRegistry<TreeFolderRoot>().contains(id))
				{
					return env.getEditable<TreeFolderRoot>(id);
				}

*/				boost::shared_ptr<typename ObjectType_::TreeFolderType> object;
/*				try
				{
					db::DBResultSPtr rows(db::DBTableSyncTemplate<TreeFolderTableSync>::_GetRow(id));
					object.reset(new typename ObjectType_::TreeFolderType(rows->getKey()));
					Load(object.get(), rows, env, linkLevel);
				}
				catch (typename db::DBEmptyResultException<TreeFolderRoot>&)
				{
					throw util::ObjectNotFoundException<ObjectType_>(id, "Object not found in "+ TreeFolderTableSync::TABLE.NAME);
				}

				env.getEditableRegistry<TreeFolderRoot>().add(boost::static_pointer_cast<TreeFolderRoot, typename ObjectType_::TreeFolderType>(object));
*/				return object;
			}



			/** Tree folder search.
				@param env Environment to populate
				@param parentFolderId ID of the parent folder of the result
				@param name Name of the folders
				@param first First  object to answer
				@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found objects.
				@author Hugues Romain
				@date 2010
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> parentFolderId = boost::optional<util::RegistryKeyType>(),
				boost::optional<std::string> name = boost::optional<std::string>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_TreeFolderTableSync_H__
