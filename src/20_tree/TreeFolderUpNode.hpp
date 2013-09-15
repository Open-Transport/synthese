
/** TreeFolderUpNode class header.
	@file TreeFolderUpNode.hpp

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

#ifndef SYNTHESE_tree_TreeFolderUpNode_hpp__
#define SYNTHESE_tree_TreeFolderUpNode_hpp__

#include "Registrable.h"

#include <set>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace synthese
{
	namespace tree
	{
		class TreeFolder;
		class TreeFolderDownNodeInterface;

		/** TreeFolderUpNode class.
			@ingroup m20
		*/
		class TreeFolderUpNode:
			public virtual util::Registrable
		{
		private:
			typedef std::set<TreeFolderDownNodeInterface*> Children;

		public:
			typedef std::vector<TreeFolder*> SubFolders;

		private:
			mutable boost::recursive_mutex _childrenMutex;
			Children _children;

		protected:
			TreeFolderUpNode(
				util::RegistryKeyType id = 0
			):	util::Registrable(id)
			{}
			virtual ~TreeFolderUpNode(){}

		private:
			void addChild(TreeFolderDownNodeInterface& value)
			{
				boost::recursive_mutex::scoped_lock lock(_childrenMutex);
				_children.insert(&value);
			}

			void removeChild(TreeFolderDownNodeInterface& value)
			{
				boost::recursive_mutex::scoped_lock lock(_childrenMutex);
				_children.erase(&value);
			}

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the node has no children of any type.
			/// @return if the node has no children of any type.
			bool empty() const;



			//////////////////////////////////////////////////////////////////////////
			/// Get the children of the specified type
			template<class T>
			std::vector<T*> getChildren() const
			{
				std::vector<T*> result;
				boost::recursive_mutex::scoped_lock lock(_childrenMutex);
				BOOST_FOREACH(const Children::value_type& item, _children)
				{
					if(!dynamic_cast<T*>(item))
					{
						continue;
					}
					result.push_back(dynamic_cast<T*>(item));
				}
				return result;
			}

			SubFolders getSubFolders() const;

			friend class TreeFolderDownNodeInterface;

			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > Labels;

			
			/** Labels list containing each folder, indicating the full path in the folder tree.
				@param prefix text to add at the beginning of each item (optional)
				@param forbiddenId id of an item which must not be present in the result
				@return The list
				@author Hugues Romain
				@date 2012
			*/
			Labels getSubFoldersLabels(
				std::string prefix = std::string(),
				boost::optional<util::RegistryKeyType> forbiddenId = boost::optional<util::RegistryKeyType>()
			) const;



			/** Labels list containing each folder, indicating the full path in the folder tree.
				@param labels Labels list to populate
				@param prefix text to add at the beginning of each item (optional)
				@param forbiddenId id of an item which must not be present in the result
				@author Hugues Romain
				@date 2012
			*/
			void getSubFoldersLabels(
				Labels& labels,
				std::string prefix = std::string(),
				boost::optional<util::RegistryKeyType> forbiddenId = boost::optional<util::RegistryKeyType>()
			) const;


			virtual SubObjects getSubObjects() const;
		};
}	}

#endif // SYNTHESE_tree_TreeFolderUpNode_hpp__

