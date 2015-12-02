
/** Crossing class implementation.
	@file Crossing.cpp

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

#include "Crossing.h"

#include "AllowedUseRule.h"
#include "CrossingTableSync.hpp"
#include "DBModule.h"
#include "ImportableTableSync.hpp"
#include "ReachableFromCrossing.hpp"
#include "RoadModule.h"
#include "VertexAccessMap.h"

#include <sstream>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;


namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace geography;
	using namespace impex;
	using namespace road;
	
	CLASS_DEFINITION(Crossing, "t043_crossings", 43)
	FIELD_DEFINITION_OF_OBJECT(Crossing, "crossing_id", "crossing_ids")
	
	FIELD_DEFINITION_OF_TYPE(NonReachableRoads, "non_reachable_roads", SQL_TEXT)

	namespace road
	{
		const string Crossing::TAG_NON_REACHABLE_ROADS = "non_reachable_roads";

		Crossing::Crossing(
			util::RegistryKeyType key,
			boost::shared_ptr<Point> geometry,
			bool withIndexation
		):	Registrable(key),
			Hub(),
			Vertex(this, geometry, withIndexation),
			Object<Crossing, CrossingSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_DEFAULT_CONSTRUCTOR(NonReachableRoads),
					FIELD_VALUE_CONSTRUCTOR(PointGeometry, geometry)
			)	)
		{
			// Default accessibility
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			Hub::setRules(rules);
		}



		Crossing::~Crossing ()
		{}



		bool Crossing::isConnectionAllowed(
			const Vertex& fromVertex,
			const Vertex& toVertex
		) const	{
			return true;
		}



		const boost::shared_ptr<geos::geom::Point>& Crossing::getPoint() const
		{
			return WithGeometry<Point>::getGeometry();
		}



		posix_time::time_duration Crossing::getMinTransferDelay() const
		{
			return posix_time::minutes(0);
		}



		bool Crossing::containsAnyVertex( graph::GraphIdType graphType ) const
		{
			if(graphType == RoadModule::GRAPH_ID)
			{
				return true;
			}

			BOOST_FOREACH(ReachableFromCrossing* vertex, _reachableVertices)
			{
				if(vertex->getGraphType() == graphType)
				{
					return true;
				}
			}

			return false;
		}



		void Crossing::getVertexAccessMap(
			graph::VertexAccessMap& result,
			graph::GraphIdType whatToSearch,
			const graph::Vertex& origin,
			bool vertexIsOrigin
		) const	{
			if(whatToSearch == RoadModule::GRAPH_ID)
			{
				result.insert(
					this,
					VertexAccess()
				);
			}

			BOOST_FOREACH(ReachableFromCrossing* vertex, _reachableVertices)
			{
				if(	vertex->getGraphType() != whatToSearch
				){
					continue;
				}

				assert(dynamic_cast<Vertex*>(vertex));

				result.insert(
					dynamic_cast<Vertex*>(vertex),
					vertex->getVertexAccess(*this)
				);
			}
		}



		bool Crossing::isNonReachableRoad(
			const Road* from,
			const Road* to
		) const {
			if(_nonReachableRoadFromRoad.find(make_pair(from, to)) != _nonReachableRoadFromRoad.end())
			{
				return true;
			}
			else
			{
				return false;
			}
		}



		synthese::graph::HubScore Crossing::getScore() const
		{
			return 1;
		}



		posix_time::time_duration Crossing::getTransferDelay(
			const graph::Vertex& fromVertex,
			const graph::Vertex& toVertex
		) const	{
			return posix_time::minutes(0);
		}



		graph::GraphIdType Crossing::getGraphType() const
		{
			return RoadModule::GRAPH_ID;
		}



		std::string Crossing::getRuleUserName() const
		{
			return "Intersection";
		}



		Hub::Vertices Crossing::getVertices( graph::GraphIdType graphId ) const
		{
			Vertices result;
			if(graphId == RoadModule::GRAPH_ID)
			{
				result.push_back(this);
			}
			return result;
		}
		
		void Crossing::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /* = std::string */
		) const	{
			
			// Non reachable roads as sub map
			BOOST_FOREACH(const NonReachableRoads::Type::value_type& road, get<NonReachableRoads>())
			{
				boost::shared_ptr<ParametersMap> roadPM(new ParametersMap);
				road->toParametersMap(*roadPM, true);
				map.insert(prefix + TAG_NON_REACHABLE_ROADS, roadPM);
			}

		}
		
		void Crossing::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<PointGeometry>())
			{
				setGeometry(get<PointGeometry>());
			}
		}
		
		void Crossing::unlink()
		{
		}
}	}
