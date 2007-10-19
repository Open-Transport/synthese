
/** PlaceAliasTableSync class implementation.
	@file PlaceAliasTableSync.cpp

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

#include "PlaceAliasTableSync.h"

#include "15_env/CityTableSync.h"
#include "15_env/EnvModule.h"

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

	namespace util
	{
		template<> const std::string FactorableTemplate<SQLiteTableSync, PlaceAliasTableSync>::FACTORY_KEY("15.50.01 Places Alias");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::TABLE_NAME = "t011_place_aliases";
		template<> const int SQLiteTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::TABLE_ID = 11;
		template<> const bool SQLiteTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::load(PlaceAlias* object, const db::SQLiteResultSPtr& rows )
		{
			object->setKey (rows->getLongLong (TABLE_COL_ID));
			object->setName (rows->getText (PlaceAliasTableSync::COL_NAME));
		}

		template<> void SQLiteTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::_link(PlaceAlias* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			// Because the fetch place has no equivalent in database read mode
			assert(temporary == GET_REGISTRY);

			uid aliasedPlaceId (
				rows->getLongLong (PlaceAliasTableSync::COL_ALIASEDPLACEID));
			uid cityId (
				rows->getLongLong (PlaceAliasTableSync::COL_CITYID));
			City* city(CityTableSync::GetUpdateable(cityId,obj,temporary));

			obj->setCity(city);
			obj->setAliasedPlace(EnvModule::fetchPlace (aliasedPlaceId).get());


			bool isCityMainConnection (rows->getBool ( PlaceAliasTableSync::COL_ISCITYMAINCONNECTION));
			if (isCityMainConnection)
			{
				city->addIncludedPlace (obj);
			}
			city->getPlaceAliasesMatcher ().add (obj->getName (), obj);
		}

		template<> void SQLiteTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::_unlink(PlaceAlias* obj)
		{
			City* city(const_cast<City*>(obj->getCity()));

			city->getPlaceAliasesMatcher ().remove (obj->getName ());
			
			obj->setCity(NULL);
		}

		template<> void SQLiteTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>::save(PlaceAlias* object)
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
		const std::string PlaceAliasTableSync::COL_NAME ("name");
		const std::string PlaceAliasTableSync::COL_ALIASEDPLACEID ("aliased_place_id");
		const std::string PlaceAliasTableSync::COL_CITYID ("city_id");
		const std::string PlaceAliasTableSync::COL_ISCITYMAINCONNECTION ("is_city_main_connection");

		PlaceAliasTableSync::PlaceAliasTableSync()
			: SQLiteRegistryTableSyncTemplate<PlaceAliasTableSync,PlaceAlias>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_ALIASEDPLACEID, "INTEGER", false);
			addTableColumn (COL_CITYID, "INTEGER", false);
			addTableColumn (COL_ISCITYMAINCONNECTION, "BOOLEAN", false);
		}

/*		void PlaceAliasTableSync::rowsAdded(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (PlaceAlias::Contains(id))
				{
					load(PlaceAlias::GetUpdateable(id).get(), rows);
				}
				else
				{
					PlaceAlias* object(new PlaceAlias);
					load(object, rows);
					object->store();

					uid cityId (rows->getLongLong (COL_CITYID));
					
					shared_ptr<City> city = City::GetUpdateable (cityId);

					bool isCityMainConnection (rows->getBool ( COL_ISCITYMAINCONNECTION));

					if (isCityMainConnection)
					{
						city->addIncludedPlace (object);
					}

					city->getPlaceAliasesMatcher ().add (object->getName (), object);
				}
			}
		}
		
		void PlaceAliasTableSync::rowsUpdated(db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (PlaceAlias::Contains(id))
				{
					shared_ptr<PlaceAlias> object = PlaceAlias::GetUpdateable(id);
					load(object.get(), rows);

					shared_ptr<City> city = City::GetUpdateable (object->getCity ()->getKey ());
					city->getPlaceAliasesMatcher ().add (object->getName (), object.get());
					/// @todo Where is the removal of the old name ??
				}
			}
		}

		void PlaceAliasTableSync::rowsRemoved( db::SQLite* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (PlaceAlias::Contains(id))
				{
					shared_ptr<const PlaceAlias> pa = PlaceAlias::Get(id);
					shared_ptr<City> city = City::GetUpdateable (pa->getCity ()->getKey ());
					city->getPlaceAliasesMatcher ().remove (pa->getName ());

					PlaceAlias::Remove(id);
				}
			}
		}
*/
		vector<shared_ptr<PlaceAlias> > PlaceAliasTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
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

			try
			{
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<PlaceAlias> > objects;
				while (rows->next ())
				{
					shared_ptr<PlaceAlias> object(new PlaceAlias);
					load (object.get(), rows);
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
