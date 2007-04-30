
/** RoadTableSync class implementation.
	@file RoadTableSync.cpp

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

#include "RoadTableSync.h"
#include "Road.h"
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
		// template<> const std::string FactorableTemplate<SQLiteTableSync, RoadTableSync>::FACTORY_KEY("");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Road>::TABLE_NAME = "t015_roads";
		template<> const int SQLiteTableSyncTemplate<Road>::TABLE_ID = 15;
		template<> const bool SQLiteTableSyncTemplate<Road>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Road>::load(Road* object, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			// ID
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));

			// Name
			std::string name (
				rows.getColumn (rowIndex, RoadTableSync::COL_NAME));
			object->setName(name);

			// Type
			Road::RoadType roadType = (Road::RoadType)
				Conversion::ToInt (rows.getColumn (rowIndex, RoadTableSync::COL_ROADTYPE));
			object->setType(roadType);

			// City
			uid cityId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, RoadTableSync::COL_CITYID)));
			object->setCity(EnvModule::getCities ().get(cityId).get());

			// Fare
			uid fareId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, RoadTableSync::COL_FAREID)));
			object->setFare(EnvModule::getFares ().get (fareId).get());


			uid bikeComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, RoadTableSync::COL_BIKECOMPLIANCEID)));
			object->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId).get());

			uid handicappedComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID)));
			object->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId).get());

			uid pedestrianComplianceId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, RoadTableSync::COL_PEDESTRIANCOMPLIANCEID)));
			object->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId).get());

			uid reservationRuleId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, RoadTableSync::COL_RESERVATIONRULEID)));
			object->setReservationRule (EnvModule::getReservationRules ().get (reservationRuleId).get()); 


		}

		template<> void SQLiteTableSyncTemplate<Road>::save(Road* object)
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
		const string RoadTableSync::COL_NAME ("name");
		const string RoadTableSync::COL_CITYID ("city_id");
		const string RoadTableSync::COL_ROADTYPE ("road_type");
		const string RoadTableSync::COL_FAREID ("fare_id");
		const string RoadTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const string RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const string RoadTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const string RoadTableSync::COL_RESERVATIONRULEID ("reservation_rule_id");
		const string RoadTableSync::COL_VIAPOINTS ("via_points");

		RoadTableSync::RoadTableSync()
			: SQLiteTableSyncTemplate<Road>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_CITYID, "INTEGER", false);
			addTableColumn (COL_ROADTYPE, "INTEGER", true);
			addTableColumn (COL_FAREID, "INTEGER", true);
			addTableColumn (COL_BIKECOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_RESERVATIONRULEID, "INTEGER", true);
			addTableColumn (COL_VIAPOINTS, "TEXT", true);
		}

		void RoadTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isItFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getRoads().contains(id))
				{
					shared_ptr<const Road> road = EnvModule::getRoads().get(id);
					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (road->getCity ()->getKey ());
					city->getRoadsMatcher ().remove (road->getName ());

					load(EnvModule::getRoads().getUpdateable(id).get(), rows, i);

					city->getRoadsMatcher ().add (road->getName (), road.get());
				}
				else
				{
					shared_ptr<Road> object(new Road);
					load(object.get(), rows, i);
					EnvModule::getRoads().add(object);

					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (object->getCity ()->getKey ());
					city->getRoadsMatcher ().add (object->getName (), object.get());
				}
			}
		}
		
		void RoadTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getRoads().contains(id))
				{
					shared_ptr<const Road> road = EnvModule::getRoads().get(id);
					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (road->getCity ()->getKey ());
					city->getRoadsMatcher ().remove (road->getName ());

					load(EnvModule::getRoads().getUpdateable(id).get(), rows, i);

					city->getRoadsMatcher ().add (road->getName (), road.get());
				}
			}
		}

		void RoadTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getRoads().contains(id))
				{
					shared_ptr<const Road> road = EnvModule::getRoads().get(id);
					shared_ptr<City> city = EnvModule::getCities ().getUpdateable (road->getCity ()->getKey ());
					city->getRoadsMatcher ().remove (road->getName ());

					EnvModule::getRoads().remove(id);
				}
			}
		}

		vector<shared_ptr<Road> > RoadTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				vector<shared_ptr<Road> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<Road> object(new Road);
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
