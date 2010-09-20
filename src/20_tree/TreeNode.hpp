
/** TreeNode class header.
	@file TreeNode.hpp

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

#ifndef SYNTHESE_util_TreeNode_hpp__
#define SYNTHESE_util_TreeNode_hpp__

#include <map>
#include <deque>
#include <assert.h>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace tree
	{
		/** Template for creating tree classes without any container.
			@ingroup m20

			TreeNode objects are ordered in a auto-linked tree structure : objects know both their parent and children.

			An object without parent is considered "at the root of the tree".

			More than one object can be at the root of the tree theoretically.
			In this case, some methods cannot return any result, because each root is considered as a separated tree.
			These methods are marked in the documentation by "not for root".
		*/
		template<
			class ObjectType_,
			template<class> class OrderingPolicy_,
			class RootPolicy_
		>	class TreeNode:
			public OrderingPolicy_<ObjectType_>,
			public RootPolicy_
		{
		public:
			typedef ObjectType_ ObjectType;
			typedef OrderingPolicy_<ObjectType_> OrderingPolicy;
			typedef RootPolicy_ RootPolicy;
			typedef TreeNode<ObjectType_, OrderingPolicy_, RootPolicy_> TreeNodeType;

			typedef std::map<typename OrderingPolicy::OrderingKeyType, ObjectType*> ChildrenType;

		private:
			//! @name Tree node informations
			//@{
				ObjectType*	_parent;
				ChildrenType _children;
			//@}

		public:

			TreeNode() : _parent(NULL) { }
			~TreeNode() {}

			//! @name Getters
			//@{
				const ChildrenType& getChildren() const { return _children; }
				ObjectType* getParent() const { return _parent; }
			//@}

			//! @name Setters
			//@{
				void setParent(ObjectType* object){	_parent = object; }
			//@}

			//! @name Updaters
			//@{
				void addChild(ObjectType* object) { _children.insert(std::make_pair(object->getTreeOrderingKey(), object)); }
				void removeChild(ObjectType* object) { _children.erase(object->getTreeOrderingKey()); }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets the depth of the node position in the tree.
				/// @return the depth of the node position in the tree (0 if the node is at the root)
				/// @author Hugues Romain
				/// @date 2010
				std::size_t getDepth() const
				{
					std::size_t result(0);
					for(ObjectType* parent(_parent); parent; parent = parent->_parent, ++result) ;
					return result;
				}



				//////////////////////////////////////////////////////////////////////////
				/// Gets the next sibling object.
				//////////////////////////////////////////////////////////////////////////
				/// @return NULL if the object is the last item of the parents children
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.2.0
				ObjectType* getNextSibling() const
				{
					if(!_parent)
					{
						return NULL;
					}
					typename ChildrenType::const_iterator it(_parent->_children.find(this->getTreeOrderingKey()));
					if(it == _parent->_children.end())
					{
						return NULL;
					}
					++it;
					if(it == _parent->_children.end())
					{
						return NULL;
					}
					return it->second;
				}



				//////////////////////////////////////////////////////////////////////////
				/// Gets the previous sibling object.
				//////////////////////////////////////////////////////////////////////////
				/// @return NULL if the object is the first item of the parents children
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.2.0
				ObjectType* getPreviousSibling() const
				{
					if(!_parent)
					{
						return NULL;
					}
					typename ChildrenType::const_iterator it(_parent->_children.find(this->getTreeOrderingKey()));
					if(it == _parent->_children.end() || it == _parent->_children.begin())
					{
						return NULL;
					}
					--it;
					return it->second;
				}

			//@}
		};
	}
}

#endif // SYNTHESE_util_TreeNode_hpp__
