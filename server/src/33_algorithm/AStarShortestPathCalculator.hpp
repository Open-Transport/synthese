
/** AStarShortestPathCalculator class header.
	@file AStarShortestPathCalculator.hpp

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

#ifndef SYNTHESE_AStarShortestPathCalculator_H__
#define SYNTHESE_AStarShortestPathCalculator_H__

#include "AccessParameters.h"
#include "AlgorithmTypes.h"

#include <queue>

#include <geos/geom/LineString.h>

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace geography
	{
		class Place;
	}

	namespace graph
	{
		class AccessParameters;
		class Journey;
		class VertexAccessMap;
	}

	namespace pt
	{
		class StopPoint;
	}

	namespace road
	{
		class Address;
		class Crossing;
		class RoadChunkEdge;
	}

	namespace algorithm
	{
		class AStarNode;

		class AStarShortestPathCalculator
		{
		public:
			typedef std::vector<const road::RoadChunkEdge*> ResultPath;
			typedef std::map<util::RegistryKeyType, boost::shared_ptr<AStarNode> > NodeMap;
			typedef std::set<const pt::StopPoint*> FoundStops;

		private:
			const geography::Place* const _departurePlace;
			const geography::Place* const _arrivalPlace;
			const boost::posix_time::ptime& _departureTime;
			const graph::AccessParameters _accessParameters;
			const algorithm::PlanningPhase _direction;
			const graph::VertexAccessMap& _startingVertices;
			const graph::VertexAccessMap& _endingVertices;

		public:
			AStarShortestPathCalculator(
				const geography::Place* origin,
				const geography::Place* destination,
				const boost::posix_time::ptime& departureTime,
				const graph::AccessParameters accessParameters,
				const algorithm::PlanningPhase direction = algorithm::DEPARTURE_TO_ARRIVAL
			);


			AStarShortestPathCalculator(
				const graph::VertexAccessMap& startingVertices,
				const graph::VertexAccessMap& endingVertices,
				const boost::posix_time::ptime& departureTime,
				const graph::AccessParameters accessParameters,
				const algorithm::PlanningPhase direction = algorithm::DEPARTURE_TO_ARRIVAL
			);



			AStarShortestPathCalculator(
				const boost::posix_time::ptime& departureTime,
				const graph::AccessParameters accessParameters,
				const algorithm::PlanningPhase direction = algorithm::DEPARTURE_TO_ARRIVAL
			);



			ResultPath run() const;



			graph::VertexAccessMap roadPlanningToClosePhysicalStops(
				const graph::VertexAccessMap& originVAM,
				const graph::VertexAccessMap& destinationVAM
			) const;

		private:
			boost::shared_ptr<AStarNode> _findShortestPath(
				NodeMap& nodeMap,
				std::priority_queue<boost::shared_ptr<AStarNode> >& openSet,
				const graph::VertexAccessMap& endingVertices,
				const boost::shared_ptr<geos::geom::Point> heuristicReference,
				const bool findAllAccessibleVertices = false
			) const;



			int _getHeuristicScore(
				const road::Crossing* origin,
				const boost::shared_ptr<geos::geom::Point> destination
			) const;



			void _reconstructPath(
				ResultPath& result,
				boost::shared_ptr<AStarNode> curNode
			) const;



			graph::Journey _generateJourneyFromNode(
				const pt::StopPoint* arrival,
				const boost::shared_ptr<AStarNode> lastNode
			) const;


			boost::shared_ptr<geos::geom::LineString> _computeGeometryExtremity(
				const road::Address* startAddress,
				const road::Address* endAddress,
				const road::RoadChunkEdge* chunk,
				bool chunkIsForward,
				ResultPath::iterator insertPosition,
				ResultPath& path
			) const;
		};

		class AStarNode 
		{
		private:
			const road::Crossing* _node;
			boost::shared_ptr<AStarNode> _parent;
			const road::RoadChunkEdge* _link;
			int _heuristicCost;
			int _realCost;
			double _distance;
			bool _visited;

		public:
			AStarNode(
				const road::Crossing* node,
				const int heuristicCost,
				const int realCost,
				const double distance
			);

			const road::Crossing* getCrossing(){ return _node; }
			boost::shared_ptr<AStarNode> getParent(){ return _parent; }
			const road::RoadChunkEdge* getLink(){ return _link; }
			int getHeuristicCost(){ return _heuristicCost; }
			int getRealCost(){ return _realCost; }
			double getDistance(){ return _distance; }
			bool isVisited(){ return _visited; }

			void setParent(boost::shared_ptr<AStarNode> parent){ _parent = parent; }
			void setLink(const road::RoadChunkEdge* link){ _link = link; }
			void setHeuristicCost(int heuristicCost){ _heuristicCost = heuristicCost; }
			void setRealCost(int realCost){ _realCost = realCost; }
			void setDistance(double distance){ _distance = distance; }
			void markAsVisited(){ _visited = true; }
		};

		struct AStarNodeComparator {
			bool operator()(const boost::shared_ptr<AStarNode> n1, const boost::shared_ptr<AStarNode> n2)
			{
				if(n1->getRealCost() == n2->getRealCost())
				{
					if(n1->getHeuristicCost() == n2->getHeuristicCost())
					{
						return &n1 < &n2;
					}
					else
					{
						return n1->getHeuristicCost() < n2->getHeuristicCost();
					}
				}
				else
				{
					return n1->getRealCost() < n2->getRealCost();
				}
			}
		};
	}
}

#endif // SYNTHESE_AStarShortestPathCalculator_H___
