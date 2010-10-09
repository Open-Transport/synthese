
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
#include "CoordinatesSystem.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <geos/geom/LineString.h>


using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, RoadChunkTableSync>::FACTORY_KEY("34.30.01 Road chunks");
	}

	namespace road
	{
		const string RoadChunkTableSync::COL_CROSSING_ID("address_id");
		const string RoadChunkTableSync::COL_RANKINPATH ("rank_in_path");
		const string RoadChunkTableSync::COL_GEOMETRY("geometry");  // list of ids
		const string RoadChunkTableSync::COL_ROADID ("road_id");  // NU
		const string RoadChunkTableSync::COL_METRICOFFSET ("metric_offset");  // U ??
		const string RoadChunkTableSync::COL_LEFT_MIN_HOUSE_NUMBER("left_min_house_number");
		const string RoadChunkTableSync::COL_LEFT_MAX_HOUSE_NUMBER("left_max_house_number");
		const string RoadChunkTableSync::COL_RIGHT_MIN_HOUSE_NUMBER("right_min_house_number");
		const string RoadChunkTableSync::COL_RIGHT_MAX_HOUSE_NUMBER("right_max_house_number");
		const string RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY("left_house_numbering_policy");
		const string RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY("right_house_numbering_policy");
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
			SQLiteTableSync::Field(RoadChunkTableSync::COL_ROADID, SQL_INTEGER, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_METRICOFFSET, SQL_DOUBLE, false),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_LEFT_MIN_HOUSE_NUMBER, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_LEFT_MAX_HOUSE_NUMBER, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_RIGHT_MIN_HOUSE_NUMBER, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_RIGHT_MAX_HOUSE_NUMBER, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY, SQL_INTEGER),
			SQLiteTableSync::Field(RoadChunkTableSync::COL_GEOMETRY, SQL_GEOM_LINESTRING),
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
			string viaPointsStr(rows->getText (RoadChunkTableSync::COL_GEOMETRY));
			if(viaPointsStr.empty())
			{
				object->setGeometry(shared_ptr<LineString>());
			}
			else
			{
				object->setGeometry(
					dynamic_pointer_cast<LineString,Geometry>(rows->getGeometryFromWKB(RoadChunkTableSync::COL_GEOMETRY))
				);
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

				// Left house number bounds
				if(	!rows->getText(RoadChunkTableSync::COL_LEFT_MIN_HOUSE_NUMBER).empty() &&
					!rows->getText(RoadChunkTableSync::COL_LEFT_MAX_HOUSE_NUMBER).empty()
				){
					((object->getRoad()->getSide() == Road::LEFT_SIDE) ? object : object->getReverseChunk())->setHouseNumberBounds(
						RoadChunk::HouseNumberBounds(RoadChunk::HouseNumberBounds::value_type(
							rows->getInt(RoadChunkTableSync::COL_LEFT_MIN_HOUSE_NUMBER),
							rows->getInt(RoadChunkTableSync::COL_LEFT_MIN_HOUSE_NUMBER)
					)	));

					// Left House numbering policy
					((object->getRoad()->getSide() == Road::LEFT_SIDE) ? object : object->getReverseChunk())->setHouseNumberingPolicy(
						static_cast<RoadChunk::HouseNumberingPolicy>(rows->getInt(RoadChunkTableSync::COL_LEFT_HOUSE_NUMBERING_POLICY))
					);
				}


				// Right house number bounds
				if(	!rows->getText(RoadChunkTableSync::COL_RIGHT_MIN_HOUSE_NUMBER).empty() &&
					!rows->getText(RoadChunkTableSync::COL_RIGHT_MAX_HOUSE_NUMBER).empty()
				){
					((object->getRoad()->getSide() == Road::RIGHT_SIDE) ? object : object->getReverseChunk())->setHouseNumberBounds(
						RoadChunk::HouseNumberBounds(RoadChunk::HouseNumberBounds::value_type(
							rows->getInt(RoadChunkTableSync::COL_RIGHT_MIN_HOUSE_NUMBER),
							rows->getInt(RoadChunkTableSync::COL_RIGHT_MIN_HOUSE_NUMBER)
					)	));

					// Right House numbering policy
					((object->getRoad()->getSide() == Road::RIGHT_SIDE) ? object : object->getReverseChunk())->setHouseNumberingPolicy(
						static_cast<RoadChunk::HouseNumberingPolicy>(rows->getInt(RoadChunkTableSync::COL_RIGHT_HOUSE_NUMBERING_POLICY))
					);
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
			RoadChunk* leftChunk(NULL);
			RoadChunk* rightChunk(NULL);
			
			if(object->getRoad())
			{
				if(object->getRoad()->getSide() == Road::LEFT_SIDE)
				{
					leftChunk = object;
					rightChunk = object->getReverseChunk();
				}
				else
				{
					leftChunk = object->getReverseChunk();
					rightChunk = object;
				}
			}

			ReplaceQuery<RoadChunkTableSync> query(*object);
			query.addField(object->getFromCrossing() ? object->getFromCrossing()->getKey() : RegistryKeyType(0));
			query.addField(object->getRankInPath());
			query.addField(object->getRoad() ? object->getRoad()->getKey() : RegistryKeyType(0));
			query.addField(object->getMetricOffset());
			query.addField((leftChunk && leftChunk->getHouseNumberBounds()) ? lexical_cast<string>(leftChunk->getHouseNumberBounds()->first) : string());
			query.addField((leftChunk && leftChunk->getHouseNumberBounds()) ? lexical_cast<string>(leftChunk->getHouseNumberBounds()->second) :	string());
			query.addField((rightChunk && rightChunk->getHouseNumberBounds()) ? lexical_cast<string>(rightChunk->getHouseNumberBounds()->first) : string());
			query.addField((rightChunk && rightChunk->getHouseNumberBounds()) ? lexical_cast<string>(rightChunk->getHouseNumberBounds()->second) :	string());
			query.addField(static_cast<int>((leftChunk && leftChunk->getHouseNumberBounds()) ? leftChunk->getHouseNumberingPolicy() : RoadChunk::ALL));
			query.addField(static_cast<int>((rightChunk && rightChunk->getHouseNumberBounds()) ? rightChunk->getHouseNumberingPolicy() : RoadChunk::ALL));
			query.addField(static_pointer_cast<Geometry,LineString>(object->getGeometry()));
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



		RoadChunkTableSync::SearchResult RoadChunkTableSync::SearchByMaxDistance(
			const geos::geom::Point& point,
			double distanceLimit,
			util::LinkLevel linkLevel
		){
			if(point.isEmpty())
			{
				return SearchResult();
			}

			shared_ptr<Point> minPoint(
				DBModule::GetStorageCoordinatesSystem().convertPoint(
					*CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
						point.getX() - distanceLimit,
						point.getY() - distanceLimit
			)	)	);
			shared_ptr<Point> maxPoint(
				DBModule::GetStorageCoordinatesSystem().convertPoint(
					*CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
						point.getX() + distanceLimit,
						point.getY() + distanceLimit
			)	)	);
			
			stringstream subQuery;
			subQuery << "SELECT pkid FROM idx_" << RoadChunkTableSync::TABLE.NAME << "_" << RoadChunkTableSync::COL_GEOMETRY << " WHERE " <<
				"xmin > " << minPoint->getX() << " AND xmax < " << maxPoint->getX() <<
				" AND ymin > " << minPoint->getY() << " AND ymax < " << maxPoint->getY()
			;

			SelectQuery<RoadChunkTableSync> query;
			query.addTableField(TABLE_COL_ID);
			query.addWhere(
				ComposedExpression::Get(
					FieldExpression::Get(RoadChunkTableSync::TABLE.NAME, TABLE_COL_ID),
					ComposedExpression::OP_IN,
					SubQueryExpression::Get(subQuery.str())
			)	);

			return LoadFromQuery(query, Env::GetOfficialEnv(), linkLevel);
		}
	}
}
