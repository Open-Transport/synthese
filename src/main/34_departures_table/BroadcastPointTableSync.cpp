
/** BroadcastPointTableSync class implementation.
	@file BroadcastPointTableSync.cpp

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

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"
#include "15_env/EnvModule.h"

#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/BroadcastPointTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace departurestable;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<BroadcastPoint>::TABLE_NAME = "t037_broadcast_points";
		template<> const int SQLiteTableSyncTemplate<BroadcastPoint>::TABLE_ID = 37;
		template<> const bool SQLiteTableSyncTemplate<BroadcastPoint>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<BroadcastPoint>::load(BroadcastPoint* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setName(rows.getColumn(rowId, BroadcastPointTableSync::TABLE_COL_NAME));
			object->setConnectionPlace(EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(rows.getColumn(rowId, BroadcastPointTableSync::TABLE_COL_PLACE_ID))));
			object->setPhysicalStop(EnvModule::getPhysicalStops().get(Conversion::ToLongLong(rows.getColumn(rowId, BroadcastPointTableSync::TABLE_COL_PHYSICAL_STOP_ID))));
		}

		template<> void SQLiteTableSyncTemplate<BroadcastPoint>::save(BroadcastPoint* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId(1,1));

			query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< "," << Conversion::ToString(object->getConnectionPlace() ? object->getConnectionPlace()->getKey() : 0)
				<< "," << Conversion::ToString(object->getPhysicalStop() ? object->getPhysicalStop()->getKey() : 0)
				<< ")";

			sqlite->execUpdate(query.str());
		}

	}

	namespace departurestable
	{
		const std::string BroadcastPointTableSync::TABLE_COL_NAME = "name";
		const std::string BroadcastPointTableSync::TABLE_COL_PLACE_ID = "place_id";
		const std::string BroadcastPointTableSync::TABLE_COL_PHYSICAL_STOP_ID = "physical_stop_id";

		BroadcastPointTableSync::BroadcastPointTableSync()
			: SQLiteTableSyncTemplate<BroadcastPoint>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_PLACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_PHYSICAL_STOP_ID, "INTEGER", true);

			addTableIndex(TABLE_COL_PLACE_ID);
			addTableIndex(TABLE_COL_PHYSICAL_STOP_ID);
		}

		void BroadcastPointTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (DeparturesTableModule::getBroadcastPoints().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					load(DeparturesTableModule::getBroadcastPoints().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))).get(), rows, i);
				}
				else
				{
					shared_ptr<BroadcastPoint> object(new BroadcastPoint());
					load(object.get(), rows, i);
					DeparturesTableModule::getBroadcastPoints().add(object);
				}
			}
		}

		void BroadcastPointTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				try
				{
					shared_ptr<BroadcastPoint> object = DeparturesTableModule::getBroadcastPoints().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
					load(object.get(), rows, i);
				}
				catch (Exception& e)
				{
					
				}
			}
		}

		void BroadcastPointTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				DeparturesTableModule::getBroadcastPoints().remove(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
			}
		}

		std::vector<shared_ptr<BroadcastPoint> > BroadcastPointTableSync::search(
			uid placeId
			, int number
			, int first
			, bool orderByName
			, bool raisingOrder
		){
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 " ;
			if (placeId != UNKNOWN_VALUE)
				query << " AND " << TABLE_COL_PLACE_ID << "=" << placeId;
			if (orderByName)
				query << " ORDER BY " << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
 			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<BroadcastPoint> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<BroadcastPoint> object(new BroadcastPoint());
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
