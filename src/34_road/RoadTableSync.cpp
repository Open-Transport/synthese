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
#include "RoadPlaceTableSync.h"
#include "CityTableSync.h"
#include "RoadChunkTableSync.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ReplaceQuery.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, RoadTableSync>::FACTORY_KEY(
			"34.10.01 Roads"
		);
	}

	namespace road
	{
		const string RoadTableSync::COL_ROADTYPE ("road_type");
		const string RoadTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const string RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const string RoadTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const string RoadTableSync::COL_ROAD_PLACE_ID("road_place_id");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RoadTableSync>::TABLE(
			"t015_roads"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<RoadTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(RoadTableSync::COL_ROADTYPE, SQL_INTEGER),
			DBTableSync::Field(RoadTableSync::COL_BIKECOMPLIANCEID, SQL_INTEGER),
			DBTableSync::Field(RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID, SQL_INTEGER),
			DBTableSync::Field(RoadTableSync::COL_PEDESTRIANCOMPLIANCEID, SQL_INTEGER),
			DBTableSync::Field(RoadTableSync::COL_ROAD_PLACE_ID, SQL_INTEGER),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<RoadTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<RoadTableSync,MainRoadPart>::Load(
			MainRoadPart* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Type
			Road::RoadType roadType = (Road::RoadType) rows->getInt (RoadTableSync::COL_ROADTYPE);
			object->setType(roadType);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType roadPlaceId(rows->getLongLong(RoadTableSync::COL_ROAD_PLACE_ID));
				RoadPlace* roadPlace(RoadPlaceTableSync::GetEditable(roadPlaceId, env, linkLevel).get());
				object->setRoadPlace(*roadPlace);
				
// 				object->setBikeCompliance (BikeComplianceTableSync::Get (rows->getLongLong (RoadTableSync::COL_BIKECOMPLIANCEID), env, linkLevel));

// 				object->setHandicappedCompliance(HandicappedComplianceTableSync::Get(rows->getLongLong (RoadTableSync::COL_HANDICAPPEDCOMPLIANCEID), env, linkLevel));

// 				object->setPedestrianCompliance(PedestrianComplianceTableSync::Get(rows->getLongLong (RoadTableSync::COL_PEDESTRIANCOMPLIANCEID), env, linkLevel));
			}
		}



		template<> void DBDirectTableSyncTemplate<RoadTableSync,MainRoadPart>::Unlink(
			MainRoadPart* obj
		){
		}



		template<> void DBDirectTableSyncTemplate<RoadTableSync,MainRoadPart>::Save(
			MainRoadPart* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<RoadTableSync> query(*object);
			query.addField(static_cast<int>(object->getType()));
			query.addField(0);
			query.addField(0);
			query.addField(0);
			query.addField(object->getRoadPlace() ? object->getRoadPlace()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<RoadTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Control user rights
			return true;
		}



		template<> void DBTableSyncTemplate<RoadTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RoadTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		void DBTableSyncTemplate<RoadTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace road
	{
		RoadTableSync::SearchResult RoadTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> roadPlaceId,
			boost::optional<util::RegistryKeyType> cityId,
//			boost::optional<util::RegistryKeyType> startingNodeId,
//			boost::optional<util::RegistryKeyType> endingNodeId,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if(roadPlaceId)
			{
				query << " AND " << COL_ROAD_PLACE_ID << "=" << *roadPlaceId;
			} /// @todo implementation
/*			if(startingNodeId)
			{
				query << " AND (SELECT " << RoadChunkTableSync::COL_CROSSING_ID << " FROM " << RoadChunkTableSync::TABLE.NAME << " WHERE " << RoadChunkTableSync::COL_ROADID << "=" << TABLE.NAME << "." << TABLE_COL_ID << " AND " << RoadChunkTableSync::COL_RANKINPATH << "=0)=" << *startingNodeId;
			}
			if(endingNodeId)
			{
				query << " AND (SELECT " << RoadChunkTableSync::COL_CROSSING_ID << " FROM " << RoadChunkTableSync::TABLE.NAME << " WHERE " << RoadChunkTableSync::COL_ROADID << "=" << TABLE.NAME << "." << TABLE_COL_ID << " ORDER BY " << RoadChunkTableSync::COL_RANKINPATH << " DESC LIMIT 1)=" << *startingNodeId;
			}
*/			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
