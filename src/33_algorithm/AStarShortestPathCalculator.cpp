
/** AStarShortestPathCalculator class implementation.
	@file AStarShortestPathCalculator.cpp

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

#include <geos/algorithm/CGAlgorithms.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/LineString.h>
#include <geos/operation/distance/DistanceOp.h>

#include "AStarShortestPathCalculator.hpp"

#include "AccessParameters.h"
#include "Crossing.h"
#include "Journey.h"
#include "MainRoadChunk.hpp"
#include "NamedPlace.h"
#include "Place.h"
#include "PTModule.h"
#include "ReverseRoadChunk.hpp"
#include "ReverseRoadPart.hpp"
#include "Road.h"
#include "RoadModule.h"
#include "ServicePointer.h"
#include "StopPoint.hpp"
#include "Vertex.h"
#include "VertexAccessMap.h"

using namespace std;
using namespace boost;
using namespace geos::algorithm;
using namespace geos::geom;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace road;
	using namespace util;
	
	namespace algorithm
	{
		AStarShortestPathCalculator::AStarShortestPathCalculator(
			const Place* origin,
			const Place* destination,
			const posix_time::ptime& departureTime,
			const AccessParameters accessParameters,
			const algorithm::PlanningPhase direction
		):	_departurePlace(origin),
			_arrivalPlace(destination),
			_departureTime(departureTime),
			_accessParameters(accessParameters),
			_direction(direction)
		{
		}



		AStarShortestPathCalculator::AStarShortestPathCalculator(
			const posix_time::ptime& departureTime,
			const AccessParameters accessParameters,
			const algorithm::PlanningPhase direction
		):	_departurePlace(NULL),
			_arrivalPlace(NULL),
			_departureTime(departureTime),
			_accessParameters(accessParameters),
			_direction(direction)
		{
		}



		AStarNode::AStarNode(
			const Crossing* node,
			const int heuristicCost,
			const int realCost,
			const double distance
		):	_node(node),
			_parent(boost::shared_ptr<AStarNode>()),
			_link(NULL),
			_heuristicCost(heuristicCost),
			_realCost(realCost),
			_distance(distance),
			_visited(false)
		{
		}



		bool operator<(const boost::shared_ptr<AStarNode> n1, const boost::shared_ptr<AStarNode> n2)
		{
			return n1->getHeuristicCost() > n2->getHeuristicCost();
		}



		AStarShortestPathCalculator::ResultPath AStarShortestPathCalculator::run() const
		{
			ResultPath result;

			if(!_departurePlace || !_arrivalPlace)
			{
				return result;
			}

			VertexAccessMap startingVertices = _departurePlace->getVertexAccessMap(_accessParameters, RoadModule::GRAPH_ID);
			VertexAccessMap endingVertices = _arrivalPlace->getVertexAccessMap(_accessParameters, RoadModule::GRAPH_ID);

			if(startingVertices.getMap().empty() || endingVertices.getMap().empty())
			{
				// error message, no road journey planning possible
				return result;
			}

			const boost::shared_ptr<Point> heuristicReference = endingVertices.getCentroid();
			priority_queue<boost::shared_ptr<AStarNode> > openSet;
			NodeMap nodeMap;

			BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& currentVertex, startingVertices.getMap())
			{
				if(const Crossing* c = dynamic_cast<const Crossing*>(currentVertex.first))
				{
					int crossingCost = currentVertex.second.approachTime.total_seconds();
					int heuristic = _getHeuristicScore(c, heuristicReference);
					boost::shared_ptr<AStarNode> startNode(new AStarNode(c, crossingCost + heuristic, crossingCost, currentVertex.second.approachDistance)); // The real cost might be bad (road_place or city...)
					nodeMap.insert(NodeMap::value_type(c->getKey(), startNode));
					openSet.push(startNode);
				}
			}

			boost::shared_ptr<AStarNode> lastNode = _findShortestPath(
				nodeMap,
				openSet,
				endingVertices,
				heuristicReference
			);

			if(lastNode.get())
			{
				_reconstructPath(result, lastNode);
			}

			return result;
		}



		boost::shared_ptr<AStarNode> AStarShortestPathCalculator::_findShortestPath(
			NodeMap& nodeMap,
			priority_queue<boost::shared_ptr<AStarNode> >& openSet,
			const VertexAccessMap& endingVertices,
			const boost::shared_ptr<Point> heuristicReference
		) const {
			boost::shared_ptr<AStarNode> resultNode;
			
			if(openSet.empty())
			{
				return resultNode;
			}

			while(!openSet.empty() && !resultNode.get())
			{
				boost::shared_ptr<AStarNode> curNode = openSet.top();
				openSet.pop();

				if(endingVertices.contains(curNode->getCrossing()))
				{
					resultNode = curNode;
				}
				else
				{
					curNode->markAsVisited();
					const Vertex::Edges& edges(curNode->getCrossing()->getDepartureEdges());

					BOOST_FOREACH(const Vertex::Edges::value_type& itEdges, edges)
					{
						const Path* path(itEdges.first);
						const RoadChunk* chunk = static_cast<const RoadChunk*>(itEdges.second);

						// Check if the edge exists and is authorized for the user class
						if(!chunk || !chunk->isCompatibleWith(_accessParameters))
							continue;

						// Specific car user class verification (turn restriction)
						if(_accessParameters.getUserClass() == USER_CAR && curNode->getLink())
						{
							const Road* from = static_cast<const Road*>(curNode->getLink()->getParentPath());
							const Road* to = static_cast<const Road*>(path);

							if(from->isReversed())
								from = static_cast<const ReverseRoadPart*>(from)->getMainRoad();

							if(to->isReversed())
								to = static_cast<const ReverseRoadPart*>(to)->getMainRoad();

							if((_direction == algorithm::DEPARTURE_TO_ARRIVAL) && curNode->getCrossing()->isNonReachableRoad(from, to))
								continue;
							else if((_direction == algorithm::ARRIVAL_TO_DEPARTURE) && curNode->getCrossing()->isNonReachableRoad(to, from))
								continue;
						}

						// Retrieving the next or the previous chunk in the path, if there is one
						RoadChunk* nextChunkInPath = static_cast<RoadChunk*>(_direction == algorithm::ARRIVAL_TO_DEPARTURE ? chunk->getPrevious() : chunk->getNext());
						if(!nextChunkInPath)
							continue;

						// Retrieving the associated crossing
						Crossing* nextCrossing = nextChunkInPath->getFromCrossing();

						// Check if it is already visited
						NodeMap::iterator nextNode = nodeMap.find(nextCrossing->getKey());
						if(nextNode != nodeMap.end() && nextNode->second->isVisited())
							continue;

						const RoadChunk* linkChunk;
						double distance(0);

						if(_direction == algorithm::DEPARTURE_TO_ARRIVAL)
							linkChunk = chunk;
						else
							linkChunk = nextChunkInPath;

						boost::shared_ptr<LineString> chunkGeom = linkChunk->getRealGeometry();
						
						if(chunkGeom)
							distance = CGAlgorithms::length(chunkGeom->getCoordinates());

						double speed(_accessParameters.getApproachSpeed());
						if(_accessParameters.getUserClass() == USER_CAR && linkChunk->getCarSpeed() > 0)
						{
							speed = linkChunk->getCarSpeed();
						}

						int newScore = curNode->getRealCost() + static_cast<int>(distance / speed);
						int heuristic = _getHeuristicScore(nextCrossing, heuristicReference);
						double newDistance = curNode->getDistance() + distance;

						// Check if compatible with max approach distance and max approach time (especially usefull to find close physical stops)
						if(!_accessParameters.isCompatibleWithApproach(newDistance, boost::posix_time::seconds(newScore)))
							continue;

						// If we haven't discovered the crossing yet, we're adding it to the open set, if we have, we're updating its score if ours is better
						if(nextNode == nodeMap.end())
						{
							boost::shared_ptr<AStarNode> newNode(new AStarNode(nextCrossing, newScore + heuristic, newScore, newDistance));
							newNode->setParent(curNode);
							newNode->setLink(linkChunk);
							nodeMap.insert(NodeMap::value_type(nextCrossing->getKey(), newNode));
							openSet.push(newNode);
						}
						else if(nextNode->second->getRealCost() > newScore)
						{
							nextNode->second->setParent(curNode);
							nextNode->second->setLink(linkChunk);
							nextNode->second->setHeuristicCost(newScore + heuristic);
							nextNode->second->setRealCost(newScore);
							nextNode->second->setDistance(newDistance);
						}
					}
				}
			}

			return resultNode;
		}



		int AStarShortestPathCalculator::_getHeuristicScore(
			const Crossing* origin,
			const boost::shared_ptr<Point> destination
		) const {
			boost::shared_ptr<Point> originPoint = origin->getGeometry();

			if(originPoint.get())
				return static_cast<int>(geos::operation::distance::DistanceOp::distance(*originPoint, *destination) / _accessParameters.getApproachSpeed());
			else
				return 0;
		}



		VertexAccessMap AStarShortestPathCalculator::roadPlanningToClosePhysicalStops(
			const VertexAccessMap& originVAM,
			const VertexAccessMap& destinationVAM
		) const {
			boost::shared_ptr<Point> heuristicReference = destinationVAM.getCentroid();
			VertexAccessMap result;
			FoundStops foundStops;

			priority_queue<boost::shared_ptr<AStarNode> > openSet;
			NodeMap nodeMap;

			BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& currentVertex, originVAM.getMap())
			{
				if(const Crossing* c = dynamic_cast<const Crossing*>(currentVertex.first))
				{
					int crossingCost = currentVertex.second.approachTime.total_seconds();
					int heuristic = _getHeuristicScore(c, heuristicReference);
					boost::shared_ptr<AStarNode> startNode(new AStarNode(c, crossingCost + heuristic, crossingCost, currentVertex.second.approachDistance)); // The real cost might be bad (road_place or city...)
					nodeMap.insert(NodeMap::value_type(c->getKey(), startNode));
					openSet.push(startNode);
				}
				else if(const pt::StopPoint* stop = dynamic_cast<const pt::StopPoint*>(currentVertex.first))
				{
					foundStops.insert(stop);
					result.insert(stop, currentVertex.second);
				}
			}

			boost::shared_ptr<AStarNode> lastNode = _findShortestPath(
				nodeMap,
				openSet,
				destinationVAM,
				heuristicReference
			);

			BOOST_FOREACH(const NodeMap::value_type& node, nodeMap)
			{
				VertexAccessMap crossingVAM;
				node.second->getCrossing()->getVertexAccessMap(crossingVAM, pt::PTModule::GRAPH_ID, *node.second->getCrossing(), false);

				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& currentVertex, crossingVAM.getMap())
				{
					const pt::StopPoint* stop = static_cast<const pt::StopPoint*>(currentVertex.first);
					if(foundStops.find(stop) == foundStops.end())
					{
						foundStops.insert(stop);
						Journey approachJourney = _generateJourneyFromNode(stop, node.second);
						result.insert(stop, VertexAccess(approachJourney.getEffectiveDuration(), approachJourney.getDistance(), approachJourney));
					}
				}
			}

			return result;
		}



		void AStarShortestPathCalculator::_reconstructPath(
			ResultPath& result,
			boost::shared_ptr<AStarNode> curNode
		) const {
			while(curNode->getParent())
			{
				result.insert(result.begin(), curNode->getLink());
				curNode = curNode->getParent(); 
			}
		}



		Journey AStarShortestPathCalculator::_generateJourneyFromNode(
			const pt::StopPoint* arrival,
			const boost::shared_ptr<AStarNode> lastNode
		) const {
			// Reconstructing a SYNTHESE Journey from a vector of edges returned by _findShortestPath
			Journey result;

			ResultPath path;
			_reconstructPath(path, lastNode);

			posix_time::ptime departure(_departureTime);

			// Iterating the edges vector
			for(ResultPath::iterator it = path.begin() ; it != path.end() ; it++)
			{
				optional<Edge::DepartureServiceIndex::Value> departureIndex;
				optional<Edge::ArrivalServiceIndex::Value> arrivalIndex;

				const RoadChunk* startChunk = *it;
				/*
					Retrieve the first, or last, edge of the path.
					There is a little difference between SYNTHESE's representation of a path and the vector we have.
					
					o------------> o---------------> o // This is internal SYNTHESE's reprensentation, 3 edges, the last one just carry the information of the end crossing

					In the result vector, we just have :

					o------------> o--------------->

					And the end crossing will be the crossing of the next edge in the vector.
				*/
				if(_direction == algorithm::ARRIVAL_TO_DEPARTURE) // Here, we want the last edge
					startChunk = static_cast<const RoadChunk*>(startChunk->getNext());

				ServicePointer service(
					(_direction == algorithm::DEPARTURE_TO_ARRIVAL) ?
					startChunk->getNextService(
						_accessParameters,
						departure,
						departure,
						true,
						departureIndex,
						false,
						true,
						true
					) :
					startChunk->getPreviousService(
						_accessParameters,
						departure,
						departure,
						true,
						arrivalIndex,
						false,
						true,
						true
					)
				);

				// Standing on the current path as long as we can (the path don't change and we aren't at the end of the vector yet)
				Path* currentPath = startChunk->getParentPath();
				MetricOffset startMetricOffset = startChunk->getMetricOffset();

				do {
					it++;
				}
				while(it != path.end() && currentPath == (*it)->getParentPath() && (_direction == algorithm::DEPARTURE_TO_ARRIVAL ? startMetricOffset < (*it)->getMetricOffset() : startMetricOffset > (*it)->getMetricOffset()));

				it--;

				// Retrieving the last edge of the path if we are on the common direction
				const RoadChunk* endChunk = *it;
				if(_direction == algorithm::DEPARTURE_TO_ARRIVAL)
					endChunk = static_cast<const RoadChunk*>(endChunk->getNext());

				ServicePointer completeService(
					service,
					*endChunk,
					_accessParameters
				);

				// Handle necessary time to park a car before taking public transports
				if(_direction == algorithm::DEPARTURE_TO_ARRIVAL && (it + 1 == path.end()) && (_accessParameters.getUserClass() == USER_CAR))
				{
					completeService.setArrivalInformations(
						*(completeService.getArrivalEdge()),
						completeService.getArrivalDateTime() + posix_time::minutes(4),
						completeService.getTheoreticalArrivalDateTime() + posix_time::minutes(4),
						*(completeService.getRealTimeArrivalVertex())
					);
				}

				// And adding the new service to the journey
				if(_direction == algorithm::DEPARTURE_TO_ARRIVAL)
				{
					result.append(completeService);
					departure = completeService.getArrivalDateTime();
				}
				else
				{
					result.prepend(completeService);
					departure = completeService.getDepartureDateTime();
				}
			}

			return result;
		}
	}
}
