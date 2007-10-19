

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
#include "15_env/RoadTableSync.h"
#include "15_env/CrossingTableSync.h"
#include "15_env/ConnectionPlaceTableSync.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"
#include "02_db/LinkException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,AddressTableSync>::FACTORY_KEY("15.50.02 Addresses");
	}

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<AddressTableSync,Address>::TABLE_NAME = "t002_addresses";
		template<> const int SQLiteTableSyncTemplate<AddressTableSync,Address>::TABLE_ID = 2;
		template<> const bool SQLiteTableSyncTemplate<AddressTableSync,Address>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<AddressTableSync,Address>::load(Address* object, const db::SQLiteResultSPtr& rows)
		{
			// Columns reading
		    uid id (rows->getLongLong (TABLE_COL_ID));

			// Properties
		    object->setKey (id);
		    object->setMetricOffset (rows->getDouble (AddressTableSync::COL_METRICOFFSET));
		    object->setXY (rows->getDouble (AddressTableSync::COL_X), rows->getDouble (AddressTableSync::COL_Y));
		}


		template<> void SQLiteTableSyncTemplate<AddressTableSync,Address>::_link(Address* obj, const db::SQLiteResultSPtr& rows, GetSource temporary)
		{
			// Columns reading
			uid placeId = rows->getLongLong (AddressTableSync::COL_PLACEID);
			int tableId = decodeTableId(placeId);
			uid roadId(rows->getLongLong(AddressTableSync::COL_ROADID));

			// Links from the object
			try
			{
				if (tableId == CrossingTableSync::TABLE_ID)
					obj->setPlace(CrossingTableSync::Get(placeId, obj, true, temporary));
				else if (tableId == ConnectionPlaceTableSync::TABLE_ID)
					obj->setPlace(ConnectionPlaceTableSync::Get(placeId, obj, true, temporary));

				obj->setRoad (RoadTableSync::Get (roadId, obj, true, temporary));

				// Links to the object
				shared_ptr<AddressablePlace> place = 
					EnvModule::fetchUpdateableAddressablePlace (placeId);
				shared_ptr<Road> road = Road::GetUpdateable(roadId);

				place->addAddress(obj);
				road->addAddress(obj);
			}
			catch (Crossing::ObjectNotFoundException& e)
			{
				throw LinkException<AddressTableSync>(obj->getKey(), "Crossing ("+ AddressTableSync::COL_PLACEID +")", e);
			}
			catch (PublicTransportStopZoneConnectionPlace::ObjectNotFoundException& e)
			{
				throw LinkException<AddressTableSync>(obj->getKey(), "Connection place ("+ AddressTableSync::COL_PLACEID +")", e);
			}
			catch (Road::ObjectNotFoundException& e)
			{
				throw LinkException<AddressTableSync>(obj->getKey(), AddressTableSync::COL_ROADID, e);
			}
		}



		template<> void SQLiteTableSyncTemplate<AddressTableSync,Address>::_unlink(Address* obj)
		{
			shared_ptr<AddressablePlace> place(EnvModule::fetchUpdateableAddressablePlace(obj->getPlace()->getId()));
//			place->removeAddress(obj);

			shared_ptr<Road> road(Road::GetUpdateable(obj->getRoad()->getKey()));
//			road->removeAddress(obj);
		}


		template<> void SQLiteTableSyncTemplate<AddressTableSync,Address>::save(Address* object)
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
			: SQLiteRegistryTableSyncTemplate<AddressTableSync,Address>()
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

/*
		void AddressTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool)
		{
			while (rows->next ())
			{
				if (Address::Contains(rows->getLongLong (TABLE_COL_ID)))
				{
					shared_ptr<Address> address(Address::GetUpdateable(rows->getLongLong (TABLE_COL_ID)));
					unlink(address.get());
					load (address.get(), rows);
					link(address.get(), rows);
				}
				else
				{
					Address* object(new Address);
					load(object, rows);
					link(object, rows);
					object->store();
					
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
					shared_ptr<Address> address(Address::GetUpdateable(id));
					unlink(address.get());
					load (address.get(), rows);
					link(address.get(), rows);
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
					unlink(Address::Get(id));
					Address::Remove(id);
				}
			}
		}
*/
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
