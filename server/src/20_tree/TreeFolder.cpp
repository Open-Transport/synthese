
/** TreeFolder class implementation.
	@file TreeFolder.cpp

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

#include "TreeFolder.hpp"

#include "Profile.h"
#include "Session.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace tree;
	

	CLASS_DEFINITION(TreeFolder, "t092_tree_folders", 92)


	namespace tree
	{
		TreeFolder::TreeFolder(
			RegistryKeyType id
		):	Registrable(id),
			Object<TreeFolder, TreeFolderRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Parent)
			)	)
		{}

		TreeFolder::~TreeFolder()
		{
			unlink();
		}



		void TreeFolder::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}



		void TreeFolder::unlink()
		{
			if(_getParent())
			{
				setNullParent();
			}

			BOOST_FOREACH(TreeFolderDownNodeInterface* treeFolderDownNode, getChildren<TreeFolderDownNodeInterface>())
			{
				treeFolderDownNode->setNullParent();
			}
		}


		bool TreeFolder::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool TreeFolder::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool TreeFolder::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}

