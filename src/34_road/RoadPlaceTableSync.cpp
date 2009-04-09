////////////////////////////////////////////////////////////////////////////////
///	RoadPlace Table Synchronizer class implementation.
///	@file RoadPlaceTableSync.cpp
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "RoadPlaceTableSync.h"
#include "RoadPlace.h"
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
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,RoadPlaceTableSync>::FACTORY_KEY("34.01 RoadPlace");
	}

	namespace road
	{
		const string RoadPlaceTableSync::COL_NAME("name");
		const string RoadPlaceTableSync::COL_CITYID("city_id");
	}

	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<RoadPlaceTableSync>::TABLE(
			"t060_road_places"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<RoadPlaceTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadPlaceTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(RoadPlaceTableSync::COL_CITYID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<RoadPlaceTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				RoadPlaceTableSync::COL_CITYID.c_str()
				, ""),
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<RoadPlaceTableSync,RoadPlace>::Load(
			RoadPlace* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setName(rows->getText(RoadPlaceTableSync::COL_NAME));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType cityId(rows->getLongLong(RoadPlaceTableSync::COL_CITYID));
				
				// City
				object->setCity(CityTableSync::Get(cityId, env, linkLevel).get());
				City* city(CityTableSync::GetEditable(cityId, env, linkLevel).get());
				city->getRoadsMatcher ().add (object->getName (), object);
				city->getAllPlacesMatcher().add(
					object->getName() + " [voie]",
					static_cast<const Place*>(object)
				);
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<RoadPlaceTableSync,RoadPlace>::Save(
			RoadPlace* object
		){
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< "," << (
					object->getCity() ?
					Conversion::ToString(object->getCity()->getKey()) :
					"0"
				)<< ")";
			
			DBModule::GetSQLite()->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<RoadPlaceTableSync,RoadPlace>::Unlink(
			RoadPlace* obj
		){
			City* city = const_cast<City*>(obj->getCity ());
			if (city != NULL)
			{
				city->getRoadsMatcher ().remove (obj->getName ());
				city->getAllPlacesMatcher().remove(obj->getName() + " [voie]");
				obj->setCity(NULL);
			}
		}
	}
	
	
	
	namespace road
	{
		RoadPlaceTableSync::RoadPlaceTableSync()
			: SQLiteRegistryTableSyncTemplate<RoadPlaceTableSync,RoadPlace>()
		{
		}



		void RoadPlaceTableSync::Search(
			Env& env,
			string name,
			int first, /*= 0*/
			int number, /*= 0*/
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			// @todo Fill Where criteria
			if (!name.empty())
			 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}

 
