
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

#include "TreeFolderTableSync.hpp"

#include "Conversion.h"
#include "TreeFolderRoot.hpp"
#include "ReplaceQuery.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"

#include <sstream>

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

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TreeFolderTableSync>::TABLE(
			"t092_tree_folders"
		);

		template<> const Field DBTableSyncTemplate<TreeFolderTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TreeFolderTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					Parent::FIELD.name.c_str(),
					Name::FIELD.name.c_str(),
			"")	);
			return r;
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
				query << " AND " << Parent::FIELD.name << "=" << *parentFolderId;
			}
			if(name)
			{
				query << " AND " << Name::FIELD.name << " LIKE " << Conversion::ToDBString(*name);
			}
			query << " ORDER BY " << Name::FIELD.name << " ASC";
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		TreeFolderTableSync::TreeFolderTableSync()
		{

		}

	}

	namespace db
	{
		template<> bool DBTableSyncTemplate<TreeFolderTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<TreeFolderTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TreeFolderTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TreeFolderTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}
}
