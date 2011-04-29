
/** TreeFolder class header.
	@file TreeFolder.hpp

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

#ifndef SYNTHESE_tree_TreeFolder_hpp__
#define SYNTHESE_tree_TreeFolder_hpp__

#include "TreeFolderRoot.hpp"
#include "TreeFolderChild.hpp"
#include "TreeAlphabeticalOrderingPolicy.hpp"

namespace synthese
{
	namespace tree
	{
		/** TreeFolder class.
			@ingroup m20
		*/
		template<
			class ObjectType_,
			class RootPolicy_
		>
		class TreeFolder:
			public TreeFolderRoot,
			public RootPolicy_
		{
		public:

			typedef TreeFolder<ObjectType_, RootPolicy_> TreeFolderType;
			typedef ObjectType_ ObjectType;

			typedef std::map<std::string, ObjectType*> ChildrenObjects;
			typedef std::map<std::string, TreeFolderType*> ChildrenFolders;

		private:
			ChildrenObjects _childrenObjects;
			ChildrenFolders _childrenFolders;

		public:
			TreeFolder(util::RegistryKeyType key = 0) : TreeFolderRoot(key) {}

			void removeObject(ObjectType* object) { _childrenObjects.erase( object->getName()); }
			void addObject(ObjectType* object) { _childrenObjects.insert(std::make_pair(object->getName(), object)); }
			void removeFolder(TreeFolderType* object) { _childrenFolders.erase( object->getName()); }
			void addFolder(TreeFolderType* object) { _childrenFolders.insert(std::make_pair(object->getName(), object)); }

			void setParent(TreeFolderType* value)
			{
				if(getParent()) getParent()->removeFolder(this);
				_setParent(static_cast<TreeFolderRoot*>(value));
				if(value) value->addFolder(this);
			}

			TreeFolderType* getParent() const
			{
				return static_cast<TreeFolderType*>(_getParent());
			}
		};
	}
}

#endif // SYNTHESE_tree_TreeFolder_hpp__
