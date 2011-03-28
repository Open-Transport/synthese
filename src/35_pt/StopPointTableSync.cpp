
/** StopPointTableSync class implementation.
    @file StopPointTableSync.cpp

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

#include "StopPointTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "StopAreaTableSync.hpp"
#include "CoordinatesSystem.hpp"
#include "RoadChunkTableSync.h"
#include "LinkException.h"
#include "CityTableSync.h"
#include "CrossingTableSync.hpp"
#include "ImportableTableSync.hpp"

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

	template<> const string util::FactorableTemplate<DBTableSync,StopPointTableSync>::FACTORY_KEY("35.55.01 Physical stops");
	template<> const string FactorableTemplate<Fetcher<graph::Vertex>, StopPointTableSync>::FACTORY_KEY("12");

	namespace pt
	{
		const string StopPointTableSync::COL_NAME = "name";
		const string StopPointTableSync::COL_PLACEID = "place_id";
		const string StopPointTableSync::COL_X = "x";
		const string StopPointTableSync::COL_Y = "y";
		const string StopPointTableSync::COL_OPERATOR_CODE("operator_code");
		const string StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID("projected_road_chunk_id");
		const string StopPointTableSync::COL_PROJECTED_METRIC_OFFSET("projected_metric_offset");
	}

    namespace db
    {
		template<> const DBTableSync::Format DBTableSyncTemplate<StopPointTableSync>::TABLE(
			"t012_physical_stops"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<StopPointTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(StopPointTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(StopPointTableSync::COL_PLACEID, SQL_INTEGER),
			DBTableSync::Field(StopPointTableSync::COL_X, SQL_DOUBLE),
			DBTableSync::Field(StopPointTableSync::COL_Y, SQL_DOUBLE),
			DBTableSync::Field(StopPointTableSync::COL_OPERATOR_CODE, SQL_TEXT),
			DBTableSync::Field(StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID, SQL_INTEGER),
			DBTableSync::Field(StopPointTableSync::COL_PROJECTED_METRIC_OFFSET, SQL_DOUBLE),
			DBTableSync::Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<StopPointTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(StopPointTableSync::COL_PLACEID.c_str(), ""),
			DBTableSync::Index(StopPointTableSync::COL_OPERATOR_CODE.c_str(), ""),
			DBTableSync::Index()
		};


		/** Does not update the place */
		template<> void DBDirectTableSyncTemplate<StopPointTableSync,StopPoint>::Load(
			StopPoint* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( StopPointTableSync::COL_NAME));

			// Position : Lon/lat prior to x/y
			object->resetGeometry();
			if(!rows->getText(TABLE_COL_GEOMETRY).empty())
			{
				shared_ptr<Point> point(
					static_pointer_cast<Point, Geometry>(
						rows->getGeometryFromWKT(TABLE_COL_GEOMETRY)
				)	);
				if(point.get())
				{
					object->setGeometry(point);
				}
			}
			else if(rows->getDouble(StopPointTableSync::COL_X) > 0 && rows->getDouble(StopPointTableSync::COL_Y) > 0)
			{
				object->setGeometry(
					CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
						rows->getDouble(StopPointTableSync::COL_X),
						rows->getDouble(StopPointTableSync::COL_Y)
				)	);
			}

			object->setDataSourceLinks(
				ImportableTableSync::GetDataSourceLinksFromSerializedString(
					rows->getText(StopPointTableSync::COL_OPERATOR_CODE),
					env
			)	);

			object->setHub(NULL);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Stop area
				try
				{
					StopArea* place(
						StopAreaTableSync::GetEditable(
							rows->getLongLong (StopPointTableSync::COL_PLACEID),
							env,
							linkLevel
						).get()
					);
					object->setHub(place);

					place->addPhysicalStop(*object);
				}
				catch (ObjectNotFoundException<StopArea>& e)
				{
					throw LinkException<StopAreaTableSync>(rows, StopPointTableSync::COL_PLACEID, e);
				}

				// Projected point
				RegistryKeyType chunkId(rows->getLongLong(StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID));
				if(chunkId > 0)
				{
					try
					{
						MainRoadChunk& chunk(*RoadChunkTableSync::GetEditable(chunkId, env, linkLevel));
						RoadChunk::MetricOffset metricOffset(rows->getDouble(StopPointTableSync::COL_PROJECTED_METRIC_OFFSET));

						object->setProjectedPoint(Address(chunk, metricOffset));

						chunk.getFromCrossing()->addReachableVertex(object);
					}
					catch (ObjectNotFoundException<MainRoadChunk>& e)
					{
						throw LinkException<RoadChunkTableSync>(rows, StopPointTableSync::COL_PROJECTED_ROAD_CHUNK_ID, e);
					}
				}
			}
		}



		template<> void DBDirectTableSyncTemplate<StopPointTableSync,StopPoint>::Unlink(
			StopPoint* obj
		){
			StopArea* place = const_cast<StopArea*>(obj->getConnectionPlace());
			place->removePhysicalStop(*obj);

			obj->setHub(NULL);

			if(obj->getProjectedPoint().getRoadChunk())
			{
				obj->getProjectedPoint().getRoadChunk()->getFromCrossing()->removeReachableVertex(obj);
			}
		}



		template<> void DBDirectTableSyncTemplate<StopPointTableSync,StopPoint>::Save(
			StopPoint* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<StopPointTableSync> query(*object);
			query.addField(object->getName());
			query.addField(dynamic_cast<const StopArea*>(object->getHub()) ? dynamic_cast<const StopArea*>(object->getHub())->getKey() : RegistryKeyType(0));
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
			query.addField(
				ImportableTableSync::SerializeDataSourceLinks(object->getDataSourceLinks())
			);
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
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
