
/** TreeNode class header.
	@file TreeNode.hpp

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

#ifndef SYNTHESE_util_TreeNode_hpp__
#define SYNTHESE_util_TreeNode_hpp__

#include "InconsistentTreeException.hpp"

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

			typedef std::multimap<typename OrderingPolicy::OrderingKeyType, ObjectType*> ChildrenType;

		private:
			//! @name Tree node informations
			//@{
				boost::optional<ObjectType*> _parent;
				ChildrenType _children;
			//@}

		public:
			//! @name Getters
			//@{
				const ChildrenType& getChildren() const { return _children; }
				ChildrenType& getChildren() { return _children; }
				ObjectType* getParent(bool allowNULL = false) const;
			//@}

			//! @name Setters
			//@{
				void setParent(ObjectType* parent)
				{
					_parent = parent;
					if(parent)
					{
						this->setSameRoot(*parent);
					}
				}
			//@}

			//! @name Services
			//@{
				void registerInParentOrRoot();
				void unregisterInParentOrRoot();


				//////////////////////////////////////////////////////////////////////////
				/// Gets the depth of the node position in the tree.
				/// @return the depth of the node position in the tree (0 if the node is at the root)
				/// @author Hugues Romain
				/// @date 2010
				std::size_t getDepth() const
				{
					std::size_t result(1);
					for(ObjectType* parent(getParent()); parent; parent = parent->getParent(), ++result) ;
					return result;
				}



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the object is a child of an other one.
				/// @param other object that should be parent of this object
				/// @return true or false
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.2.0
				bool isChildOf(const ObjectType& other) const
				{
					for(ObjectType* parent(getParent()); parent; parent = parent->getParent())
					{
						if(parent == &other)
						{
							return true;
						}
					}
					return false;
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
						throw InconsistentTreeException();
					}

					typename ChildrenType::const_iterator it;
					if(*_parent)
					{
						it = (*_parent)->_children.find(this->getTreeOrderingKey());
						if(it == (*_parent)->_children.end())
						{
							throw InconsistentTreeException();
						}
						++it;
						if(it == (*_parent)->_children.end())
						{
							return NULL;
						}
					}
					else
					{
						it = this->getRootChildren().find(this->getTreeOrderingKey());
						if(it == this->getRootChildren().end())
						{
							throw InconsistentTreeException();
						}
						++it;
						if(it == this->getRootChildren().end())
						{
							return NULL;
						}
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
						throw InconsistentTreeException();
					}
					typename ChildrenType::const_iterator it;
					if(*_parent)
					{
						it = (*_parent)->_children.find(this->getTreeOrderingKey());
						if(it == (*_parent)->_children.end())
						{
							throw InconsistentTreeException();
						}
						if(it == (*_parent)->_children.begin())
						{
							return NULL;
						}
					}
					else
					{
						it = this->getRootChildren().find(this->getTreeOrderingKey());
						if(it == this->getRootChildren().end())
						{
							throw InconsistentTreeException();
						}
						if(it == this->getRootChildren().begin())
						{
							return NULL;
						}
					}
					--it;
					return it->second;
				}

			//@}
		};



		template<
			class ObjectType_,
			template<class> class OrderingPolicy_,
			class RootPolicy_
		>
		ObjectType_* TreeNode<ObjectType_, OrderingPolicy_, RootPolicy_>::getParent(bool allowNULL) const
		{
			if(!_parent)
			{
				if(allowNULL)
				{
					return NULL;
				}
				throw InconsistentTreeException();
			}
			return *_parent;
		}



		template<
			class ObjectType_,
			template<class> class OrderingPolicy_,
			class RootPolicy_
		>
		void TreeNode<ObjectType_, OrderingPolicy_, RootPolicy_>::unregisterInParentOrRoot(
		){
			if(_parent)
			{
				if(*_parent)
				{
					(*_parent)->_children.erase(OrderingPolicy::getTreeOrderingKey());
				}
				else
				{
					if(!this->hasRoot())
					{
						throw InconsistentTreeException();
					}
					unregisterChildFromRoot(static_cast<ObjectType_&>(*this));
				}
			}
		}



		template<
			class ObjectType_,
			template<class> class OrderingPolicy_,
			class RootPolicy_
		>
		void TreeNode<ObjectType_, OrderingPolicy_, RootPolicy_>::registerInParentOrRoot(
		){
			if(_parent)
			{
				if(*_parent)
				{
					(*_parent)->_children.insert(
						std::make_pair(
							OrderingPolicy::getTreeOrderingKey(),
							static_cast<ObjectType*>(this)
					)	);
				}
				else
				{
					if(!this->hasRoot())
					{
						throw InconsistentTreeException();
					}
					this->registerChildToRoot(static_cast<ObjectType_&>(*this));
				}
			}
		}
}	}

#endif // SYNTHESE_util_TreeNode_hpp__
