
/** TreeFolderUpNode class implementation.
	@file TreeFolderUpNode.cpp

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

#include "TreeFolderUpNode.hpp"

#include "TreeFolder.hpp"

using namespace std;

namespace synthese
{
	namespace tree
	{
		TreeFolderUpNode::SubFolders TreeFolderUpNode::getSubFolders() const
		{
			return getChildren<TreeFolder>();
		}



		TreeFolderUpNode::Labels TreeFolderUpNode::getSubFoldersLabels(
			std::string prefix /*= std::string()*/,
			boost::optional<util::RegistryKeyType> forbiddenId /*= boost::optional<util::RegistryKeyType>() */
		) const {
			Labels labels;
			getSubFoldersLabels(labels, prefix, forbiddenId);
			return labels;
		}



		void TreeFolderUpNode::getSubFoldersLabels(
			Labels& labels,
			std::string prefix /*= std::string()*/,
			boost::optional<util::RegistryKeyType> forbiddenId /*= boost::optional<util::RegistryKeyType>() */
		) const	{

			// Avoid forbidden id
			if (forbiddenId && getKey() == *forbiddenId)
			{
				return;
			}

			// Adds the current item to the labels list
			if(dynamic_cast<const TreeFolder*>(this))
			{
				labels.push_back(make_pair(getKey(), prefix + dynamic_cast<const TreeFolder*>(this)->get<Name>()));
			}
			else
			{
				labels.push_back(make_pair(getKey(), prefix + "(racine)"));
			}

			// Adds the current folder name to the prefix
			string fullPrefix(prefix);
			if(dynamic_cast<const TreeFolder*>(this))
			{
				fullPrefix += dynamic_cast<const TreeFolder*>(this)->get<Name>();
			}
			fullPrefix += "/";

			// Recursion
			BOOST_FOREACH(TreeFolder* folder, getSubFolders())
			{
				folder->getSubFoldersLabels(labels, fullPrefix, forbiddenId);
			}
		}



		bool TreeFolderUpNode::empty() const
		{
			return _children.empty();
		}



		/// Gets all children of the object.
		SubObjects TreeFolderUpNode::getSubObjects() const
		{
			SubObjects r;

			// Lines and folders in a single call
			BOOST_FOREACH(Registrable* item, getChildren<Registrable>())
			{
				r.push_back(item);
			}

			return r;
		}
}	}

