
/** TreeFolderTableSync class implementation.
	@file TreeFolderTableSync.cpp
	@author Hugues
	@date 2010

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

#include "TreeFolderTableSync.hpp"
#include "TreeFolderRoot.hpp"
#include "ReplaceQuery.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLiteException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace tree;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,TreeFolderTableSync>::FACTORY_KEY("20 Tree folders");
	}

	namespace tree
	{
		const string TreeFolderTableSync::COL_PARENT_ID ("parent_id");
		const string TreeFolderTableSync::COL_NAME ("name");
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<TreeFolderTableSync>::TABLE(
			"t051_scenario_folder"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<TreeFolderTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(TreeFolderTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(TreeFolderTableSync::COL_PARENT_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<TreeFolderTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				TreeFolderTableSync::COL_PARENT_ID.c_str(),
				TreeFolderTableSync::COL_NAME.c_str(),
			""),
			SQLiteTableSync::Index()
		};




		template<> void SQLiteDirectTableSyncTemplate<TreeFolderTableSync,TreeFolderRoot>::Load(
			TreeFolderRoot* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(TreeFolderTableSync::COL_NAME));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType pid(rows->getLongLong(TreeFolderTableSync::COL_PARENT_ID));
				if(pid > 0)
				{
					try
					{
//						object->setParent(GetEditable(pid, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<TreeFolderRoot>& e)
					{
						Log::GetInstance().warn("No such parent "+ lexical_cast<string>(pid) +" in folder "+ lexical_cast<string>(object->getKey()));
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<TreeFolderTableSync,TreeFolderRoot>::Save(
			TreeFolderRoot* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<TreeFolderTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->_getParent() ? object->_getParent()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<TreeFolderTableSync,TreeFolderRoot>::Unlink(
			TreeFolderRoot* obj
		){			
		}
	}
	
	
	
	namespace tree
	{
		TreeFolderTableSync::SearchResult TreeFolderTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> parentFolderId /*= boost::optional<util::RegistryKeyType>()*/,
			boost::optional<std::string> name /*= boost::optional<std::string>()*/,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 "
				;
			if(parentFolderId)
			{
				query << " AND " << COL_PARENT_ID << "=" << *parentFolderId;
			}
			if(name)
			{
				query << " AND " << COL_NAME << " LIKE " << Conversion::ToSQLiteString(*name);
			}
			query << " ORDER BY " << COL_NAME << " ASC";
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
