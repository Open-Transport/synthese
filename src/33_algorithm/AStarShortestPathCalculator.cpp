
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
#include <geos/linearref/LengthIndexedLine.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>
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
			bool addGeometry = false;
			boost::shared_ptr<LineString> newGeometry;
			posix_time::time_duration newDuration;

			ResultPath path;
			_reconstructPath(path, lastNode);

			/* SYNTHESE doesn't know how to build the geometries between the:
				- departure projected object and first chunk in the ResultPath
				- arrival projected object and last chunk in the ResultPath
				This issue is visible in some cases when drawing the full path
				of a journey. This part of code manage this lack by adding a
				departureChunk and a arrivalChunk when needed.

				When the projected place has an offset in the added roadchunk, it can be
				possible that we don't need a full length roadchunk to be added.
				In this case, adding a custom new MainRoadChunk instead of the existing one
				is too risky and may create memory issues. To bypass this problem, the code builds
				a custom geometry using the projected place offset and the chunk associated
				to it. Then, the custom geometry is added in the Journey and will be used
				by RoutePlannerFunction if present, to draw correctly the way by replacing
				the added departure/arrival chunk geometries with it. It will manage the distance too
				by using the custom geometries length directly before displaying result.
				Timing is also managed by recalculating it with the custom geometry length and replacing
				the existing one in the service associated to the chunks.

				Example :

					Departure geometries needed to draw the way between departure
					object and first roadchunk we have :
						- add the roadchunk associated to the projected departure object in the begin of the ResultPath

					Custom offset here, don't need full length chunk geometries
					because our point is not at the begin/end of the chunk but somewhere in it :
						- add a custom linestring to the associated Journey

					When displaying the journey, RoutePlannerFunction has only access to the journey object,
					so it will check if something like custom geometries were added to it and will use them if founded.
			*/

			// This first part of code manage to build the custom geometry and add
			// the associated roadchunk to the ResultPath
			if (
				(_direction == DEPARTURE_TO_ARRIVAL && _departurePlace) ||
				(_direction == ARRIVAL_TO_DEPARTURE && _arrivalPlace)
			){
				if (!path.empty() && dynamic_cast<const RoadChunk*>(*(path.begin())))
				{
					RoadChunk* firstChunk(const_cast<RoadChunk*>(*(path.begin())));
					const geography::Place* thePlace;

					// Get the correct place to check, looking at the direction
					if (_direction == DEPARTURE_TO_ARRIVAL)
					{
						thePlace = _departurePlace;
					}
					else
					{
						thePlace = _arrivalPlace;
					}

					if (dynamic_cast<const road::Address*>(thePlace))
					{
						const road::Address* theAddress(dynamic_cast<const road::Address*>(thePlace));
						LengthIndexedLine addressChunkGeometry(static_cast<Geometry*>((theAddress->getRoadChunk()->getGeometry()).get()));
						bool reversed = false;

						// Build the custom geometry with correct direction
						if (_direction == DEPARTURE_TO_ARRIVAL)
						{
							if (theAddress->getRoadChunk()->getFromVertex()->getKey() == firstChunk->getFromVertex()->getKey())
							{
								newGeometry = boost::shared_ptr<LineString>(static_cast<LineString*>(addressChunkGeometry.extractLine(addressChunkGeometry.indexOf(*(theAddress->getGeometry()->getCoordinate())),0)));
								reversed = true;
							}
							else if (theAddress->getRoadChunk()->getNext()->getFromVertex()->getKey() == firstChunk->getFromVertex()->getKey())
							{
								newGeometry = boost::shared_ptr<LineString>(static_cast<LineString*>(addressChunkGeometry.extractLine(addressChunkGeometry.indexOf(*(theAddress->getGeometry()->getCoordinate())),theAddress->getRoadChunk()->getGeometry()->getLength())));
							}
						}
						else
						{
							if (theAddress->getRoadChunk()->getFromVertex()->getKey() == firstChunk->getNext()->getFromVertex()->getKey())
							{
								newGeometry = boost::shared_ptr<LineString>(static_cast<LineString*>(addressChunkGeometry.extractLine(0,addressChunkGeometry.indexOf(*(theAddress->getGeometry()->getCoordinate())))));
							}
							else if (theAddress->getRoadChunk()->getNext()->getFromVertex()->getKey() == firstChunk->getNext()->getFromVertex()->getKey())
							{
								newGeometry = boost::shared_ptr<LineString>(static_cast<LineString*>(addressChunkGeometry.extractLine(theAddress->getRoadChunk()->getGeometry()->getLength(),addressChunkGeometry.indexOf(*(theAddress->getGeometry()->getCoordinate())))));
								reversed = true;
							}
						}

						// Add the associated roadchunk to the path
						if (newGeometry && newGeometry->getLength() > 0)
						{
							newDuration = posix_time::seconds(ceil((theAddress->getRoadChunk()->getGeometry()->getLength() - newGeometry->getLength()) / _accessParameters.getApproachSpeed()));
							addGeometry = true;
							if (reversed)
							{
								MainRoadChunk* reverseChunk(static_cast<MainRoadChunk*>(theAddress->getRoadChunk()->getNext()));
								path.insert(path.begin(),reverseChunk->getReverseRoadChunk());
							}
							else
							{
								path.insert(path.begin(),theAddress->getRoadChunk());
							}
						}
					}
				}
			}

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

				// Before merging roadchunks belonging to the same path, check we need to merge
				// a custom geometry too with the other roadchunks' geometries
				bool customizeGeometries = false;
				if (addGeometry && it == path.begin() && (*(it+1))->getParentPath() == currentPath)
				{
					customizeGeometries = true;
				}

				do {
					it++;
					// Merge geometries with the custom one
					if(it != path.end() && customizeGeometries && (*it)->getParentPath() == currentPath)
					{
						boost::shared_ptr<LineString> currentGeometry((*it)->getRealGeometry());
						boost::shared_ptr<CoordinateSequence> cs;
						CoordinateSequence* tempCs;

						if (_direction == DEPARTURE_TO_ARRIVAL)
						{
							cs = boost::shared_ptr<CoordinateSequence>(newGeometry->getCoordinates());
							tempCs = currentGeometry->getCoordinates();
							cs->add(tempCs,false,true);
						}
						else
						{
							cs = boost::shared_ptr<CoordinateSequence>(currentGeometry->getCoordinates());
							tempCs = newGeometry->getCoordinates();
							cs->add(tempCs,false,true);
						}
						newGeometry = boost::shared_ptr<LineString>(CoordinatesSystem::GetDefaultGeometryFactory().createLineString(cs->clone()));
						delete tempCs;
					}
				}
				while(it != path.end() && currentPath == (*it)->getParentPath() && (_direction == algorithm::DEPARTURE_TO_ARRIVAL ? startMetricOffset < (*it)->getMetricOffset() : startMetricOffset > (*it)->getMetricOffset()));

				it--;

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

				// If a custom geometry has been created, must calculate the associated time
				if (customizeGeometries)
				{
					if (_direction == algorithm::DEPARTURE_TO_ARRIVAL)
					{
						completeService.setArrivalInformations(
							*(completeService.getArrivalEdge()),
							completeService.getArrivalDateTime() - newDuration,
							completeService.getTheoreticalArrivalDateTime() - newDuration,
							*(completeService.getRealTimeArrivalVertex())
						);
					}
					else
					{
						completeService.setDepartureInformations(
							*(completeService.getDepartureEdge()),
							completeService.getDepartureDateTime() + newDuration,
							completeService.getTheoreticalDepartureDateTime() + newDuration,
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

			// Adding custom geometries to the associated journey if needed
			if (addGeometry && newGeometry)
			{
				if (_direction == algorithm::DEPARTURE_TO_ARRIVAL)
					result.setDepartureGeometry(newGeometry);
				else
					result.setArrivalGeometry(newGeometry);
			}

			return result;
		}
	}
}
