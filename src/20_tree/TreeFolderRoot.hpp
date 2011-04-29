
/** TreeFolderRoot class header.
	@file TreeFolderRoot.hpp

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

#ifndef SYNTHESE_tree_TreeFolderRoot_hpp__
#define SYNTHESE_tree_TreeFolderRoot_hpp__

#include "Named.h"
#include "Registry.h"
#include "Registrable.h"

namespace synthese
{
	namespace tree
	{
		/** TreeFolderRoot class.
			@ingroup m20
		*/
		class TreeFolderRoot:
			public virtual util::Registrable,
			public util::Named
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<TreeFolderRoot>	Registry;

		private:
			TreeFolderRoot* _parent;

		public:
			TreeFolderRoot(util::RegistryKeyType key = 0) : util::Registrable(key) {}

			TreeFolderRoot* _getParent() const { return _parent; }
			void _setParent(TreeFolderRoot* value) { _parent = value; }
		};
	}
}

#endif // SYNTHESE_tree_TreeFolderRoot_hpp__
