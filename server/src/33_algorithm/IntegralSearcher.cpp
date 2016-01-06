/** IntegralSearcher class implementation.
	@file IntegralSearcher.cpp

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

#include "IntegralSearcher.h"

#include "AlgorithmLogger.hpp"
#include "BestVertexReachesMap.h"
#include "JourneysResult.h"
#include "Junction.hpp"
#include "RoadPath.hpp"
#include "RoadPlace.h"
#include "VertexAccessMap.h"
#include "Vertex.h"
#include "Edge.h"
#include "Path.h"
#include "Hub.h"
#include "PTUseRule.h" // TODO remove it
#include "RoadModule.h" // TODO remove it
#include "RoutePlanningIntermediateJourney.hpp"
#include "JourneyTemplates.h"
#include "Service.h"
#include "Log.h"
#include "Crossing.h"
#include "Junction.hpp"
#include "StopPoint.hpp"

#include <sstream>
#include <limits>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt; // TODO remove it
	using namespace road; // TODO remove it

	namespace algorithm
	{

// -----------------------------------------------------------------------1 Construction

		IntegralSearcher::IntegralSearcher(
			PlanningPhase accessDirection,
			const AccessParameters&	accessParameters,
			GraphIdType whatToSearch,
			bool searchOnlyNodes,
			GraphIdType graphToUse,
			JourneysResult&	result,
			BestVertexReachesMap& bestVertexReachesMap,
			const VertexAccessMap& destinationVam,
			const boost::posix_time::ptime&			originDateTime,
			const boost::posix_time::ptime&	minMaxDateTimeAtOrigin,
			boost::posix_time::ptime&	minMaxDateTimeAtDestination,
			bool inverted,
			bool optim,
			optional<posix_time::time_duration> maxDuration,
			double vmax,
			bool ignoreReservation,
			const AlgorithmLogger& logger,
			int totalDistance,
			boost::optional<const JourneyTemplates&> journeyTemplates,
			bool enableTheoretical,
			bool enableRealTime,
			UseRule::ReservationDelayType reservationRulesDelayType
		):	_accessParameters(accessParameters),
			_accessDirection(accessDirection),
			_whatToSearch(whatToSearch),
			_searchOnlyNodes(searchOnlyNodes),
			_graphToUse(graphToUse),
			_result(result),
			_bestVertexReachesMap(bestVertexReachesMap),
			_originDateTime(originDateTime),
			_minMaxDateTimeAtOrigin(minMaxDateTimeAtOrigin),
			_minMaxDateTimeAtDestination(minMaxDateTimeAtDestination),
			_inverted(inverted),
			_optim(optim),
			_logger(logger),
			_maxDuration(maxDuration),
			_vmax(vmax),
			_ignoreReservation(ignoreReservation),
			_enableTheoretical(enableTheoretical),
			_enableRealTime(enableRealTime),
			_reservationRulesDelayType(reservationRulesDelayType),
			_destinationVam(destinationVam),
			_totalDistance(totalDistance),
			_journeyTemplates(journeyTemplates)
		{}



		void IntegralSearcher::integralSearch(
			const graph::VertexAccessMap& vertices,
			optional<size_t> maxDepth,
			boost::optional<boost::posix_time::time_duration> totalDuration
		){
			RoutePlanningIntermediateJourney emptyJourney(_accessDirection);
			_integralSearch(
				vertices,
				emptyJourney,
				_originDateTime,
				_minMaxDateTimeAtOrigin,
				maxDepth,
				totalDuration
			);
		}



		void IntegralSearcher::integralSearch(
			const RoutePlanningIntermediateJourney& startJourney,
			optional<std::size_t> maxDepth,
			boost::optional<boost::posix_time::time_duration> totalDuration,
			boost::optional<boost::posix_time::time_duration> maxTransferWaitingTime
		){
			VertexAccessMap vam;
			startJourney.getEndEdge().getHub()->getVertexAccessMap(
				vam,
				_graphToUse,
				*startJourney.getEndEdge().getFromVertex(),
				_accessDirection == DEPARTURE_TO_ARRIVAL
			);

			// Time at origin
			ptime desiredTime(startJourney.getEndTime(false));

			// Worse time at origin
			ptime minMaxDateTimeAtOrigin(_minMaxDateTimeAtDestination);
			if(maxTransferWaitingTime)
			{
				if(_accessDirection == DEPARTURE_TO_ARRIVAL)
				{
					ptime maxTransferTime(minMaxDateTimeAtOrigin = desiredTime + *maxTransferWaitingTime);
					if(maxTransferTime > _minMaxDateTimeAtDestination) // TODO introduce here max speed
					{
						minMaxDateTimeAtOrigin = maxTransferTime;
					}
				}
				else
				{
					ptime minTransferTime(minMaxDateTimeAtOrigin = desiredTime - *maxTransferWaitingTime);
					if(minTransferTime < _minMaxDateTimeAtDestination) // TODO introduce here max speed
					{
						minMaxDateTimeAtOrigin = minTransferTime;
					}
				}
			}

			_integralSearch(
				vam,
				startJourney,
				desiredTime,
				minMaxDateTimeAtOrigin,
				maxDepth,
				totalDuration
			);
		}



		void IntegralSearcher::_integralSearch(
			const graph::VertexAccessMap& vam,
			const RoutePlanningIntermediateJourney& startJourney,
			const boost::posix_time::ptime& desiredTime,
			const boost::posix_time::ptime& callMinMaxDateTimeAtOrigin,
			optional<size_t> maxDepth,
			boost::optional<boost::posix_time::time_duration> totalDuration
		){
			// Recursions to do
			JourneysResult todo(_originDateTime, _accessDirection);
			todo.addEmptyJourney();

			_logger.openIntegralSearchLog(
				_accessDirection,
				desiredTime
			);

			// The Loop
			while(!todo.empty())
			{
				this_thread::interruption_point();
				boost::shared_ptr<RoutePlanningIntermediateJourney> journey(todo.front());

				VertexAccessMap curVam;
				ptime minMaxDateTimeAtOrigin(callMinMaxDateTimeAtOrigin);
				if(journey->empty())
				{
					curVam = vam;
				}
				else
				{
					journey->getEndEdge().getHub()->getVertexAccessMap(
						curVam,
						_graphToUse,
						*journey->getEndEdge().getFromVertex(),
						_accessDirection == DEPARTURE_TO_ARRIVAL
					);
					if(_accessDirection == DEPARTURE_TO_ARRIVAL)
					{
						minMaxDateTimeAtOrigin += journey->getDuration();
					}
					else
					{
						minMaxDateTimeAtOrigin -= journey->getDuration();
					}
				}

				_logger.logIntegralSearchJourney(*journey);

				RoutePlanningIntermediateJourney currentJourney(
					_accessDirection == DEPARTURE_TO_ARRIVAL ? startJourney : *journey,
					_accessDirection == DEPARTURE_TO_ARRIVAL ? *journey : startJourney
				);

				// Loop on each origin vertex
				for(map<const Vertex*, VertexAccess>::const_iterator itVertex(curVam.getMap ().begin());
					itVertex != curVam.getMap().end();
					++itVertex
				){
					this_thread::interruption_point();

					// Initialization of loop local variables
					const Vertex* origin(itVertex->first);

					// Read only the vertices of the current graph
					if(origin->getGraphType() != _graphToUse)
						continue;

					// Checks if the vertex use rules are compliant with current user profile
					const UseRule& vertexUseRule(origin->getUseRule(_accessParameters.getUserClassRank()));
					if(	!vertexUseRule.isCompatibleWith(_accessParameters)
					){
						continue;
					}

					// Approach to the vertex
					RoutePlanningIntermediateJourney fullApproachJourney(currentJourney);
					if(fullApproachJourney.empty())
					{
						if(_accessDirection == DEPARTURE_TO_ARRIVAL)
						{
							fullApproachJourney.setStartApproachDuration(itVertex->second.approachTime);
						}
						else
						{
							fullApproachJourney.setEndApproachDuration(itVertex->second.approachTime);
						}
					}
					ptime correctedDesiredTime(journey->empty() ? desiredTime : journey->getEndTime());
					ptime correctedMinMaxDateTimeAtOrigin(minMaxDateTimeAtOrigin);
					if (_accessDirection == DEPARTURE_TO_ARRIVAL)
					{
						correctedDesiredTime += itVertex->second.approachTime;
						correctedMinMaxDateTimeAtOrigin += itVertex->second.approachTime;
					}
					else
					{
						correctedDesiredTime -= itVertex->second.approachTime;
						correctedMinMaxDateTimeAtOrigin -= itVertex->second.approachTime;
					}

					// Goal edges loop
					const Vertex::Edges& edges((_accessDirection == DEPARTURE_TO_ARRIVAL) ? origin->getDepartureEdges() : origin->getArrivalEdges());

					BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, edges)
					{
						// Getting the current path
						const Path& path(*itEdge.first);

						// Abort current path if the edge is at its end
						assert(itEdge.second);
						const Edge& edge(*itEdge.second);

						// If we are on a road journey planning
						if(_graphToUse == RoadModule::GRAPH_ID)
						{
							// Check if the edge is authorized for the user class
							if(!edge.isCompatibleWith(_accessParameters))
							{
								continue;
							}

							// Specific car user class verification (turn restriction)
							if(_accessParameters.getUserClass() == USER_CAR)
							{
								const Crossing* originCrossing = static_cast<const Crossing*>(origin);
								if(originCrossing && !currentJourney.getServiceUses().empty())
								{
									const RoadPath* from = static_cast<const RoadPath*>(currentJourney.getEndEdge().getParentPath());
									const RoadPath* to = static_cast<const RoadPath*>(&path);

									if((_accessDirection == DEPARTURE_TO_ARRIVAL) && originCrossing->isNonReachableRoad(from->getRoad(), to->getRoad()))
									{
										continue;
									}
									else if((_accessDirection == ARRIVAL_TO_DEPARTURE) && originCrossing->isNonReachableRoad(to->getRoad(), from->getRoad()))
									{
										continue;
									}
								}
							}
						}

						PtrEdgeStep fineStep(
							(_accessDirection == DEPARTURE_TO_ARRIVAL) ?
							(&Edge::getFollowingArrivalForFineSteppingOnly) :
							(&Edge::getPreviousDepartureForFineSteppingOnly)
						);
						if(!(edge.*fineStep)())
						{
							continue;
						}

						// Checks if the path properties are compliant with current filters
						if(	!path.isCompatibleWith(_accessParameters) ||
							!_accessParameters.isAllowedPathClass
							(
								path.getPathClass() ? path.getPathClass()->getIdentifier() : 0,
								path.getPathNetwork() ? path.getPathNetwork()->getIdentifier() : 0
							)
						){
							continue;
						}

						// Checks if the path use rules are compliant with current user profile
						const UseRule& useRule(path.getUseRule(_accessParameters.getUserClassRank()));
						if(	dynamic_cast<const PTUseRule*>(&useRule) &&
							static_cast<const PTUseRule&>(useRule).getForbiddenInJourneyPlanning()
						){
							continue;
						}

						// Getting the path traversal method
						bool needsFineStepping(
							!_searchOnlyNodes ||
							(	_accessDirection == DEPARTURE_TO_ARRIVAL ?
								_destinationVam.needFineSteppingForArrival(&path) :
								_destinationVam.needFineSteppingForDeparture(&path) // TODO Is it not originVAM ???
							)
						);
						PtrEdgeStep step(
							(_accessDirection == DEPARTURE_TO_ARRIVAL) ?
							(	needsFineStepping ?
								(&Edge::getFollowingArrivalForFineSteppingOnly) :
								(&Edge::getFollowingConnectionArrival)
							):(	needsFineStepping ?
								(&Edge::getPreviousDepartureForFineSteppingOnly) :
								(&Edge::getPreviousConnectionDeparture)
						)	);

						// If path is a junction, we verify that the origin vertex is the same
						// RULE-204
						const Junction* junction(dynamic_cast<const Junction*> (&path));
						if (junction != NULL)
						{
							if (!currentJourney.empty() &&
								origin->getKey() != currentJourney.getEndEdge().getFromVertex()->getKey())
								continue;
							// Junction should not follow a road path (it may exist a road approach to do the same, junction should always follow PT path)
							if (!currentJourney.empty() &&
								dynamic_cast<const RoadPath*>(currentJourney.getEndEdge().getParentPath()))
								continue;
							// Junction should not start or end a journey
							if (currentJourney.empty())
								continue;
						}
						if(!currentJourney.empty())
						{
							const Junction* currentJunction(dynamic_cast<const Junction*>(currentJourney.getEndEdge().getParentPath()));
							if(currentJunction != NULL &&
								(((_accessDirection == DEPARTURE_TO_ARRIVAL) ? currentJunction->getEnd()->getKey() : currentJunction->getStart()->getKey()) != origin->getKey()))
								continue;
						}
						const RoadPath* roadApproach(dynamic_cast<const RoadPath*> (&path));
						if (roadApproach != NULL && !currentJourney.empty())
						{
							// Junction should not follow a road path (it may exist a road approach to do the same, junction should always follow PT path)
							const Junction* currentJunction(dynamic_cast<const Junction*>(currentJourney.getEndEdge().getParentPath()));
							if(currentJunction != NULL)
								continue;
						}

						// Loop on services collections
						BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, path.getServiceCollections())
						{
							set<const Edge*> nonServedEdges;
							optional<Edge::DepartureServiceIndex::Value> departureServiceNumber;
							optional<Edge::ArrivalServiceIndex::Value> arrivalServiceNumber;
							ptime departureMoment(correctedDesiredTime);

							// Loop on services
						while(true)
						{
							this_thread::interruption_point();

							// Reach of the next/previous service serving the edge
							ServicePointer serviceInstance(
								(_accessDirection == DEPARTURE_TO_ARRIVAL) ?
								edge.getNextService(
										*itCollection,
									_accessParameters,
									departureMoment,
									correctedMinMaxDateTimeAtOrigin,
									true,
									departureServiceNumber,
									_inverted,
									_ignoreReservation,
									false, // allowCanceledService
									_enableTheoretical,
									_enableRealTime,
									_reservationRulesDelayType
								):
								edge.getPreviousService(
										*itCollection,
									_accessParameters,
									departureMoment,
									correctedMinMaxDateTimeAtOrigin,
									true,
									arrivalServiceNumber,
									_inverted,
									_ignoreReservation,
									false, // allowCanceledService
									_enableTheoretical,
									_enableRealTime,
									_reservationRulesDelayType
							)	);

							// If no service, advance to the next edge
							if (!serviceInstance.getService())
							{
								break;
							}

							if(_accessDirection == DEPARTURE_TO_ARRIVAL)
							{
								++*departureServiceNumber; // To the next service
								departureMoment = serviceInstance.getDepartureDateTime();
								if(_inverted)
								{
									departureMoment += serviceInstance.getServiceRange();
								}
							}
							else
							{
								++*arrivalServiceNumber; // To the previous service (reverse iterator increment)
								departureMoment = serviceInstance.getArrivalDateTime();
								if(_inverted)
								{
									departureMoment -= serviceInstance.getServiceRange();
								}
							}

							// Check for service compliance rules.
							if (!serviceInstance.getService()->isCompatibleWith(_accessParameters))
							{
								continue;
							}

							bool nonServedEdgesSearch(!nonServedEdges.empty());

							// The path is traversed
							for(const Edge* curEdge = (edge.*step)();
								curEdge != NULL;
								curEdge = (curEdge->*step)()
							){
								this_thread::interruption_point();

								// If the path traversal is only to find non served edges, analyze it only if
								// it belongs to the list
								if(nonServedEdgesSearch)
								{
									set<const Edge*>::iterator it(nonServedEdges.find(curEdge));
									if(it == nonServedEdges.end())
										continue;
									nonServedEdges.erase(it);
								}

								const Vertex* reachedVertex(curEdge->getFromVertex());

								// Checks if the vertex use rules are compliant with current user profile
								const UseRule& vertexUseRule(
									reachedVertex->getUseRule(_accessParameters.getUserClassRank())
								);
								if(	!vertexUseRule.isCompatibleWith(_accessParameters)
								){
									continue;
								}

								// The reached vertex is analyzed only in 3 cases :
								//  - if the vertex belongs to the goal
								//  - if the type of the vertex corresponds to the searched one (if
								//		the _searchOnlyNodes parameter is activated, the vertex must
								//		also belong to a connection place)
								//  - if the vertex belongs to a connection place
								bool isGoalReached(
									_destinationVam.contains(reachedVertex) &&
										(!_accessParameters.getMaxtransportConnectionsCount() ||
										 fullApproachJourney.size() < *_accessParameters.getMaxtransportConnectionsCount() + 1)
								);
								bool isReturnedVertex(
									(	reachedVertex->getHub()->containsAnyVertex(_whatToSearch) &&
										(	!_searchOnlyNodes ||
											(	reachedVertex->getHub()->isUsefulTransfer(_graphToUse) &&
												(	!_accessParameters.getMaxtransportConnectionsCount() ||
													fullApproachJourney.size() < *_accessParameters.getMaxtransportConnectionsCount() + 1
								)	)	)	)	);
								bool isARecursionNode(
									reachedVertex->getHub()->isUsefulTransfer(_graphToUse) &&
									(	!maxDepth || journey->size() < *maxDepth)
								);
								if(	!isGoalReached &&
									!isReturnedVertex &&
									!isARecursionNode
								){
									continue;
								}

								// Storage of the useful solution
								ServicePointer serviceUse(serviceInstance, *curEdge, _accessParameters);
								if (serviceUse.isUseRuleCompliant(_ignoreReservation, _reservationRulesDelayType) == UseRule::RUN_NOT_POSSIBLE)
								{
									nonServedEdges.insert(curEdge);
									continue;
								}


								// Result journey writing
								graph::Journey::Distance distanceToEnd(
									isGoalReached ?
									0 :
									(
										(_destinationVam.getCentroid().get() && reachedVertex->getHub()->getPoint().get()) ?
//										_destinationVam.getCentroid()->distance(
//											reachedVertex->getHub()->getPoint().get()
sqrt(
(_destinationVam.getCentroid()->getX() - reachedVertex->getHub()->getPoint()->getX()) *
(_destinationVam.getCentroid()->getX() - reachedVertex->getHub()->getPoint()->getX()) +
(_destinationVam.getCentroid()->getY() - reachedVertex->getHub()->getPoint()->getY()) *
(_destinationVam.getCentroid()->getY() - reachedVertex->getHub()->getPoint()->getY())
										):
										numeric_limits<graph::Journey::Distance>::max()
									)
								);

								boost::shared_ptr<RoutePlanningIntermediateJourney> resultJourney(
									new RoutePlanningIntermediateJourney(
										fullApproachJourney,
										serviceUse,
										isGoalReached,
										_destinationVam,
										distanceToEnd,
										_journeyTemplates ?
											_journeyTemplates->testSimilarity(fullApproachJourney, *reachedVertex->getHub(), _accessDirection) :
											false,
										_getScore(
											totalDuration,
											distanceToEnd,
											_accessDirection == DEPARTURE_TO_ARRIVAL ?
												serviceUse.getArrivalDateTime() - _originDateTime :
												_originDateTime - serviceUse.getDepartureDateTime(),
											*reachedVertex->getHub()
										)
								)	);


								// Analyze of the utility of the edge
								// If the edge is useless, the path is not traversed anymore
								_JourneyUsefulness evaluationResult(evaluateJourney(resultJourney,isGoalReached));
								if (!evaluationResult.canBeAResultPart)
								{
									if (!evaluationResult.continueToTraverseThePath)
										break;
									else
										continue;
								}

								// Storage of the journey as a result :
								//	- if goal reached
								//	- if useful for a transfer
								if(	isGoalReached ||
									isReturnedVertex
								){
									_result.add(resultJourney);
								}

								// Storage of the journey for recursion
								if(	isARecursionNode
								){
									boost::shared_ptr<RoutePlanningIntermediateJourney> todoJourney(
										new RoutePlanningIntermediateJourney(
											*journey,
											serviceUse,
											false,
											_destinationVam,
											distanceToEnd,
											_journeyTemplates ? _journeyTemplates->testSimilarity(*journey, *reachedVertex->getHub(), _accessDirection) : false,
											resultJourney->getScore()
									)	);
									todo.add(todoJourney);
								}

								// Storage of the reach time at the goal if applicable
								if (isGoalReached)
								{
									if (_accessDirection == DEPARTURE_TO_ARRIVAL)
									{
										ptime newMinMaxDateTimeAtDestination(serviceUse.getArrivalDateTime());
										newMinMaxDateTimeAtDestination += _destinationVam.getVertexAccess(reachedVertex).approachTime;
										if(newMinMaxDateTimeAtDestination < _minMaxDateTimeAtDestination)
										{
											_minMaxDateTimeAtDestination = newMinMaxDateTimeAtDestination;
										}
									}
									else
									{
										ptime newMinMaxDateTimeAtDestination(serviceUse.getDepartureDateTime());
										newMinMaxDateTimeAtDestination -= _destinationVam.getVertexAccess(reachedVertex).approachTime;
										if(newMinMaxDateTimeAtDestination > _minMaxDateTimeAtDestination)
										{
											_minMaxDateTimeAtDestination = newMinMaxDateTimeAtDestination;
										}
									}
								}
							} // next arrival edge

							if(nonServedEdges.empty())
								break;
						} // next service
						} // next service collection
					} // next departure edge
				} // next vertex in vam
			} // Next place to explore (todo)

			// Validating all the service uses compared to the final result list
/*			for (IntegralSearchWorkingResult::iterator it(result.begin()); it != result.end();)
			{
				if (!evaluateJourney(it->second))
					result.erase(it++);
				else
				{
					_result[it->first] = it->second;
					++it;
				}
			}
*/

			_logger.closeIntegralSearchLog();
		}

// ------------------------------------------------------------------------- Utilities

		IntegralSearcher::_JourneyUsefulness IntegralSearcher::evaluateJourney(
			const boost::shared_ptr<RoutePlanningIntermediateJourney>& journeysptr,
			bool isGoalReached
		) const {

			const RoutePlanningIntermediateJourney& journey(*journeysptr);

			assert(!journey.empty());

			/// <h2>Check of the compliance with the maximal duration</h2>
			if(_maxDuration && journey.getDuration() > *_maxDuration)
			{ /// TODO do the same think to all false,false returns
				return _JourneyUsefulness(false, journey.getDuration() - (_accessDirection == DEPARTURE_TO_ARRIVAL ? journey.getEndApproachDuration() : journey.getStartApproachDuration()) <= *_maxDuration );
			}

			/// <h2>Check of the compliance with the current filters</h2>
			const ServicePointer& serviceUse( journey.getEndServiceUse());
			const Vertex* reachedVertex(	(
					_accessDirection == DEPARTURE_TO_ARRIVAL ?
					serviceUse.getArrivalEdge() :
					serviceUse.getDepartureEdge()
				)->getFromVertex()
			);
			if (reachedVertex->getGraphType() == RoadModule::GRAPH_ID)
			{
				/** - If the edge is an address, the currentJourney necessarily contains
					only road legs, filter approach (= walk distance and duration).
				*/
				if(!_accessParameters.isCompatibleWithApproach(journey.getDistance(), journey.getDuration()))
					return _JourneyUsefulness(false,false);
			}

			/// <h2>Determination of the usefulness to store the service use</h2>

			/** - To be worse than the absolute best time is forbidden. */
			const ptime& reachDateTime(
				_accessDirection == DEPARTURE_TO_ARRIVAL ?
				serviceUse.getArrivalDateTime() :
				serviceUse.getDepartureDateTime()
			);
			/* TODO : solve memory errors that makes _destinationVam.getVertexAccess(reachedVertex).approachTime having a wrong vaule (40000 hours !)
			 * See https://extranet.rcsmobility.com/issues/19961
			 * When solved, this code cold be uncommented
			if(isGoalReached)
			{
				if(	(	(_accessDirection == ARRIVAL_TO_DEPARTURE)
					&&	(reachDateTime - _destinationVam.getVertexAccess(reachedVertex).approachTime < _minMaxDateTimeAtDestination)
					)
				||	(	(_accessDirection == DEPARTURE_TO_ARRIVAL)
					&&	(reachDateTime + _destinationVam.getVertexAccess(reachedVertex).approachTime > _minMaxDateTimeAtDestination)
					)
				)	return _JourneyUsefulness(false, false);
			}
			else
			{*/
				if(	(	(_accessDirection == ARRIVAL_TO_DEPARTURE)
					&&	(reachDateTime < _minMaxDateTimeAtDestination)
					)
				||	(	(_accessDirection == DEPARTURE_TO_ARRIVAL)
					&&	(reachDateTime > _minMaxDateTimeAtDestination)
					)
				)	return _JourneyUsefulness(false, false);
			//}

			/** - If the reached vertex does not belong to the goal, comparison with the known best time at the goal, to determinate
				if there is any chance to reach the goal more efficiently by using this path
				The time used for comparison corresponds to the minimal time to reach the goal from the vertex, constituted of :
					-# the known time to reach the current vertex
					-# the minimal time to do a transfer in the connecting place
					-# the minimal travel time from the connecting place and the goal according to the current vmax
			*/
			assert(journey.getDistanceToEnd());

			if(	!isGoalReached &&
				reachedVertex->getHub()->isUsefulTransfer(_graphToUse) &&
				*journey.getDistanceToEnd() > 2000
			){
				ptime bestHopedGoalAccessDateTime (reachDateTime);
				posix_time::time_duration minimalGoalReachDuration(
					reachedVertex->getHub()->getMinTransferDelay()	// Minimal time to transfer
					+ seconds(static_cast<long>(*journey.getDistanceToEnd() / _vmax))						// Minimal time to reach the goal
				);

				if (_accessDirection == DEPARTURE_TO_ARRIVAL)
					bestHopedGoalAccessDateTime += minimalGoalReachDuration;
				else
					bestHopedGoalAccessDateTime -= minimalGoalReachDuration;

				if(	(	(_accessDirection == ARRIVAL_TO_DEPARTURE)
					&&	(bestHopedGoalAccessDateTime < _minMaxDateTimeAtDestination)
					)
				||	(	(_accessDirection == DEPARTURE_TO_ARRIVAL)
					&&	(bestHopedGoalAccessDateTime > _minMaxDateTimeAtDestination)
					)
				)	return _JourneyUsefulness(false,true);
			}

			/** - Best vertex map control : the service use is useful only if no other already found
				service use reaches the vertex at a strictly better time.
			*/
			if(	_bestVertexReachesMap.isUseLess(
					journeysptr,
					_originDateTime,
					true,
					!_optim
			)	){
					return _JourneyUsefulness(false,true);
			}

			return _JourneyUsefulness(true,true);
		}



		const ptime& IntegralSearcher::getOriginDateTime() const
		{
			return _originDateTime;
		}



		RoutePlanningIntermediateJourney::Score IntegralSearcher::_getScore(
			boost::optional<boost::posix_time::time_duration> totalDuration,
			int distanceToEnd,
			time_duration journeyDuration,
			const Hub& hub
		) const	{
			RoutePlanningIntermediateJourney::Score score(1000);

			// Case a journey was already found
			if(totalDuration)
			{
				// Part 1+2 : 75% needed speed to reach the goal shorter
				if(totalDuration != journeyDuration)
				{
					score = (distanceToEnd * 750) / (_vmax * (*totalDuration - journeyDuration).total_seconds());
				}
			}
			else
			{
				if(_totalDistance && (_totalDistance > distanceToEnd))
				{
					// Part 1 : 25% projected speed
					score = ((_totalDistance - distanceToEnd) * 250) / (_vmax * journeyDuration.total_seconds());
					if(score > 250)
					{
						score = 250;
					}
					else
					{
						score = 250 - score;
					}

					// Part 2 : 50% projected distance
					score += ((500 * distanceToEnd) / _totalDistance);
				}
			}

			// Part 3 : 25% hub score
			HubScore hubScore(hub.getScore());
			if(hubScore > 1)
			{
				score += 250 - 2.5 * hubScore;
			}
			else
			{
				score += 250;
			}

			return score;
		}


		IntegralSearcher::_JourneyUsefulness::_JourneyUsefulness(
			bool _canBeAResultPart,
			bool _continueToTraverseThePath
		):	canBeAResultPart(_canBeAResultPart),
			continueToTraverseThePath(_continueToTraverseThePath)
		{}
	}
}
