
/** TreeFolder class header.
	@file TreeFolder.hpp

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

#ifndef SYNTHESE_tree_TreeFolder_hpp__
#define SYNTHESE_tree_TreeFolder_hpp__

#include "NumericField.hpp"
#include "Object.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "SchemaMacros.hpp"
#include "StringField.hpp"
#include "TreeFolderUpNode.hpp"
#include "TreeFolderDownNodeInterface.hpp"

namespace synthese
{
	namespace tree
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Parent)
		> TreeFolderRecord;

		/** TreeFolder class.
			@ingroup m20
		*/
		class TreeFolder:
			public TreeFolderUpNode,
			public TreeFolderDownNodeInterface,
			public Object<TreeFolder, TreeFolderRecord>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<TreeFolder>	Registry;

			TreeFolder(
				util::RegistryKeyType id = 0
			);

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_tree_TreeFolder_hpp__
