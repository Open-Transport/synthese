
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
#include "PlaceAlias.h"
#include "15_env/EnvModule.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
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
		// template<> const std::string FactorableTemplate<SQLiteTableSync, PlaceAliasTableSync>::FACTORY_KEY("");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<PlaceAlias>::TABLE_NAME = "t011_place_aliases";
		template<> const int SQLiteTableSyncTemplate<PlaceAlias>::TABLE_ID = 11;
		template<> const bool SQLiteTableSyncTemplate<PlaceAlias>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<PlaceAlias>::load(PlaceAlias* object, const db::SQLiteResultSPtr& rows )
		{

			uid aliasedPlaceId (
				rows->getLongLong (PlaceAliasTableSync::COL_ALIASEDPLACEID));
			uid cityId (
				rows->getLongLong (PlaceAliasTableSync::COL_CITYID));

			object->setKey (rows->getLongLong (TABLE_COL_ID));
			object->setName (rows->getText (PlaceAliasTableSync::COL_NAME));
			object->setCity(EnvModule::getCities ().get (cityId).get());
			object->setAliasedPlace(EnvModule::fetchPlace (aliasedPlaceId).get());

		}

		template<> void SQLiteTableSyncTemplate<PlaceAlias>::save(PlaceAlias* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
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
			: SQLiteTableSyncTemplate<PlaceAlias>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_ALIASEDPLACEID, "INTEGER", false);
			addTableColumn (COL_CITYID, "INTEGER", false);
			addTableColumn (COL_ISCITYMAINCONNECTION, "BOOLEAN", false);
		}

		void PlaceAliasTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (EnvModule::getPlaceAliases().contains(id))
				{
					load(EnvModule::getPlaceAliases().getUpdateable(id).get(), rows);
				}
				else
				{
					shared_ptr<PlaceAlias> object(new PlaceAlias);
					load(object.get(), rows);
					EnvModule::getPlaceAliases().add(object);

					uid cityId (rows->getLongLong (COL_CITYID));
					
					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (cityId);

					bool isCityMainConnection (rows->getBool ( COL_ISCITYMAINCONNECTION));

					if (isCityMainConnection)
					{
						city->addIncludedPlace (object.get());
					}

					city->getPlaceAliasesMatcher ().add (object->getName (), object.get() );
				}
			}
		}
		
		void PlaceAliasTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (EnvModule::getPlaceAliases().contains(id))
				{
					shared_ptr<PlaceAlias> object = EnvModule::getPlaceAliases().getUpdateable(id);
					load(object.get(), rows);

					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (object->getCity ()->getKey ());
					city->getPlaceAliasesMatcher ().add (object->getName (), object.get());
					/// @todo Where is the removal of the old name ??
				}
			}
		}

		void PlaceAliasTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				uid id = rows->getLongLong (TABLE_COL_ID);
				if (EnvModule::getPlaceAliases().contains(id))
				{
					shared_ptr<const PlaceAlias> pa = EnvModule::getPlaceAliases ().get(id);
					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (pa->getCity ()->getKey ());
					city->getPlaceAliasesMatcher ().remove (pa->getName ());

					EnvModule::getPlaceAliases().remove(id);
				}
			}
		}

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
