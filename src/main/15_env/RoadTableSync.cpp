
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

#include "15_env/CityTableSync.h"
#include "15_env/FareTableSync.h"
#include "15_env/BikeComplianceTableSync.h"
#include "15_env/HandicappedComplianceTableSync.h"
#include "15_env/PedestrianComplianceTableSync.h"
#include "15_env/ReservationRuleTableSync.h"

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
		template<> const std::string FactorableTemplate<SQLiteTableSync, RoadTableSync>::FACTORY_KEY("15.30.02 Roads");
	}
	
	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<RoadTableSync,Road>::TABLE_NAME = "t015_roads";
		template<> const int SQLiteTableSyncTemplate<RoadTableSync,Road>::TABLE_ID = 15;
		template<> const bool SQLiteTableSyncTemplate<RoadTableSync,Road>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<RoadTableSync,Road>::load(Road* object, const db::SQLiteResultSPtr& rows )
		{
			// ID
			object->setKey(rows->getLongLong (TABLE_COL_ID));

			// Name
			std::string name (rows->getText (RoadTableSync::COL_NAME));
			object->setName(name);

			// Type
			Road::RoadType roadType = (Road::RoadType) rows->getInt (RoadTableSync::COL_ROADTYPE);
			object->setType(roadType);

		}

		template<> void SQLiteTableSyncTemplate<RoadTableSync,Road>::_link(Road* object, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			// City
			uid cityId (rows->getLongLong (RoadTableSync::COL_CITYID));
			object->setCity(CityTableSync::Get(cityId, object, true, temporary));

			// Fare
			uid fareId (rows->getLongLong (RoadTableSync::COL_FAREID));
			object->setFare(FareTableSync::Get (fareId, object, true, temporary));

			uid bikeComplianceId (rows->getLongLong (RoadTableSync::COL_BIKECOMPLIANCEID));
			object->setBikeCompliance (BikeComplianceTableSync::Get (bikeComplianceId, object, true, temporary));

			uid handicappedComplianceId (rows->getLongLong (RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID));
			object->setHandicappedCompliance (HandicappedComplianceTableSync::Get (handicappedComplianceId, object, true, temporary));

			uid pedestrianComplianceId (rows->getLongLong (RoadTableSync::COL_PEDESTRIANCOMPLIANCEID));
			object->setPedestrianCompliance (PedestrianComplianceTableSync::Get (pedestrianComplianceId, object, true, temporary));

			uid reservationRuleId (rows->getLongLong (RoadTableSync::COL_RESERVATIONRULEID));
			object->setReservationRule (ReservationRuleTableSync::Get (reservationRuleId, object, true, temporary));


			// Links to the loaded object
			if (temporary == GET_REGISTRY)
			{
				City* city(City::GetUpdateable(cityId).get());
				city->getRoadsMatcher ().add (object->getName (), object);
				city->getAllPlacesMatcher().add(object->getName() + " [voie]", static_cast<const Place*>(object));
			}

		}

		template<> void SQLiteTableSyncTemplate<RoadTableSync,Road>::_unlink(Road* obj)
		{
			City* city = const_cast<City*>(obj->getCity ());
			city->getRoadsMatcher ().remove (obj->getName ());
			city->getAllPlacesMatcher().remove(obj->getName() + " [voie]");

			obj->setCity(NULL);
			obj->setFare(NULL);
			obj->setBikeCompliance(NULL);
			obj->setHandicappedCompliance(NULL);
			obj->setPedestrianCompliance(NULL);
			obj->setReservationRule(NULL);
		}

		template<> void SQLiteTableSyncTemplate<RoadTableSync,Road>::save(Road* object)
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
			: SQLiteRegistryTableSyncTemplate<RoadTableSync,Road>()
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
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				vector<shared_ptr<Road> > objects;
				while (rows->next ())
				{
					shared_ptr<Road> object(new Road);
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
