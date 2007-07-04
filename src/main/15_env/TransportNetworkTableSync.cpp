
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

#include "15_env/TransportNetworkTableSync.h"
#include "15_env/TransportNetwork.h"
#include "15_env/EnvModule.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

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

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<TransportNetwork>::TABLE_NAME("t022_transport_networks");
		template<> const int SQLiteTableSyncTemplate<TransportNetwork>::TABLE_ID(22);
		template<> const bool SQLiteTableSyncTemplate<TransportNetwork>::HAS_AUTO_INCREMENT(true);

		template<> void SQLiteTableSyncTemplate<TransportNetwork>::load(TransportNetwork* object, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			std::string name (rows.getColumn (rowIndex, TransportNetworkTableSync::COL_NAME));

			object->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));
			object->setName(name);
		}

		template<> void SQLiteTableSyncTemplate<TransportNetwork>::save(TransportNetwork* object)
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

	}

	namespace env
	{
		const string TransportNetworkTableSync::COL_NAME("name");

		TransportNetworkTableSync::TransportNetworkTableSync ()
		: SQLiteTableSyncTemplate<TransportNetwork>(true, true, db::TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_NAME, "TEXT");
		}


		void TransportNetworkTableSync::rowsAdded( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync /*= false*/ )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id (Conversion::ToLongLong (rows.getColumn (i, TABLE_COL_ID)));

				shared_ptr<TransportNetwork> object;
				if (EnvModule::getTransportNetworks().contains(id))
				{
					object = EnvModule::getTransportNetworks().getUpdateable(id);
					load(object.get(), rows, i);
				}
				else
				{
					object.reset(new TransportNetwork);
					load(object.get(), rows, i);
					EnvModule::getTransportNetworks ().add (object);
				}
			}
		}

		void TransportNetworkTableSync::rowsUpdated( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id (Conversion::ToLongLong (rows.getColumn (i, TABLE_COL_ID)));
				
				if (!EnvModule::getTransportNetworks().contains(id))
					continue;

				shared_ptr<TransportNetwork> object = EnvModule::getTransportNetworks().getUpdateable(id);
				load(object.get(), rows, i);
			}
		}

		void TransportNetworkTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id (Conversion::ToLongLong (rows.getColumn (i, TABLE_COL_ID)));

				EnvModule::getTransportNetworks().remove(id);
			}
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
				SQLiteResult result = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<TransportNetwork> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<TransportNetwork> object(new TransportNetwork());
					load(object.get(), result, i);
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
