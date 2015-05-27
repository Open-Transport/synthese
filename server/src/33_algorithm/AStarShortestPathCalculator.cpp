
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
#include "RoadChunkEdge.hpp"
#include "RoadPath.hpp"
#include "NamedPlace.h"
#include "Place.h"
#include "PTModule.h"
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
			_direction(direction),
			_startingVertices(VertexAccessMap()),
			_endingVertices(VertexAccessMap())
		{
		}


		AStarShortestPathCalculator::AStarShortestPathCalculator(
			const VertexAccessMap& startingVertices,
			const VertexAccessMap& endingVertices,
			const posix_time::ptime& departureTime,
			const AccessParameters accessParameters,
			const algorithm::PlanningPhase direction
		):	_departurePlace(NULL),
			_arrivalPlace(NULL),
			_departureTime(departureTime),
			_accessParameters(accessParameters),
			_direction(direction),
			_startingVertices(startingVertices),
			_endingVertices(endingVertices)
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
			_direction(direction),
			_startingVertices(VertexAccessMap()),
			_endingVertices(VertexAccessMap())
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

			if((!_departurePlace || !_arrivalPlace) &&
				(_startingVertices.getMap().empty() || _endingVertices.getMap().empty()))
			{
				return result;
			}

			VertexAccessMap startingVertices, endingVertices;

			if (_departurePlace)
			{
				startingVertices = _departurePlace->getVertexAccessMap(_accessParameters, RoadModule::GRAPH_ID, 0);
			}
			else
			{
				startingVertices = _startingVertices;
			}
			if (_arrivalPlace)
			{
				endingVertices = _arrivalPlace->getVertexAccessMap(_accessParameters, RoadModule::GRAPH_ID, 0);
			}
			else
			{
				endingVertices= _endingVertices;
			}

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
						const RoadChunkEdge* chunk = static_cast<const RoadChunkEdge*>(itEdges.second);

						// Check if the edge exists and is authorized for the user class
						if(!chunk || !chunk->isCompatibleWith(ap))
							continue;

						// Specific car user class verification (turn restriction)
						if(ap.getUserClass() == USER_CAR && curNode->getLink())
						{
							const RoadPath* from = static_cast<const RoadPath*>(curNode->getLink()->getParentPath());
							const RoadPath* to = static_cast<const RoadPath*>(path);

							if((_direction == algorithm::DEPARTURE_TO_ARRIVAL) && curNode->getCrossing()->isNonReachableRoad(from->getRoad(), to->getRoad()))
								continue;
							else if((_direction == algorithm::ARRIVAL_TO_DEPARTURE) && curNode->getCrossing()->isNonReachableRoad(to->getRoad(), from->getRoad()))
								continue;
						}

						// Retrieving the next or the previous chunk in the path, if there is one
						RoadChunkEdge* nextChunkInPath = static_cast<RoadChunkEdge*>(_direction == algorithm::ARRIVAL_TO_DEPARTURE ? chunk->getPrevious() : chunk->getNext());
						if(!nextChunkInPath)
							continue;

						// Retrieving the associated crossing
						Crossing* nextCrossing = static_cast<Crossing*>(nextChunkInPath->getFromVertex());

						// Check if it is already visited
						NodeMap::iterator nextNode = nodeMap.find(nextCrossing->getKey());
						if(nextNode != nodeMap.end() && nextNode->second->isVisited() &&
							nextNode->second->getParent())
							continue;

						const RoadChunkEdge* linkChunk;
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
						else if(nextNode->second->getRealCost() > newScore ||
							!nextNode->second->getParent()
						)
						{
							if (!curNode->getParent() ||
								curNode->getParent() != nextNode->second)
							{
								nextNode->second->setParent(curNode);
							}
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

			if(originPoint.get() && destination.get())
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
				if (node.second->getLink())
				{
					nodes.insert(node.second);
				}
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
			if (!curNode->getParent())
			{
				result.insert(result.begin(), curNode->getLink());
			}
			std::vector<util::RegistryKeyType> vectKey;
			bool alreadyVisited(false);
			while(curNode->getParent() && curNode != curNode->getParent() && !alreadyVisited)
			{
				result.insert(result.begin(), curNode->getLink());
				vectKey.push_back(curNode->getCrossing()->getKey());
				curNode = curNode->getParent();
				BOOST_FOREACH(const util::RegistryKeyType key, vectKey)
				{
					if (key == curNode->getCrossing()->getKey())
					{
						alreadyVisited = true;
						break;
					}
				}
			}
		}



		boost::shared_ptr<geos::geom::LineString> AStarShortestPathCalculator::_computeGeometryExtremity(
			const Address* startAddress,
			const Address* endAddress,
			const RoadChunkEdge* chunk,
			bool forwardMode,
			ResultPath::iterator insertPosition,
			ResultPath& path
		) const {
			RoadChunkEdge* customChunk = NULL;
			double customOffset = 0;
			boost::shared_ptr<LineString> resultGeometry;
			bool useReverseChunk(false);

			// Retrieve chunk and offset of the departure
			if(startAddress)
			{
				customChunk = &(startAddress->getRoadChunk()->getForwardEdge());
				customOffset = startAddress->getMetricOffset();
			}

			// If we have a start chunk and if we have :
			//  - No first chunk of a path returned by A* (empty path, only chunks from the VAM, 1 or 2)
			//	- Or the first chunk of the path isn't the start chunk nor the reverse chunk of the start chunk (don't generate start geometry two times in case of one chunk geometry)
			if(customChunk &&
				(!chunk ||
					(chunk->isReversed() ?
						chunk->getRoadChunk()->getKey() != static_cast<const RoadChunkEdge*>(customChunk->getNext())->getRoadChunk()->getKey() :
						chunk->getRoadChunk()->getKey() != customChunk->getRoadChunk()->getKey()
					)
				)
			){
				LengthIndexedLine customChunkGeometry(
					static_cast<Geometry*>(customChunk->getRoadChunk()->getGeometry().get())
				);
				double chunkLength = customChunk->getRoadChunk()->getGeometry()->getLength();
				double geometryOffset = customOffset;

				// If the start chunk length is not null (otherwise don't need to insert anything)
				if(chunkLength > 0)
				{
					// If we don't have any chunk from A* and there is an arrival address
					if(!chunk && endAddress)
					{
						// Get the end address chunk as the first chunk of the path (it will be the last too but it's not important)
						chunk = &(endAddress->getRoadChunk()->getForwardEdge());
						double chunkOffset = endAddress->getMetricOffset();

						// If the two chunks are the same : case of an address and a stop projected on the same chunk
						if(chunk->getRoadChunk()->getKey() == customChunk->getRoadChunk()->getKey() && chunk->isReversed() == customChunk->isReversed())
						{
							// Build a custom geometry according to their offsets
							resultGeometry = boost::shared_ptr<LineString>(
								static_cast<LineString*>(customChunkGeometry.extractLine(geometryOffset, chunkOffset)
							));

							// If the offset of the start is bigger than the one from the end, use the reverse chunk
							if(customOffset + customChunk->getMetricOffset() > chunkOffset)
							{
								useReverseChunk = true;
							}
						}
						/*
						 * Otherwise there is only 2 chunks in the approach. WE NEED TO BE SURE THAT THE SECOND CHUNK TO THE ARRIVAL IS THE RIGHT ONE (reverse / main)
						 * to ensure that the code below will work as intended.
						 * There is multiple cases, we have start (s) and end (e) :
						 *		1) <----(s)---o----(e)--->
						 *		2) o---(s)--->o----(e)--->
						 *		3) <----(s)---o<---(e)---o
						 *		4) o----(s)---><---(e)---o
						 *
						 * In DEPARTURE_TO_ARRIVAL direction we are going from the start to the end. As we can see, 1 and 2 are correct, the chunk (e) is in the right way.
						 * For 3 and 4, we need to take the reverse chunk of (e).
						 * In ARRIVAL_TO_DEPARTURE 3 and 4 are correct and 1 and 2 are not.
						 */
						else if(_direction == DEPARTURE_TO_ARRIVAL)
						{
							// If the next chunk of (e) has the same vertex as the chunk (s) (case 3) or the next chunk of (s) (case 4)
							if(chunk->getNext()->getFromVertex()->getKey() == customChunk->getFromVertex()->getKey() ||
							   chunk->getNext()->getFromVertex()->getKey() == customChunk->getNext()->getFromVertex()->getKey()
							)
							{
								chunk = &(static_cast<const RoadChunkEdge*>(chunk->getNext())->getRoadChunk()->getReverseEdge());
							}
						}
						else if(_direction == ARRIVAL_TO_DEPARTURE)
						{
							// If the chunk (e) has the same vertex as the chunk (s) (case 1) or the next chunk of (s) (case 2)
							if(chunk->getFromVertex()->getKey() == customChunk->getFromVertex()->getKey() ||
							   chunk->getFromVertex()->getKey() == customChunk->getNext()->getFromVertex()->getKey()
							)
							{
								chunk = &(static_cast<const RoadChunkEdge*>(chunk->getNext())->getRoadChunk()->getReverseEdge());
							}
						}
					}

					// If we don't have a geometry (we build it ourselve if customChunk and chunk are the same) and we have a first chunk in the path
					// We know that chunk is in the right direction, A* create a path in the right order and in case of a two chunks geometry the code above did the job.
					// We build the correct geometry for customChunk and reverse it if necessary.
					if(!resultGeometry && chunk)
					{
						// In forwardMode -> DEPARTURE_TO_ARRIVAL approach from the start_place to the first crossing
						//				  -> ARRIVAL_TO_DEPARTURE approach to the stop from the last crossing
						if(forwardMode)
						{
							// proj share the same vertex than first, use the reverse chunk and build geometry from our offset to the start
							// <----proj----o----first----->
							if(customChunk->getFromVertex()->getKey() == chunk->getFromVertex()->getKey())
							{
								resultGeometry = boost::shared_ptr<LineString>(
									static_cast<LineString*>(customChunkGeometry.extractLine(geometryOffset, 0)
								));
								useReverseChunk = true;
							}
							// proj and first are already in place, build the geometry from our offset to the end
							// o----proj---->o----first----->
							else if(customChunk->getNext()->getFromVertex()->getKey() == chunk->getFromVertex()->getKey())
							{
								resultGeometry = boost::shared_ptr<LineString>(
									static_cast<LineString*>(customChunkGeometry.extractLine(geometryOffset, chunkLength)
								));
							}
						}
						// In backwardMode -> DEPARTURE_TO_ARRIVAL approach to the stop from the last crossing
						//				   -> ARRIVAL_TO_DEPARTURE approach from the start_place to the first crossing
						else
						{
							// proj and first are already in place, build the geometry from the start to our offset
							// o----first----->o-----proj---->
							if(customChunk->getFromVertex()->getKey() == chunk->getNext()->getFromVertex()->getKey())
							{
								resultGeometry = boost::shared_ptr<LineString>(
									static_cast<LineString*>(customChunkGeometry.extractLine(0, geometryOffset)
								));
							}
							// next chunk of proj share the same vertex than the next chunk of first, use the reverse chunk and build geometry from the end to our offset
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

					if(resultGeometry && !resultGeometry->isEmpty())
					{
						path.insert(
							insertPosition,
							(useReverseChunk ?
								&(static_cast<const RoadChunkEdge*>(customChunk->getNext())->getRoadChunk()->getReverseEdge()) :
								customChunk)
						);
					}
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
				representing chunks taken by the origin vam and departure vam.
				This issue is visible in some cases when drawing the full path
				of a journey. We're adding a departureChunk and an arrivalChunk when needed
				in the code below.

				In order to have the right geometry we've added a custom geometry on a ServicePointer.
				If it's defined, ServicePointer::getGeometry will return it, and ServicePointer::getDistance
				will return ServicePointer::getGeometry->getLength(). This way it's totally transparent for
				classes above.

				Geometries are built based on offsets of places and the right chunk is added by looking at
				the first vertex of the path, places and stops are indeed always projected on a MainRoadChunk,
				but we something want the reverse one
			*/
			const Place* startPlace = (_direction == DEPARTURE_TO_ARRIVAL ? _departurePlace : _arrivalPlace);
			boost::shared_ptr<LineString> startGeometry, endGeometry;
			const Address* startAddress = NULL;

			if(dynamic_cast<const Address*>(startPlace))
			{
				startAddress = dynamic_cast<const Address*>(startPlace);
			}

			startGeometry = _computeGeometryExtremity(
				startAddress,
				&arrival->getProjectedPoint(),
				(path.size() == 0 ? NULL : *(path.begin())),
				_direction == algorithm::DEPARTURE_TO_ARRIVAL,
				path.begin(),
				path
			);

			endGeometry = _computeGeometryExtremity(
				&arrival->getProjectedPoint(),
				startAddress,
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
				
				const RoadChunkEdge* startChunk = *it;
				
				if (startChunk)
				{

					// A road is supposed to have one and only one service collection
					const ChronologicalServicesCollection& collection(**startChunk->getParentPath()->getServiceCollections().begin());
		
					/*
						Retrieve the first, or last, edge of the path.
						There is a little difference between SYNTHESE's representation of a path and the vector we have.
						
						o------------> o---------------> o // This is internal SYNTHESE's reprensentation, 3 edges, the last one just carry the information of the end crossing

						In the result vector, we just have :

						o------------> o--------------->

						And the end crossing will be the crossing of the next edge in the vector.
					*/
					if(_direction == algorithm::ARRIVAL_TO_DEPARTURE) // Here, we want the last edge
					{
						startChunk = static_cast<const RoadChunkEdge*>(startChunk->getNext());
					}

					ServicePointer service(
						(_direction == algorithm::DEPARTURE_TO_ARRIVAL) ?
						startChunk->getNextService(
							collection,
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
							collection,
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
						// Build custom geometry for the service
						boost::shared_ptr<LineString> currentGeometry((*it)->getRealGeometry());

						// In DEPARTURE_TO_ARRIVAL push new geometry at the back of the coordinate sequence
						if(_direction == DEPARTURE_TO_ARRIVAL)
						{
							CoordinateSequence* cs;
							// use startGeometry if it exists and we are at the beginning of the path
							if(startGeometry && !startGeometry->isEmpty() && it == path.begin())
							{
								cs = startGeometry->getCoordinates();
							}
							// use endGeometry if it exists and we are at the end of the path
							else if(endGeometry && !endGeometry->isEmpty() && (it + 1) == path.end())
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
						// In ARRIVAL_TO_DEPARTURE push new geometry at the front of the coordinate sequence
						else
						{
							CoordinateSequence* cs;
							if(startGeometry && !startGeometry->isEmpty() && it == path.begin())
							{
								cs = startGeometry->getCoordinates();
							}
							else if(endGeometry && !endGeometry->isEmpty() && (it + 1) == path.end())
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

					// Create custom LineString if necessary
					boost::shared_ptr<LineString> customGeometry;
					if(customSequence->size() > 1)
					{
						customGeometry = boost::shared_ptr<LineString>(
							CoordinatesSystem::GetDefaultGeometryFactory().createLineString(customSequence->clone())
						);
					}
					delete customSequence;

					// Retrieving the last edge of the path if we are on the common direction
					const RoadChunkEdge* endChunk = *it;
					if(_direction == algorithm::DEPARTURE_TO_ARRIVAL)
					{
						endChunk = static_cast<const RoadChunkEdge*>(endChunk->getNext());
					}

					ServicePointer completeService(
						service,
						*endChunk,
						_accessParameters
					);

					// If we have a custom geometry and it's different from the original geometry
					boost::shared_ptr<LineString> originGeometry = completeService.getGeometry();
					if(customGeometry && customGeometry->getLength() != originGeometry->getLength())
					{
						// Set geometry and compute the time difference between the two length
						completeService.setCustomGeometry(customGeometry);
						posix_time::time_duration durationDifference = posix_time::seconds(
							ceil((originGeometry->getLength() - customGeometry->getLength()) / _accessParameters.getApproachSpeed())
						);

						// Set departure / arrival times according to that difference
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
			}

			return result;
		}
	}
}
