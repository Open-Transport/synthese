

/** PublicPlaceTableSync class implementation.
	@file PublicPlaceTableSync.cpp

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

#include "15_env/PublicPlaceTableSync.h"
#include "15_env/PublicPlace.h"
#include "15_env/EnvModule.h"
#include "15_env/City.h"

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
		// template<> const std::string FactorableTemplate<SQLiteTableSync, PublicPlaceTableSync>::FACTORY_KEY("");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<PublicPlace>::TABLE_NAME = "t013_public_places";
		template<> const int SQLiteTableSyncTemplate<PublicPlace>::TABLE_ID = 13;
		template<> const bool SQLiteTableSyncTemplate<PublicPlace>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<PublicPlace>::load(PublicPlace* object, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));
			std::string name (
				rows.getColumn (rowIndex, PublicPlaceTableSync::COL_NAME));
			uid cityId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, PublicPlaceTableSync::COL_CITYID)));

			shared_ptr<const City> city = EnvModule::getCities ().get(cityId);

			object->setName(name);
			object->setCity(city.get());
		}

		template<> void SQLiteTableSyncTemplate<PublicPlace>::save(PublicPlace* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId(1,1));	/// @todo Use grid ID
               
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
		const std::string PublicPlaceTableSync::COL_NAME ("name");
		const std::string PublicPlaceTableSync::COL_CITYID ("city_id");

		PublicPlaceTableSync::PublicPlaceTableSync()
			: SQLiteTableSyncTemplate<PublicPlace>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_CITYID, "INTEGER", false);
		}

		void PublicPlaceTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getPublicPlaces().contains(id))
				{
					shared_ptr<PublicPlace> pp = EnvModule::getPublicPlaces ().getUpdateable (id);
					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (pp->getCity ()->getKey ());
					city->getPublicPlacesMatcher ().remove (pp->getName ());

					load(EnvModule::getPublicPlaces().getUpdateable(id).get(), rows, i);

					city->getPublicPlacesMatcher ().add (pp->getName (), pp.get());
				}
				else
				{
					shared_ptr<PublicPlace> object(new PublicPlace);
					load(object.get(), rows, i);
					EnvModule::getPublicPlaces().add(object);
				}
			}
		}
		
		void PublicPlaceTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getPublicPlaces().contains(id))
				{
					shared_ptr<PublicPlace> pp = EnvModule::getPublicPlaces ().getUpdateable (id);
					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (pp->getCity ()->getKey ());
					city->getPublicPlacesMatcher ().remove (pp->getName ());

					load(EnvModule::getPublicPlaces().getUpdateable(id).get(), rows, i);

					city->getPublicPlacesMatcher ().add (pp->getName (), pp.get());
				}
			}
		}

		void PublicPlaceTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getPublicPlaces().contains(id))
				{
					shared_ptr<const PublicPlace> pp = EnvModule::getPublicPlaces ().get (id);
					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (pp->getCity ()->getKey ());
					city->getPublicPlacesMatcher ().remove (pp->getName ());

					EnvModule::getPublicPlaces().remove(id);
				}
			}
		}

		vector<shared_ptr<PublicPlace> > PublicPlaceTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				SQLiteResult result = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<PublicPlace> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<PublicPlace> object(new PublicPlace);
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
