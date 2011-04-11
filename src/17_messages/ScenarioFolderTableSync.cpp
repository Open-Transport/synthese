
/** ScenarioFolderTableSync class implementation.
	@file ScenarioFolderTableSync.cpp
	@author Hugues Romain
	@date 2008

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

#include <sstream>

#include "ScenarioFolderTableSync.h"
#include "ScenarioFolder.h"

#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ReplaceQuery.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "MessagesLibraryRight.h"

#include "Conversion.h"

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

	namespace messages
	{
		const string ScenarioFolderTableSync::COL_NAME("name");
		const string ScenarioFolderTableSync::COL_PARENT_ID("parent_id");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ScenarioFolderTableSync>::TABLE(
			"t051_scenario_folder"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<ScenarioFolderTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(ScenarioFolderTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(ScenarioFolderTableSync::COL_PARENT_ID, SQL_INTEGER),
			DBTableSync::Field()
		};
		
		template<> const DBTableSync::Index DBTableSyncTemplate<ScenarioFolderTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(ScenarioFolderTableSync::COL_PARENT_ID.c_str(), ""),
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<ScenarioFolderTableSync,ScenarioFolder>::Load(
			ScenarioFolder* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(ScenarioFolderTableSync::COL_NAME));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(ScenarioFolderTableSync::COL_PARENT_ID));

				if(id > 0)
				{
					object->setParent(
						ScenarioFolderTableSync::GetEditable(
							id,
							env,
							linkLevel
						).get()
					);
				}
			}
		}



		template<> void DBDirectTableSyncTemplate<ScenarioFolderTableSync,ScenarioFolder>::Save(
			ScenarioFolder* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ScenarioFolderTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getParent() ? object->getParent()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<ScenarioFolderTableSync,ScenarioFolder>::Unlink(
			ScenarioFolder* obj
		){
		}



		template<> bool DBTableSyncTemplate<ScenarioFolderTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;
			ScenarioTemplateInheritedTableSync::Search(
				env, 
				object_id,
				string(),
				NULL,
				0,
				1
			);
			if (!env.getRegistry<ScenarioTemplate>().empty())
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
				query << " AND " << COL_PARENT_ID << "=" << Conversion::ToString(*parentFolderId);
			}
			if(name)
			{
				query << " AND " << COL_NAME << " LIKE " << Conversion::ToDBString(*name);
			}
			query << " ORDER BY " << COL_NAME << " ASC";
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
