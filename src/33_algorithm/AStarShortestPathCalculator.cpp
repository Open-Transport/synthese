
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
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/linearref/LengthIndexedLine.h>
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
using namespace geos::linearref;
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

			VertexAccessMap startingVertices = _departurePlace->getVertexAccessMap(_accessParameters, RoadModule::GRAPH_ID, 0);
			VertexAccessMap endingVertices = _arrivalPlace->getVertexAccessMap(_accessParameters, RoadModule::GRAPH_ID, 0);

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
				heuristicReference,
				false
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
			const boost::shared_ptr<Point> heuristicReference,
			const bool findAllAccessibleVertices
		) const {
			boost::shared_ptr<AStarNode> resultNode;
			AccessParameters ap = _accessParameters;

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
					// Approach mode, don't search any further than the shortest path to the node
					if(findAllAccessibleVertices)
						ap.setMaxApproachDistance(curNode->getDistance());
					else
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
						if(!chunk || !chunk->isCompatibleWith(ap))
							continue;

						// Specific car user class verification (turn restriction)
						if(ap.getUserClass() == USER_CAR && curNode->getLink())
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
						{
							CoordinateSequence* coordinates = chunkGeom->getCoordinates();
							distance = CGAlgorithms::length(coordinates);
							delete coordinates;
						}

						double speed(ap.getApproachSpeed());
						if(ap.getUserClass() == USER_CAR && linkChunk->getCarSpeed() > 0)
						{
							speed = linkChunk->getCarSpeed();
						}

						int newScore = curNode->getRealCost() + static_cast<int>(distance / speed);
						int heuristic = _getHeuristicScore(nextCrossing, heuristicReference);
						double newDistance = curNode->getDistance() + distance;

						// Check if compatible with max approach distance and max approach time (especially usefull to find close physical stops)
						if(!ap.isCompatibleWithApproach(newDistance, boost::posix_time::seconds(newScore)))
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
				heuristicReference,
				true
			);

			typedef std::set<boost::shared_ptr<AStarNode>, AStarNodeComparator> OrderedNodes;
			OrderedNodes nodes;

			BOOST_FOREACH(const NodeMap::value_type& node, nodeMap)
			{
				nodes.insert(node.second);
			}

			BOOST_FOREACH(const OrderedNodes::value_type& node, nodes)
			{
				VertexAccessMap crossingVAM;
				node->getCrossing()->getVertexAccessMap(crossingVAM, pt::PTModule::GRAPH_ID, *node->getCrossing(), false);

				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& currentVertex, crossingVAM.getMap())
				{
					const pt::StopPoint* stop = static_cast<const pt::StopPoint*>(currentVertex.first);
					if(foundStops.find(stop) == foundStops.end())
					{
						foundStops.insert(stop);
						Journey approachJourney = _generateJourneyFromNode(stop, node);
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



		boost::shared_ptr<geos::geom::LineString> AStarShortestPathCalculator::_computeGeometryExtremity(
			const Address* startAddress,
			const Address* endAddress,
			const RoadChunk* chunk,
			bool forwardMode,
			ResultPath::iterator insertPosition,
			ResultPath& path
		) const {
			MainRoadChunk* customChunk = NULL;
			double customOffset = 0;
			boost::shared_ptr<LineString> resultGeometry;
			bool useReverseChunk(false);

			if(startAddress)
			{
				customChunk = startAddress->getRoadChunk();
				customOffset = startAddress->getMetricOffset();
			}

			if(customChunk &&
				(!chunk ||
					(chunk->getKey() != customChunk->getKey() &&
					(!chunk->isReversed() || static_cast<const ReverseRoadChunk*>(chunk->getNext())->getMainRoadChunk()->getKey() != customChunk->getKey())
			)))
			{
				LengthIndexedLine customChunkGeometry(
					static_cast<Geometry*>(customChunk->getGeometry().get())
				);
				double chunkLength = customChunk->getGeometry()->getLength();
				double geometryOffset = customOffset - customChunk->getMetricOffset();

				if(chunkLength > 0)
				{
					if(!chunk && endAddress)
					{
						chunk = endAddress->getRoadChunk();
						double chunkOffset = endAddress->getMetricOffset();

						if(chunk->getKey() == customChunk->getKey())
						{
							resultGeometry = boost::shared_ptr<LineString>(
								static_cast<LineString*>(customChunkGeometry.extractLine(geometryOffset, chunkOffset - chunk->getMetricOffset())
							));

							if(customOffset > chunkOffset)
							{
								useReverseChunk = true;
							}
						}
						else if(chunk->getFromVertex()->getKey() == customChunk->getFromVertex()->getKey() && _direction == ARRIVAL_TO_DEPARTURE)
						{
							chunk = static_cast<const MainRoadChunk*>(chunk->getNext())->getReverseRoadChunk();
						}
					}

					if(!resultGeometry && chunk)
					{
						if(forwardMode)
						{
							// <----proj----o----first----->
							if(customChunk->getFromVertex()->getKey() == chunk->getFromVertex()->getKey())
							{
								resultGeometry = boost::shared_ptr<LineString>(
									static_cast<LineString*>(customChunkGeometry.extractLine(geometryOffset, 0)
								));
								useReverseChunk = true;
							}
							// o----proj---->o----first----->
							else if(customChunk->getNext()->getFromVertex()->getKey() == chunk->getFromVertex()->getKey())
							{
								resultGeometry = boost::shared_ptr<LineString>(
									static_cast<LineString*>(customChunkGeometry.extractLine(geometryOffset, chunkLength)
								));
							}
						}
						else
						{
							// o----first----->o-----proj---->
							if(customChunk->getFromVertex()->getKey() == chunk->getNext()->getFromVertex()->getKey())
							{
								resultGeometry = boost::shared_ptr<LineString>(
									static_cast<LineString*>(customChunkGeometry.extractLine(0, geometryOffset)
								));
							}
							// o----first-----><-----proj----o
							else if(customChunk->getNext()->getFromVertex()->getKey() == chunk->getNext()->getFromVertex()->getKey())
							{
								resultGeometry = boost::shared_ptr<LineString>(
									static_cast<LineString*>(customChunkGeometry.extractLine(chunkLength, geometryOffset)
								));
								useReverseChunk = true;
							}
						}
					}

					path.insert(
						insertPosition,
						(useReverseChunk ?
							static_cast<RoadChunk*>(static_cast<const MainRoadChunk*>(customChunk->getNext())->getReverseRoadChunk()) :
							static_cast<RoadChunk*>(customChunk))
					);
				}
			}

			return resultGeometry;
		}



		Journey AStarShortestPathCalculator::_generateJourneyFromNode(
			const pt::StopPoint* arrival,
			const boost::shared_ptr<AStarNode> lastNode
		) const {
			// Reconstructing a SYNTHESE Journey from a vector of edges returned by _findShortestPath
			Journey result;

			ResultPath path;
			_reconstructPath(path, lastNode);

			/* SYNTHESE doesn't know how to build each extremity of the geometry
				representing chunks taken by the origin vam et departure vam.
				This issue is visible in some cases when drawing the full path
				of a journey. We're adding a departureChunk and an arrivalChunk when needed
				in the code below.

				In order to have the right geometry we've added a custom geometry on a ServicePointer.
				If it's defined, ServicePointer::getGeometry will return it, and ServicePointer::getDistance
				will return ServicePointer::getGeometry->getLength(). This way it's totally transparent for
				classes above.

				Geometries are build based on offsets of places and the right chunk is added by looking at
				the first vertex of the path, places and stops are indeed always projected on a MainRoadChunk,
				but we something want the reverse one. There is 4 cases :
				- For the departure :
					- If we want the main road chunk,
					- If we want the reverse one,
				- For the arrival :
					- If we want the main road chunk,
					- If we want the reverse one
				In ARRIVAL_TO_DEPARTURE mode, departure and arrival cases are reversed. The code is pretty self
				explanatory above in _computeGeometryExtremity.
			*/
			const Place* startPlace = (_direction == DEPARTURE_TO_ARRIVAL ? _departurePlace : _arrivalPlace);
			boost::shared_ptr<LineString> startGeometry, endGeometry;

			startGeometry = _computeGeometryExtremity(
				dynamic_cast<const Address*>(startPlace),
				&arrival->getProjectedPoint(),
				(path.size() == 0 ? NULL : *(path.begin())),
				_direction == algorithm::DEPARTURE_TO_ARRIVAL,
				path.begin(),
				path
			);

			endGeometry = _computeGeometryExtremity(
				&arrival->getProjectedPoint(),
				dynamic_cast<const Address*>(startPlace),
				(path.size() == 0 ? NULL : *(path.rbegin())),
				!(_direction == algorithm::DEPARTURE_TO_ARRIVAL),
				path.end(),
				path
			);

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
				if(_direction == algorithm::ARRIVAL_TO_DEPARTURE && startChunk->getNext()) // Here, we want the last edge
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
				CoordinateSequence* customSequence(CoordinatesSystem::GetDefaultGeometryFactory().getCoordinateSequenceFactory()->create(0, 2));

				do {
					boost::shared_ptr<LineString> currentGeometry((*it)->getRealGeometry());

					if(_direction == DEPARTURE_TO_ARRIVAL)
					{
						CoordinateSequence* cs;
						if(startGeometry && it == path.begin())
						{
							cs = startGeometry->getCoordinates();
						}
						else if(endGeometry && (it + 1) == path.end())
						{
							cs = endGeometry->getCoordinates();
						}
						else
						{
							cs = currentGeometry->getCoordinates();
						}
						customSequence->add(cs, false, true);

						delete cs;
					}
					else
					{
						CoordinateSequence* cs;
						if(startGeometry && it == path.begin())
						{
							cs = startGeometry->getCoordinates();
						}
						else if(endGeometry && (it + 1) == path.end())
						{
							cs = endGeometry->getCoordinates();
						}
						else
						{
							cs = currentGeometry->getCoordinates();
						}
						cs->add(customSequence, false, true);

						delete customSequence;
						customSequence = cs;
					}

					it++;
				}
				while(
					it != path.end() &&
					currentPath == (*it)->getParentPath() &&
					(_direction == algorithm::DEPARTURE_TO_ARRIVAL ? startMetricOffset < (*it)->getMetricOffset() : startMetricOffset > (*it)->getMetricOffset())
				);
				it--;

				boost::shared_ptr<LineString> customGeometry;
				if(customSequence->size() > 1)
				{
					customGeometry = boost::shared_ptr<LineString>(
						CoordinatesSystem::GetDefaultGeometryFactory().createLineString(customSequence->clone())
					);
				}
				delete customSequence;

				// Retrieving the last edge of the path if we are on the common direction
				const RoadChunk* endChunk = *it;
				if(_direction == algorithm::DEPARTURE_TO_ARRIVAL && endChunk->getNext())
				{
					endChunk = static_cast<const RoadChunk*>(endChunk->getNext());
				}

				ServicePointer completeService(
					service,
					*endChunk,
					_accessParameters
				);

				boost::shared_ptr<LineString> originGeometry = completeService.getGeometry();
				if(customGeometry && customGeometry->getLength() != originGeometry->getLength())
				{
					completeService.setCustomGeometry(customGeometry);
					posix_time::time_duration durationDifference = posix_time::seconds(
						ceil((originGeometry->getLength() - customGeometry->getLength()) / _accessParameters.getApproachSpeed())
					);

					if(_direction == DEPARTURE_TO_ARRIVAL)
					{
						completeService.setArrivalInformations(
							*(completeService.getArrivalEdge()),
							completeService.getArrivalDateTime() - durationDifference,
							completeService.getTheoreticalArrivalDateTime() - durationDifference,
							*(completeService.getRealTimeArrivalVertex())
						);
					}
					else
					{
						completeService.setDepartureInformations(
							*(completeService.getDepartureEdge()),
							completeService.getDepartureDateTime() + durationDifference,
							completeService.getTheoreticalDepartureDateTime() + durationDifference,
							*(completeService.getRealTimeDepartureVertex())
						);
					}
				}

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
