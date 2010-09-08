
/** RoadChunkTableSync class implementation.
	@file RoadChunkTableSync.cpp

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

#include "RoadChunkTableSync.h"
#include "CrossingTableSync.hpp"
#include "RoadTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "LinkException.h"
#include "GeoPoint.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <geos/geom/LineString.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>


using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, RoadChunkTableSync>::FACTORY_KEY("34.30.01 Road chunks");
	}

	namespace road
	{
		const string RoadChunkTableSync::COL_CROSSING_ID("address_id");
		const string RoadChunkTableSync::COL_RANKINPATH ("rank_in_path");
		const string RoadChunkTableSync::COL_VIAPOINTS ("via_points");  // list of ids
		const string RoadChunkTableSync::COL_ROADID ("road_id");  // NU
		const string RoadChunkTableSync::COL_METRICOFFSET ("metric_offset");  // U ??

		const string RoadChunkTableSync::SEP_POINTS(",");
		const string RoadChunkTableSync::SEP_LON_LAT(":");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<RoadChunkTableSync>::TABLE(
			"t014_road_chunks"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<RoadChunkTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_CROSSING_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_RANKINPATH, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_VIAPOINTS, SQL_TEXT),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_ROADID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_METRICOFFSET, SQL_DOUBLE, false),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<RoadChunkTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				RoadChunkTableSync::COL_ROADID.c_str(),
				RoadChunkTableSync::COL_RANKINPATH.c_str(),
				""
			),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Load(
			RoadChunk* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    // Rank in road
		    int rankInRoad (rows->getInt (RoadChunkTableSync::COL_RANKINPATH));
		    object->setRankInPath(rankInRoad);
			
			// Metric offset
			object->setMetricOffset(rows->getDouble (RoadChunkTableSync::COL_METRICOFFSET));

		    // Geometry
			string viaPointsStr(rows->getText (RoadChunkTableSync::COL_VIAPOINTS));
			if(viaPointsStr.empty())
			{
				object->setGeometry(shared_ptr<LineString>());
			}
			else
			{
				const GeometryFactory* geometryFactory(GeometryFactory::getDefaultInstance());
				vector<Coordinate>* coordinates = new vector<Coordinate>();

				vector<string> points;
				algorithm::split(points, viaPointsStr, algorithm::is_any_of(RoadChunkTableSync::SEP_POINTS));

				if(points.size() > 1)
				{
					BOOST_FOREACH(const string& point, points)
					{
						vector<string> lonlat;
						algorithm::split(lonlat, point, algorithm::is_any_of(RoadChunkTableSync::SEP_LON_LAT));

						if(lonlat.size() < 2)
						{
							continue;
						}

						GeoPoint pt(
							lexical_cast<double>(lonlat[0]),
							lexical_cast<double>(lonlat[1])
						);

						coordinates->push_back(
							pt
						);
					}

					CoordinateSequence *cs = geometryFactory->getCoordinateSequenceFactory()->create(coordinates);
					//coordinates is now owned by cs

					object->setGeometry(shared_ptr<LineString>(geometryFactory->createLineString(cs)));
					//cs is now owned by the geometry
				}
				else
				{
					object->setGeometry(shared_ptr<LineString>());
				}
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					shared_ptr<Road> road(RoadTableSync::GetEditable(rows->getLongLong(RoadChunkTableSync::COL_ROADID), env, linkLevel));
					object->setRoad(road.get());
					object->setFromCrossing(
						CrossingTableSync::GetEditable(
							rows->getLongLong(RoadChunkTableSync::COL_CROSSING_ID),
							env,
							linkLevel
						).get()
					);
					road->addRoadChunk(*object);
				}
				catch (ObjectNotFoundException<Road>& e)
				{
					throw LinkException<RoadChunkTableSync>(rows, RoadChunkTableSync::COL_ROADID, e);
				}
				catch (ObjectNotFoundException<Crossing>& e)
				{
					throw LinkException<RoadChunkTableSync>(rows, RoadChunkTableSync::COL_CROSSING_ID, e);
				}
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Unlink(
			RoadChunk* obj
		){
		}

	    
	    template<> void SQLiteDirectTableSyncTemplate<RoadChunkTableSync,RoadChunk>::Save(
			RoadChunk* object,
			optional<SQLiteTransaction&> transaction
		){
			stringstream viaPoints;
			if(object->getStoredGeometry().get())
			{
				viaPoints << fixed;
				const CoordinateSequence* coords(object->getStoredGeometry()->getCoordinatesRO());
				
				for(size_t i(0); i<coords->getSize(); ++i)
				{
					if(i>0)
					{
						viaPoints << RoadChunkTableSync::SEP_POINTS;
					}
					GeoPoint pt(coords->getAt(i));
					viaPoints << pt.getLongitude() << RoadChunkTableSync::SEP_LON_LAT << pt.getLatitude();
				}
			}

			ReplaceQuery<RoadChunkTableSync> query(*object);
			query.addField(object->getFromCrossing() ? object->getFromCrossing()->getKey() : RegistryKeyType(0));
			query.addField(object->getRankInPath());
			query.addField(viaPoints.str());
			query.addField(object->getRoad() ? object->getRoad()->getKey() : RegistryKeyType(0));
			query.addField(object->getMetricOffset());
			query.execute(transaction);
	    }
	}

	namespace road
	{
	    RoadChunkTableSync::SearchResult RoadChunkTableSync::Search(
			Env& env,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<RoadChunkTableSync> query;
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
	    }
	}
}
