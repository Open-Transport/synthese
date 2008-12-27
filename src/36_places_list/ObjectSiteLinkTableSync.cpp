
/** ObjectSiteLinkTableSync class implementation.
	@file ObjectSiteLinkTableSync.cpp
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

#include "ObjectSiteLinkTableSync.h"
#include "ObjectSiteLink.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace transportwebsite;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,ObjectSiteLinkTableSync>::FACTORY_KEY("36.11 Object Site Links");
	}

	namespace transportwebsite
	{
		const string ObjectSiteLinkTableSync::COL_OBJECT_ID("object_id");
		const string ObjectSiteLinkTableSync::COL_SITE_ID("site_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ObjectSiteLinkTableSync>::TABLE(
			ObjectSiteLinkTableSync::CreateFormat(
				"t001_object_site_links",
				SQLiteTableFormat::CreateFields(
					SQLiteTableSync::Field(ObjectSiteLinkTableSync::COL_OBJECT_ID, SQL_INTEGER),
					SQLiteTableSync::Field(ObjectSiteLinkTableSync::COL_SITE_ID, SQL_INTEGER),
					SQLiteTableSync::Field()
				), SQLiteTableFormat::Indexes(),
				false
		)	);



		template<> void SQLiteDirectTableSyncTemplate<ObjectSiteLinkTableSync,ObjectSiteLink>::Load(
			ObjectSiteLink* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){

		}



		template<> void SQLiteDirectTableSyncTemplate<ObjectSiteLinkTableSync,ObjectSiteLink>::Save(
			ObjectSiteLink* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<ObjectSiteLinkTableSync,ObjectSiteLink>::Unlink(
			ObjectSiteLink* obj
		){
			/// @todo Fill it
		}
	}
	
	
	
	namespace transportwebsite
	{
		ObjectSiteLinkTableSync::ObjectSiteLinkTableSync()
			: SQLiteDirectTableSyncTemplate<ObjectSiteLinkTableSync, ObjectSiteLink>()
		{
		}



		void ObjectSiteLinkTableSync::rowsAdded(
			db::SQLite* sqlite
			, db::SQLiteSync* sync
			, const SQLiteResultSPtr& rows
			, bool isItFirstSync
		){
			while(rows->next())
			{
			}
		}


		
		void ObjectSiteLinkTableSync::rowsUpdated(
			db::SQLite* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
		){
			while(rows->next())
			{
			}
		}



		void ObjectSiteLinkTableSync::rowsRemoved(
			db::SQLite* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
		){
			while(rows->next())
			{
			}
		}



		void ObjectSiteLinkTableSync::Search(
			Env& env,
			uid siteId
			, int first /*= 0*/
			, int number, /*= 0*/
			LinkLevel linkLevel
		){
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			/// @todo Fill Where criteria
			// if (!name.empty())
			// 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
				;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
