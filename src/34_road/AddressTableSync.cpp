

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

#include "PublicPlace.h"
#include "PublicPlaceTableSync.h"
#include "RoadTableSync.h"
#include "ConnectionPlaceTableSync.h"
#include "Crossing.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "LinkException.h"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace road;
	using namespace impex;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,AddressTableSync>::FACTORY_KEY("34.20.01 Addresses");
		template<> const string FactorableTemplate<Fetcher<Vertex>, AddressTableSync>::FACTORY_KEY("2");
	}

	namespace road
	{
		const std::string AddressTableSync::COL_PLACEID ("place_id");  // NU
		const std::string AddressTableSync::COL_X ("x");  // U ??
		const std::string AddressTableSync::COL_Y ("y");  // U ??
		const std::string AddressTableSync::COL_CODE_BY_SOURCE ("code_by_source");  // U ??
		const std::string AddressTableSync::COL_SOURCE_ID ("source_id");  // U ??
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
			SQLiteTableSync::Field(AddressTableSync::COL_X, SQL_DOUBLE),
			SQLiteTableSync::Field(AddressTableSync::COL_Y, SQL_DOUBLE),
			SQLiteTableSync::Field(AddressTableSync::COL_CODE_BY_SOURCE, SQL_TEXT),
			SQLiteTableSync::Field(AddressTableSync::COL_SOURCE_ID, SQL_INTEGER),
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
		    object->setXY (rows->getDouble (AddressTableSync::COL_X), rows->getDouble (AddressTableSync::COL_Y));
			object->setCodeBySource(rows->getText(AddressTableSync::COL_CODE_BY_SOURCE));
		
			if (linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Columns reading
				RegistryKeyType placeId = rows->getLongLong (AddressTableSync::COL_PLACEID);
				RegistryKeyType sourceId = rows->getLongLong (AddressTableSync::COL_SOURCE_ID);
				RegistryTableType tableId = decodeTableId(placeId);

				// Links from the object
				try
				{
					if(tableId == ConnectionPlaceTableSync::TABLE.ID)
					{
						object->setHub(ConnectionPlaceTableSync::Get(placeId, env, linkLevel).get());
						
						// Links to the object
						PublicTransportStopZoneConnectionPlace* place(ConnectionPlaceTableSync::GetEditable(placeId, env, linkLevel).get());
				
						place->addAddress(object);
					}
					else if(tableId == PublicPlaceTableSync::TABLE.ID)
					{
						object->setHub(PublicPlaceTableSync::Get(placeId, env, linkLevel).get());

						// Links to the object
						PublicPlace* place(PublicPlaceTableSync::GetEditable(placeId, env, linkLevel).get());

						place->addAddress(object);
					}
					else
					{
						RegistryKeyType crossingId(
							util::encodeUId(43,0,decodeObjectId(object->getKey()))
						);
						shared_ptr<Crossing> crossing(new Crossing(crossingId));
						crossing->setAddress(object);
						env.getEditableRegistry<Crossing>().add(crossing);
						object->setHub(crossing.get());
					}
					if(sourceId > 0)
					{
						object->setDataSource(DataSourceTableSync::Get(sourceId, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
				{
					throw LinkException<AddressTableSync>(rows, AddressTableSync::COL_PLACEID, e);
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<AddressTableSync,Address>::Unlink(
			Address* obj
		){
			AddressablePlace* place(
				const_cast<AddressablePlace*>(
					static_cast<const AddressablePlace*>(
						obj->getHub()
			)	)	);
			if (place != NULL)
			{
//				place->removeAddress(obj);
			}

//			Road* road(const_cast<Road*>(obj->getRoad()));
//			if (road != NULL)
			{
//				road->removeAddress(obj);
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<AddressTableSync,Address>::Save(
			Address* object,
			optional<SQLiteTransaction&> transaction
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query <<
				" REPLACE INTO " << TABLE.NAME << " VALUES(" <<
				object->getKey() << "," <<
				(	dynamic_cast<const Registrable*>(object->getHub()) && !dynamic_cast<const Crossing*>(object->getHub()) ?
					lexical_cast<string>(dynamic_cast<const Registrable*>(object->getHub())->getKey()) :
					"0"
				) << "," << fixed <<
				object->getX() << "," <<
				object->getY() << "," <<
				Conversion::ToSQLiteString(object->getCodeBySource()) << "," <<
				(object->getDataSource() ? lexical_cast<string>(object->getDataSource()->getKey()) : "0") <<
			")";
			sqlite->execUpdate(query.str(), transaction);
		}

	}

	namespace road
	{
		AddressTableSync::AddressTableSync()
			: SQLiteRegistryTableSyncTemplate<AddressTableSync,Address>()
		{
		}

		AddressTableSync::~AddressTableSync()
		{

		}

		AddressTableSync::SearchResult AddressTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> placeId /*= boost::optional<util::RegistryKeyType>()*/,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if(placeId)
			{
				query << " AND " << COL_PLACEID << "=" << *placeId;
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
				{
					query << " OFFSET " << first;
				}
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
