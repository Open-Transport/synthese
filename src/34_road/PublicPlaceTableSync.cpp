

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

#include "PublicPlaceTableSync.h"
#include "PublicPlace.h"
#include "City.h"
#include "CityTableSync.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, PublicPlaceTableSync>::FACTORY_KEY("34.40.03 Public places");
		template<> const string FetcherTemplate<NamedPlace, PublicPlaceTableSync>::FACTORY_KEY("13");
	}

	namespace road
	{
		const string PublicPlaceTableSync::COL_NAME ("name");
		const string PublicPlaceTableSync::COL_CITYID ("city_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<PublicPlaceTableSync>::TABLE(
			"t013_public_places"
			);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<PublicPlaceTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(PublicPlaceTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(PublicPlaceTableSync::COL_CITYID, SQL_INTEGER, false),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<PublicPlaceTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Load(
			PublicPlace* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			string name (rows->getText (PublicPlaceTableSync::COL_NAME));
			object->setName(name);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				uid cityId (rows->getLongLong (PublicPlaceTableSync::COL_CITYID));
				City* city(CityTableSync::GetEditable(cityId, env, linkLevel).get());
				object->setCity(city);

				city->addPlaceToMatcher<PublicPlace>(object);
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Unlink(
			PublicPlace* obj
		){
			City* city(const_cast<City*>(obj->getCity()));
			if (city != NULL)
			{
				city->removePlaceFromMatcher<PublicPlace>(obj);
				obj->setCity(NULL);
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Save (PublicPlace* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES(" <<
				object->getKey() << "," <<
				Conversion::ToSQLiteString(object->getName()) << "," <<
				(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0)) <<
			")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace road
	{
		PublicPlaceTableSync::PublicPlaceTableSync()
			: SQLiteRegistryTableSyncTemplate<PublicPlaceTableSync,PublicPlace>()
		{
		}



		void PublicPlaceTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
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

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
