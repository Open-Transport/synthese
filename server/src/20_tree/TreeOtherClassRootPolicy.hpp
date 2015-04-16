
/** TreeOtherClassRootPolicy class header.
	@file TreeOtherClassRootPolicy.hpp

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

#ifndef SYNTHESE_tree_TreeOtherClassRootPolicy_h__
#define SYNTHESE_tree_TreeOtherClassRootPolicy_h__

namespace synthese
{
	namespace tree
	{
		/** TreeOtherClassRootPolicy class.
			@ingroup m20
		*/
		template<class O>
		class TreeOtherClassRootPolicy
		{
		public:
			typedef O RootType;

		private:
			RootType* _root;

		public:
			TreeOtherClassRootPolicy() : _root(NULL) {}

			RootType* getRoot() const { return _root; }

			bool hasRoot() const { return _root != NULL; }

			const typename O::ChildrenType& getRootChildren() const { return getRoot()->getChildren(); }

			void setRoot(RootType* value) { _root=value; }

			void setSameRoot(const TreeOtherClassRootPolicy<O>& value){ _root = value._root; }

			void setNullRoot(){ _root = NULL; }

			void registerChildToRoot(typename O::ChildType& child)
			{
				if(_root)
				{
					_root->getChildren().insert(
						std::make_pair(child.getTreeOrderingKey(), &child)
					);
				}
			}

			void unregisterChildFromRoot(typename O::ChildType& child)
			{
				if(_root)
				{
					_root->getChildren().erase(
						child.getTreeOrderingKey()
					);
				}
			}
		};
	}
}

#endif // SYNTHESE_tree_TreeOtherClassRootPolicy_h__
