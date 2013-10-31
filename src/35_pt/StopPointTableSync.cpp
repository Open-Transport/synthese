/** StopPointTableSync class implementation.
	@file StopPointTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "StopPointTableSync.hpp"

#include "CityTableSync.h"
#include "CoordinatesSystem.hpp"
#include "CrossingTableSync.hpp"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "LinkException.h"
#include "Profile.h"
#include "PTUseRuleTableSync.h"
#include "ReplaceQuery.h"
#include "RoadChunkTableSync.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "StopAreaTableSync.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace geography;
	using namespace road;
	using namespace impex;
	using namespace security;
	using namespace graph;

	template<> const string util::FactorableTemplate<DBTableSync,StopPointTableSync>::FACTORY_KEY("35.55.01 Physical stops");
	template<> const string FactorableTemplate<Fetcher<graph::Vertex>, StopPointTableSync>::FACTORY_KEY("12");

	namespace pt
	{
		const string StopPointTableSync::COL_NAME = "name";
		const string StopPointTableSync::COL_PLACEID = "place_id";
		const string StopPointTableSync::COL_X = "x";
		const string StopPointTableSync::COL_Y = "y";
		const string StopPointTableSync::COL_OPERATOR_CODE = "operator_code";
		const string StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID = "projected_road_chunk_id";
		const string StopPointTableSync::COL_PROJECTED_METRIC_OFFSET = "projected_metric_offset";
		const string StopPointTableSync::COL_HANDICAPPED_COMPLIANCE_ID = "handicapped_compliance_id";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<StopPointTableSync>::TABLE(
			"t012_physical_stops"
		);

		template<> const Field DBTableSyncTemplate<StopPointTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(StopPointTableSync::COL_NAME, SQL_TEXT),
			Field(StopPointTableSync::COL_PLACEID, SQL_INTEGER),
			Field(StopPointTableSync::COL_X, SQL_DOUBLE),
			Field(StopPointTableSync::COL_Y, SQL_DOUBLE),
			Field(StopPointTableSync::COL_OPERATOR_CODE, SQL_TEXT),
			Field(StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID, SQL_INTEGER),
			Field(StopPointTableSync::COL_PROJECTED_METRIC_OFFSET, SQL_DOUBLE),
			Field(StopPointTableSync::COL_HANDICAPPED_COMPLIANCE_ID, SQL_INTEGER),
			Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<StopPointTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(StopPointTableSync::COL_PLACEID.c_str(), ""));
			r.push_back(DBTableSync::Index(StopPointTableSync::COL_OPERATOR_CODE.c_str(), ""));
			return r;
		}


		/** Does not update the place */
		template<>
		void OldLoadSavePolicy<StopPointTableSync,StopPoint>::Load(
			StopPoint* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(object->getLinkedObjectsIds(*rows), env, linkLevel);
			object->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				object->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void OldLoadSavePolicy<StopPointTableSync,StopPoint>::Unlink(
			StopPoint* obj
		){
			// Hub
			StopArea* place = const_cast<StopArea*>(obj->getConnectionPlace());
			place->removePhysicalStop(*obj);
			obj->setHub(NULL);

			// Handicapped compliance
			obj->setRules(RuleUser::GetEmptyRules());

			// Projected point
			if(obj->getProjectedPoint().getRoadChunk())
			{
				obj->getProjectedPoint().getRoadChunk()->getFromCrossing()->removeReachableVertex(obj);
			}
		}



		template<> void OldLoadSavePolicy<StopPointTableSync,StopPoint>::Save(
			StopPoint* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<StopPointTableSync> query(*object);

			// Name
			query.addField(object->getName());

			// Stop area
			query.addField(dynamic_cast<const StopArea*>(object->getHub()) ? dynamic_cast<const StopArea*>(object->getHub())->getKey() : RegistryKeyType(0));

			// X Y (deprecated)
			if(object->hasGeometry())
			{
				query.addField(object->getGeometry()->getX());
				query.addField(object->getGeometry()->getY());
			}
			else
			{
				query.addFieldNull();
				query.addFieldNull();
			}

			// Data source links
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);

			// Projected point
			if(object->getProjectedPoint().getRoadChunk())
			{
				query.addField(object->getProjectedPoint().getRoadChunk()->getKey());
				query.addField(object->getProjectedPoint().getMetricOffset());
			}
			else
			{
				query.addFieldNull();
				query.addFieldNull();
			}

			// Handicapped compliance
			query.addField(
				object->getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED))->getKey() : RegistryKeyType(0)
			);

			// Geometry
			if(object->hasGeometry())
			{
				query.addField(static_pointer_cast<Geometry,Point>(object->getGeometry()));
			}
			else
			{
				query.addFieldNull();
			}

			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<StopPointTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<StopPointTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<StopPointTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			try
			{
				boost::shared_ptr<const StopPoint> stopPoint(StopPointTableSync::Get(id, env));
				StopArea::TransferDelaysMap tdMap(stopPoint->getConnectionPlace()->getTransferDelays());
				StopArea::TransferDelaysMap value(stopPoint->getConnectionPlace()->getTransferDelays());
				BOOST_FOREACH(const StopArea::TransferDelaysMap::value_type& td, tdMap)
				{
					if(td.first.first == id || td.first.second == id)
					{
						value.erase(
							make_pair(
								td.first.first,
								td.first.second
						)	);
					}
				}
				if(value != tdMap)
				{
					const_cast<StopArea*>(stopPoint->getConnectionPlace())->setTransferDelaysMatrix(value);
					StopAreaTableSync::Save(const_cast<StopArea*>(stopPoint->getConnectionPlace()), transaction);
				}
			}
			catch (...)
			{}
		}



		template<> void DBTableSyncTemplate<StopPointTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		StopPointTableSync::SearchResult StopPointTableSync::Search(
			Env& env,
			optional<RegistryKeyType> placeId,
			optional<string> operatorCode,
			bool orderByCityAndStopName,
			bool raisingOrder,
			int first /*= 0 */,
			boost::optional<std::size_t> number  /*= 0 */,
			LinkLevel linkLevel
		){
			SelectQuery<StopPointTableSync> query;
			if(operatorCode)
			{
				query.addWhereField(COL_OPERATOR_CODE, *operatorCode, ComposedExpression::OP_LIKE);
			}
			if(placeId)
			{
				query.addWhereField(COL_PLACEID, *placeId);
			}
			if(orderByCityAndStopName)
			{
				query.addTableAndEqualJoin<StopAreaTableSync>(TABLE_COL_ID, COL_PLACEID);
				query.addTableAndEqualOtherJoin<CityTableSync, StopAreaTableSync>(TABLE_COL_ID, StopAreaTableSync::TABLE_COL_CITYID);
				query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderFieldOther<StopAreaTableSync>(StopAreaTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderField(StopPointTableSync::COL_NAME, raisingOrder);
			}
			if(number)
			{
				query.setNumber(*number + 1);
				if(first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}

		
		
		StopPointTableSync::SearchResult StopPointTableSync::SearchDistance(
			StopPoint const& point,
			Env& env,
			bool insideSameStopArea,
			double const& distance,
			LinkLevel linkLevel
		){
			SelectQuery<StopPointTableSync> query;
			std::stringstream subQuery;

			subQuery << "Glength(GeomFromText('LINESTRING(" << lexical_cast<string>(point.getGeometry()->getX()) << " " 
				<< lexical_cast<string>(point.getGeometry()->getY()) << ", '||" << "t012_physical_stops.x" 
				<< "||' '||" << "t012_physical_stops.y" << "||')'))>" << distance 
				<< " AND " << point.getKey() <<"!=" << "t012_physical_stops.id ";
			if(insideSameStopArea)
				subQuery << "AND " << point.getConnectionPlace()->getKey() << "=" << "t012_physical_stops.place_id";

			query.addWhere(
				SubQueryExpression::Get(subQuery.str())
			);
			
			return LoadFromQuery(query, env, linkLevel);
		}



		db::RowsList StopPointTableSync::SearchForAutoComplete(
			const boost::optional<std::string> prefix,
			const boost::optional<std::size_t> limit,
			const boost::optional<std::string> optionalParameter
			) const {
				RowsList result;

				SelectQuery<StopPointTableSync> query;
				Env env;
				if(prefix)
				{
					query.addWhereField(StopPointTableSync::COL_NAME, "%"+ *prefix +"%", ComposedExpression::OP_LIKE);
				}
				if(optionalParameter)
				{
					query.addWhereField(StopPointTableSync::COL_PLACEID, *optionalParameter);
				}
				if(limit)
				{
					query.setNumber(*limit);
				}
				query.addOrderField(StopPointTableSync::COL_NAME,true);
				StopPointTableSync::SearchResult stops(StopPointTableSync::LoadFromQuery(query, env, UP_LINKS_LOAD_LEVEL));
				BOOST_FOREACH(const boost::shared_ptr<StopPoint>& stop, stops)
				{
					result.push_back(std::make_pair(stop->getKey(), stop->getCodeBySources() + " / " + stop->getName()));
				}
				return result;
		} ;
}	}
