
/** ScenarioFolderTableSync class implementation.
	@file ScenarioFolderTableSync.cpp
	@author Hugues Romain
	@date 2008

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

#include <sstream>
#include <boost/optional/optional_io.hpp>

#include "ScenarioFolderTableSync.h"

#include "Conversion.h"
#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "ScenarioFolder.h"
#include "ScenarioTemplateTableSync.h"
#include "Session.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace messages;
	using namespace security;


	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,ScenarioFolderTableSync>::FACTORY_KEY("17 ScenarioFolder");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ScenarioFolderTableSync>::TABLE(
			"t051_scenario_folder"
		);

		template<> const Field DBTableSyncTemplate<ScenarioFolderTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ScenarioFolderTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(Parent::FIELD.name.c_str(), ""));
			return r;
		}


		template<> bool DBTableSyncTemplate<ScenarioFolderTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;
			ScenarioTemplateTableSync::SearchResult r(
				ScenarioTemplateTableSync::Search(
					env,
					object_id,
					string(),
					NULL,
					0,
					1
			)	);
			if(	!r.empty())
			{
				return false;
			}

			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<ScenarioFolderTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ScenarioFolderTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ScenarioFolderTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace messages
	{
		ScenarioFolderTableSync::SearchResult ScenarioFolderTableSync::Search(
			Env& env,
			optional<RegistryKeyType> parentFolderId
			, optional<string> name
			, int first /*= 0*/
			, boost::optional<std::size_t> number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 "
			;
			if(parentFolderId)
			{
				query << " AND " << Parent::FIELD.name << "=" << *parentFolderId;
			}
			if(name)
			{
				query << " AND " << Name::FIELD.name << " LIKE " << Conversion::ToDBString(*name);
			}
			query << " ORDER BY " << Name::FIELD.name << " ASC";
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
		
		bool ScenarioFolderTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(security::READ);
		}
	}
}
