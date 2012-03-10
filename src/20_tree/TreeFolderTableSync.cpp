
/** TreeFolderTableSync class implementation.
	@file TreeFolderTableSync.cpp
	@author Hugues
	@date 2010

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

#include "TreeFolderTableSync.hpp"
#include "TreeFolderRoot.hpp"
#include "ReplaceQuery.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace tree;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,TreeFolderTableSync>::FACTORY_KEY("20 Tree folders");
	}

	namespace tree
	{
		const string TreeFolderTableSync::COL_PARENT_ID ("parent_id");
		const string TreeFolderTableSync::COL_NAME ("name");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TreeFolderTableSync>::TABLE(
			"t051_scenario_folder"
		);

		template<> const Field DBTableSyncTemplate<TreeFolderTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TreeFolderTableSync::COL_NAME, SQL_TEXT),
			Field(TreeFolderTableSync::COL_PARENT_ID, SQL_INTEGER),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TreeFolderTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					TreeFolderTableSync::COL_PARENT_ID.c_str(),
					TreeFolderTableSync::COL_NAME.c_str(),
			"")	);
			return r;
		}




		template<> void DBDirectTableSyncTemplate<TreeFolderTableSync,TreeFolderRoot>::Load(
			TreeFolderRoot* object,
			const db::DBResultSPtr& rows,
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
					catch(ObjectNotFoundException<TreeFolderRoot>&)
					{
						Log::GetInstance().warn("No such parent "+ lexical_cast<string>(pid) +" in folder "+ lexical_cast<string>(object->getKey()));
					}
				}
			}
		}



		template<> void DBDirectTableSyncTemplate<TreeFolderTableSync,TreeFolderRoot>::Save(
			TreeFolderRoot* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<TreeFolderTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->_getParent() ? object->_getParent()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<TreeFolderTableSync,TreeFolderRoot>::Unlink(
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
			util::LinkLevel linkLevel
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
				query << " AND " << COL_NAME << " LIKE " << Conversion::ToDBString(*name);
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
