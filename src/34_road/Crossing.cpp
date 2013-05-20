
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
#include "ReachableFromCrossing.hpp"
#include "RoadModule.h"
#include "VertexAccessMap.h"

using namespace std;
using namespace boost;
using namespace geos::geom;


namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace geography;
	using namespace impex;



	namespace util
	{
		template<> const string Registry<road::Crossing>::KEY("Crossing");
	}

	namespace road
	{
		Crossing::Crossing(
			util::RegistryKeyType key,
			boost::shared_ptr<Point> geometry,
			std::string codeBySource,
			const impex::DataSource* source,
			bool withIndexation
		):	Registrable(key),
			Hub(),
			Vertex(this, geometry, withIndexation)
		{
			// Data source
			if(source)
			{
				DataSourceLinks links;
				links.insert(make_pair(source, codeBySource));
				setDataSourceLinksWithoutRegistration(links);
			}

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
}	}
