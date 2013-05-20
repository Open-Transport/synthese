
/** CrossingTableSync class implementation.
	@file CrossingTableSync.cpp

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

#include "CrossingTableSync.hpp"
#include "RoadTableSync.h"
#include "RoadPlaceTableSync.h"
#include "Crossing.h"
#include "DBModule.h"
#include "LinkException.h"
#include "DataSource.h"
#include "DataSourceLinksField.hpp"
#include "DataSourceTableSync.h"
#include "SelectQuery.hpp"
#include "ReplaceQuery.h"
#include "ImportableTableSync.hpp"

#include <sstream>
#include <geos/geom/Point.h>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace impex;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, CrossingTableSync>::FACTORY_KEY("34.20.01 Crossings");
		template<> const string FactorableTemplate<Fetcher<Vertex>, CrossingTableSync>::FACTORY_KEY("43");
	}

	namespace road
	{
		const std::string CrossingTableSync::COL_CODE_BY_SOURCE ("code_by_source");
		const std::string CrossingTableSync::COL_NON_REACHABLE_ROADS ("non_reachable_roads");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CrossingTableSync>::TABLE(
			"t043_crossings"
		);

		template<> const Field DBTableSyncTemplate<CrossingTableSync>::_FIELDS[] =
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(CrossingTableSync::COL_CODE_BY_SOURCE, SQL_TEXT),
			Field(CrossingTableSync::COL_NON_REACHABLE_ROADS, SQL_TEXT),
			Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CrossingTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void OldLoadSavePolicy<CrossingTableSync, Crossing>::Load(
			Crossing* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Geometry
			boost::shared_ptr<Point> point(
				static_pointer_cast<Point, Geometry>(
					rows->getGeometryFromWKT(TABLE_COL_GEOMETRY)
			)	);
			if(point.get())
			{
				object->setGeometry(point);
			}

			// Code by source
			object->setDataSourceLinksWithoutRegistration(
				ImportableTableSync::GetDataSourceLinksFromSerializedString(
					rows->getText(CrossingTableSync::COL_CODE_BY_SOURCE),
					env
			)	);

			object->setNonReachableRoads(
				CrossingTableSync::UnserializeNonReachableRoads(
					rows->getText(CrossingTableSync::COL_NON_REACHABLE_ROADS),
					env
				)
			);
		}



		template<> void OldLoadSavePolicy<CrossingTableSync, Crossing>::Unlink(
			Crossing* obj
		){
		}



		template<> void OldLoadSavePolicy<CrossingTableSync, Crossing>::Save(
			Crossing* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<CrossingTableSync> query(*object);
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.addField(
				CrossingTableSync::SerializeNonReachableRoads(object->getNonReachableRoads())
			);
			query.addField(static_pointer_cast<Geometry,Point>(object->getGeometry()));
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<CrossingTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<CrossingTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CrossingTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CrossingTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace road
	{
		CrossingTableSync::SearchResult CrossingTableSync::Search(
			Env& env,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<CrossingTableSync> query;
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}
			return LoadFromQuery(query, env, linkLevel);
		}

		std::string CrossingTableSync::SerializeNonReachableRoads(const Crossing::NonReachableRoadFromRoad& value)
		{
			bool first(true);
			stringstream nonReachableRoads;
			BOOST_FOREACH(const Crossing::NonReachableRoadFromRoad::value_type& roadPair, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					nonReachableRoads << ",";
				}
				nonReachableRoads << roadPair.first->getKey() << "|" << roadPair.second->getKey();
			}
			return nonReachableRoads.str();
		}

		Crossing::NonReachableRoadFromRoad CrossingTableSync::UnserializeNonReachableRoads(
			const std::string& value,
			Env& env
		){
			if(value.empty())
			{
				return Crossing::NonReachableRoadFromRoad();
			}
			else
			{
				Crossing::NonReachableRoadFromRoad nonReachableRoads;
				vector<string> pairsVec;
				split(pairsVec, value, is_any_of(","));
				BOOST_FOREACH(const string& pairStr, pairsVec)
				{
					try
					{
						vector<string> roadsVec;
						split(roadsVec, pairStr, is_any_of("|"));

						Road* from(
							RoadTableSync::GetEditable(
								lexical_cast<RegistryKeyType>(roadsVec.at(0)),
								 env
							).get()
						);

						Road* to(
							RoadTableSync::GetEditable(
								lexical_cast<RegistryKeyType>(roadsVec.at(1)),
								 env
							).get()
						);

						nonReachableRoads.insert(make_pair(from, to));
					}
					catch(bad_lexical_cast&)
					{
					}
				}
				return nonReachableRoads;
			}
		}
	}
}
