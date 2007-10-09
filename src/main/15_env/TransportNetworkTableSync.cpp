
/** TransportNetworkTableSync class implementation.
	@file TransportNetworkTableSync.cpp

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

#include "TransportNetworkTableSync.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"

#include "01_util/Conversion.h"

#include <sqlite/sqlite3.h>
#include <boost/logic/tribool.hpp>
#include <assert.h>

using boost::logic::tribool;
using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	template<> const string util::FactorableTemplate<SQLiteTableSync,TransportNetworkTableSync>::FACTORY_KEY("15.20.02 Network transport");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::TABLE_NAME("t022_transport_networks");
		template<> const int SQLiteTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::TABLE_ID(22);
		template<> const bool SQLiteTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::HAS_AUTO_INCREMENT(true);

		template<> void SQLiteTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::load(TransportNetwork* object, const db::SQLiteResultSPtr& rows )
		{
			std::string name (rows->getText (TransportNetworkTableSync::COL_NAME));

			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setName(name);
		}

		template<> void SQLiteTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>::save(TransportNetwork* object)
		{
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());

			query
				<< "REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< ")";
			
			DBModule::GetSQLite()->execUpdate(query.str());
		}

		template<> void SQLiteTableSyncTemplate<TransportNetworkTableSync, TransportNetwork>::_link(TransportNetwork* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{

		}

		template<> void SQLiteTableSyncTemplate<TransportNetworkTableSync, TransportNetwork>::_unlink(TransportNetwork* obj)
		{

		}

	}

	namespace env
	{
		const string TransportNetworkTableSync::COL_NAME("name");

		TransportNetworkTableSync::TransportNetworkTableSync ()
		: SQLiteRegistryTableSyncTemplate<TransportNetworkTableSync,TransportNetwork>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_NAME, "TEXT");
		}


	    
	    std::vector<boost::shared_ptr<TransportNetwork> > TransportNetworkTableSync::search(
			string name
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByName
			, bool raisingOrder
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (!name.empty())
				query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<TransportNetwork> > objects;
				while (rows->next ())
				{
					shared_ptr<TransportNetwork> object(new TransportNetwork());
					load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
