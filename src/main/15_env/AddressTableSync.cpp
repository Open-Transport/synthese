

/** AddressTableSync class implementation.
	@file AddressTableSync.cpp

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

#include "AddressTableSync.h"

#include "15_env/EnvModule.h"
#include "15_env/Address.h"
#include "15_env/Crossing.h"
#include "15_env/Road.h"
#include "15_env/CrossingTableSync.h"
#include "15_env/ConnectionPlaceTableSync.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"

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
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Address>::TABLE_NAME = "t002_addresses";
		template<> const int SQLiteTableSyncTemplate<Address>::TABLE_ID = 2;
		template<> const bool SQLiteTableSyncTemplate<Address>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Address>::load(Address* object, const db::SQLiteResultSPtr& rows )
		{
		    uid id (rows->getLongLong (TABLE_COL_ID));

		    object->setKey (id);
		    uid placeId = rows->getLongLong (AddressTableSync::COL_PLACEID);
			int tableId = decodeTableId(placeId);
			if (tableId == CrossingTableSync::TABLE_ID)
				object->setPlace(Crossing::Get(placeId).get());
			else if (tableId == ConnectionPlaceTableSync::TABLE_ID)
				object->setPlace(PublicTransportStopZoneConnectionPlace::Get(placeId).get());
			object->setRoad (Road::Get (rows->getLongLong (AddressTableSync::COL_ROADID)).get());
		    object->setMetricOffset (rows->getDouble (AddressTableSync::COL_METRICOFFSET));
		    object->setXY (rows->getDouble (AddressTableSync::COL_X), rows->getDouble (AddressTableSync::COL_Y));
		}


		template<> void SQLiteTableSyncTemplate<Address>::save(Address* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string AddressTableSync::COL_PLACEID ("place_id");  // NU
		const std::string AddressTableSync::COL_ROADID ("road_id");  // NU
		const std::string AddressTableSync::COL_METRICOFFSET ("metric_offset");  // U ??
		const std::string AddressTableSync::COL_X ("x");  // U ??
		const std::string AddressTableSync::COL_Y ("y");  // U ??

		AddressTableSync::AddressTableSync()
			: SQLiteTableSyncTemplate<Address>(true, false, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_PLACEID, "INTEGER", false);
			addTableColumn (COL_ROADID, "INTEGER", false);
			addTableColumn (COL_METRICOFFSET, "DOUBLE", false);
			addTableColumn (COL_X, "DOUBLE", true);
			addTableColumn (COL_Y, "DOUBLE", true);
		}

		AddressTableSync::~AddressTableSync()
		{

		}


		void AddressTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool)
		{
			while (rows->next ())
			{
				if (Address::Contains(rows->getLongLong (TABLE_COL_ID)))
				{
					load (Address::GetUpdateable(rows->getLongLong (TABLE_COL_ID)).get(), rows);
				}
				else
				{
					Address* object(new Address);
					load(object, rows);
					object->store();
					
					uid placeId = rows->getLongLong(COL_PLACEID);
					uid roadId(rows->getLongLong(COL_ROADID));

					shared_ptr<AddressablePlace> place = 
					    EnvModule::fetchUpdateableAddressablePlace (placeId);
					shared_ptr<Road> road = Road::GetUpdateable(roadId);
					    
					place->addAddress(object);
					road->addAddress(object);
				}
			}
		}
		
		void AddressTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (Address::Contains(id))
				{
					load(Address::GetUpdateable(id).get(), rows);
				}
			}
		}

		void AddressTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (Address::Contains(id))
				{
					Address::Remove(id);
				}
			}
		}

		std::vector<shared_ptr<Address> > AddressTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<Address> > objects;
				while (rows->next ())
				{
					shared_ptr<Address> object(new Address);
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
