
/** ScenarioFolder class implementation.
	@file ScenarioFolder.cpp

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

#include "ScenarioFolder.h"

#include "MessagesRight.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace util;

	CLASS_DEFINITION(messages::ScenarioFolder, "t051_scenario_folder", 51)
	FIELD_DEFINITION_OF_TYPE(messages::Parent, "parent_id", SQL_INTEGER)
	
	namespace messages
	{
		
		ScenarioFolder::ScenarioFolder(RegistryKeyType key)
		:	Registrable(key),
			Object<ScenarioFolder, ScenarioFolderRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Parent)
					)
				)
		{

		}

		std::string ScenarioFolder::getFullName() const
		{
			string result;
			for(const ScenarioFolder* folder(this); folder != NULL; folder = &folder->get<Parent>().get())
			{
				result = "/" + folder->getName() + result;
			}
			return result;
		}


		bool ScenarioFolder::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool ScenarioFolder::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::WRITE);
		}

		bool ScenarioFolder::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::DELETE_RIGHT);
		}

}	}
