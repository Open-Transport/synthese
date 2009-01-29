

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

#include "EnvModule.h"
#include "RoadTableSync.h"
#include "CrossingTableSync.h"
#include "ConnectionPlaceTableSync.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "LinkException.h"

#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,AddressTableSync>::FACTORY_KEY("15.50.02 Addresses");
	}

	namespace env
	{
		const std::string AddressTableSync::COL_PLACEID ("place_id");  // NU
		const std::string AddressTableSync::COL_ROADID ("road_id");  // NU
		const std::string AddressTableSync::COL_METRICOFFSET ("metric_offset");  // U ??
		const std::string AddressTableSync::COL_X ("x");  // U ??
		const std::string AddressTableSync::COL_Y ("y");  // U ??
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<AddressTableSync>::TABLE(
			"t002_addresses"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<AddressTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(AddressTableSync::COL_PLACEID, SQL_INTEGER, false),
			SQLiteTableSync::Field(AddressTableSync::COL_ROADID, SQL_INTEGER, false),
			SQLiteTableSync::Field(AddressTableSync::COL_METRICOFFSET, SQL_DOUBLE, false),
			SQLiteTableSync::Field(AddressTableSync::COL_X, SQL_DOUBLE),
			SQLiteTableSync::Field(AddressTableSync::COL_Y, SQL_DOUBLE),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<AddressTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<AddressTableSync,Address>::Load(
			Address* object
			, const db::SQLiteResultSPtr& rows
			, Env& env
			, LinkLevel linkLevel
		){
			// Properties
		    object->setMetricOffset (rows->getDouble (AddressTableSync::COL_METRICOFFSET));
		    object->setXY (rows->getDouble (AddressTableSync::COL_X), rows->getDouble (AddressTableSync::COL_Y));
		
			if (linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Columns reading
				uid placeId = rows->getLongLong (AddressTableSync::COL_PLACEID);
				int tableId = decodeTableId(placeId);
				uid roadId(rows->getLongLong(AddressTableSync::COL_ROADID));

				// Links from the object
				try
				{
					if (tableId == CrossingTableSync::TABLE.ID)
						object->setPlace(CrossingTableSync::Get(placeId, env, linkLevel).get());
					else if (tableId == ConnectionPlaceTableSync::TABLE.ID)
						object->setPlace(ConnectionPlaceTableSync::Get(placeId, env, linkLevel).get());

					object->setRoad (RoadTableSync::Get (roadId, env, linkLevel).get());

					// Links to the object
					shared_ptr<AddressablePlace> place(EnvModule::FetchEditableAddressablePlace(placeId, env));
					shared_ptr<Road> road(RoadTableSync::GetEditable(roadId, env, linkLevel));

					place->addAddress(object);
					road->addAddress(object);
				}
				catch (ObjectNotFoundException<Crossing>& e)
				{
					throw LinkException<AddressTableSync>(object->getKey(), "Crossing ("+ AddressTableSync::COL_PLACEID +")", e);
				}
				catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
				{
					throw LinkException<AddressTableSync>(object->getKey(), "Connection place ("+ AddressTableSync::COL_PLACEID +")", e);
				}
				catch (ObjectNotFoundException<Road>& e)
				{
					throw LinkException<AddressTableSync>(object->getKey(), AddressTableSync::COL_ROADID, e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<AddressTableSync,Address>::Unlink(
			Address* obj
		){
			AddressablePlace* place(
				const_cast<AddressablePlace*>(
					static_cast<const AddressablePlace*>(
						obj->getPlace()
			)	)	);
			if (place != NULL)
			{
//				place->removeAddress(obj);
			}

			Road* road(const_cast<Road*>(obj->getRoad()));
			if (road != NULL)
			{
//				road->removeAddress(obj);
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<AddressTableSync,Address>::Save(
			Address* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		AddressTableSync::AddressTableSync()
			: SQLiteRegistryTableSyncTemplate<AddressTableSync,Address>()
		{
		}

		AddressTableSync::~AddressTableSync()
		{

		}

		void AddressTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
