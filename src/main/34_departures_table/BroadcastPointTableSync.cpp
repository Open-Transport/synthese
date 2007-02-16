
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
		const std::string BroadcastPointTableSync::TABLE_COL_ID = "id";
		const std::string BroadcastPointTableSync::TABLE_COL_NAME = "name";
		const std::string BroadcastPointTableSync::TABLE_COL_PLACE_ID = "place_id";
		const std::string BroadcastPointTableSync::TABLE_COL_PHYSICAL_STOP_ID = "physical_stop_id";

		BroadcastPointTableSync::BroadcastPointTableSync()
			: SQLiteTableSyncTemplate<BroadcastPoint>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NAME, "TEXT", true);
			addTableColumn(TABLE_COL_PLACE_ID, "INTEGER", true);
			addTableColumn(TABLE_COL_PHYSICAL_STOP_ID, "INTEGER", true);
		}

		void BroadcastPointTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (DeparturesTableModule::getBroadcastPoints().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					load(DeparturesTableModule::getBroadcastPoints().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))), rows, i);
				}
				else
				{
					BroadcastPoint* object = new BroadcastPoint();
					load(object, rows, i);
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
					BroadcastPoint* object = DeparturesTableModule::getBroadcastPoints().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID)));
					load(object, rows, i);
				}
				catch (Exception e)
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

		std::vector<BroadcastPoint*> BroadcastPointTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " ;
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<BroadcastPoint*> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					BroadcastPoint* object = new BroadcastPoint();
					load(object, result, i);
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
