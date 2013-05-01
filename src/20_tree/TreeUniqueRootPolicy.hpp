
/** TreeUniqueRootPolicy class header.
	@file TreeUniqueRootPolicy.hpp

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

#ifndef SYNTHESE_tree_TreeUniqueRootPolicy_hpp__
#define SYNTHESE_tree_TreeUniqueRootPolicy_hpp__

#include "TreeRoot.hpp"

#include <boost/thread/detail/singleton.hpp>

namespace synthese
{
	namespace tree
	{
		/** TreeUniqueRootPolicy class.
			@ingroup m20
		*/
		template<
			template<class> class OrderingPolicy_,
			class T
		>
		class TreeUniqueRootPolicy:
			public TreeRoot<T, OrderingPolicy_>
		{
		public:
			typedef T RootType;

			TreeUniqueRootPolicy() {}

			static T* getRoot() { return &boost::detail::thread::singleton<T>::instance(); }

			bool hasRoot() const { return true; }

			void setRoot(RootType* value){}

			void setSameRoot(const TreeUniqueRootPolicy<OrderingPolicy_, T>&){}

			const typename TreeRoot<T, OrderingPolicy_>::ChildrenType& getRootChildren() const { return getRoot()->getChildren(); }

			void registerChildToRoot(T& child)
			{
				getRoot()->getChildren().insert(
					std::make_pair(child.getTreeOrderingKey(), &child)
				);
			}

			void unregisterChildFromRoot(T& child)
			{
				getRoot()->getChildren().erase(
					child.getTreeOrderingKey()
				);
			}
		};
	}
}

#endif // SYNTHESE_tree_TreeUniqueRootPolicy_hpp__
