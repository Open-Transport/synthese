/** IntegralSearcher class implementation.
	@file IntegralSearcher.cpp

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

#include "NamedPlace.h"
#include "IntegralSearcher.h"
#include "BestVertexReachesMap.h"
#include "JourneysResult.h"
#include "RoadPlace.h"
#include "VertexAccessMap.h"
#include "Vertex.h"
#include "Edge.h"
#include "Path.h"
#include "Hub.h"
#include "Journey.h"
#include "JourneyComparator.h"
#include "JourneyTemplates.h"
#include "PublicTransportStopZoneConnectionPlace.h"
// To be removed by a log class
#include "LineStop.h"
#include "Road.h"
#include "Line.h"
#include "CommercialLine.h"
#include "RoadModule.h"
#include "Service.h"
#include "Log.h"

#include <sstream>

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace geometry;

	namespace algorithm
	{

// -----------------------------------------------------------------------1 Construction

		IntegralSearcher::IntegralSearcher(
			AccessDirection accessDirection,
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
			ostream* const logStream,
			int totalDistance,
			boost::optional<const JourneyTemplates&> journeyTemplates
		):	_accessDirection(accessDirection),
			_accessParameters(accessParameters),
			_whatToSearch(whatToSearch),
			_searchOnlyNodes(searchOnlyNodes),
			_graphToUse(graphToUse),
			_result(result),
			_bestVertexReachesMap(bestVertexReachesMap),
			_destinationVam(destinationVam),
			_originDateTime(originDateTime),
			_minMaxDateTimeAtOrigin(minMaxDateTimeAtOrigin),
			_minMaxDateTimeAtDestination(minMaxDateTimeAtDestination),
			_inverted(inverted),
			_optim(optim),
			_maxDuration(maxDuration),
			_logStream(logStream),
			_totalDistance(totalDistance),
			_journeyTemplates(journeyTemplates)
		{}



		void IntegralSearcher::integralSearch(
			const graph::VertexAccessMap& vertices,
			optional<size_t> maxDepth,
			boost::optional<boost::posix_time::time_duration> totalDuration
		){
			Journey emptyJourney;
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
			const graph::Journey& startJourney,
			optional<std::size_t> maxDepth,
			boost::optional<boost::posix_time::time_duration> totalDuration
		){
			VertexAccessMap vam;
			startJourney.getEndEdge()->getHub()->getVertexAccessMap(
				vam,
				_accessDirection,
				_graphToUse,
				*startJourney.getEndEdge()->getFromVertex()
			);
			_integralSearch(
				vam,
				startJourney,
				startJourney.getEndTime(),
				_minMaxDateTimeAtDestination,
				maxDepth,
				totalDuration
			);
		}



		void IntegralSearcher::_integralSearch(
			const graph::VertexAccessMap& vam,
			const graph::Journey& startJourney,
			const boost::posix_time::ptime& desiredTime,
			const boost::posix_time::ptime& minMaxDateTimeAtOrigin,
			optional<size_t> maxDepth,
			boost::optional<boost::posix_time::time_duration> totalDuration
		){
			// Recursions to do
			JourneysResult todo(_originDateTime);
			todo.addEmptyJourney();

			if (_logStream)
			{
				*_logStream << "<table class=\"adminresults\">";
			}

			// The Loop
			while(!todo.empty())
			{
				this_thread::interruption_point();
				shared_ptr<Journey> journey(todo.front());

				VertexAccessMap curVam;
				if(journey->empty())
				{
					curVam = vam;
				}
				else
				{
					journey->getEndEdge()->getHub()->getVertexAccessMap(
						curVam,
						_accessDirection,
						_graphToUse,
						*journey->getEndEdge()->getFromVertex()
					);
				}

				if(	!journey->empty() &&
					_logStream
				){
					*_logStream
						<< "<tr>"
						<< "<th colspan=\"7\">Journey</th>"
						<< "</tr>"
						;
					
					// Departure time
					Journey::ServiceUses::const_iterator its(journey->getServiceUses().begin());

/*					if (journey->getContinuousServiceRange() > 1)
					{
						ptime endRange(its->getDepartureDateTime());
						endRange += journey->getContinuousServiceRange();
						*_logStream << " - Service continu jusqu'à " << endRange;
					}
					if (journey->getReservationCompliance() == true)
					{
						*_logStream << " - Réservation obligatoire avant le " << journey->getReservationDeadLine();
					}
					if (journey->getReservationCompliance() == boost::logic::indeterminate)
					{
						*_logStream << " - Réservation facultative avant le " << journey->getReservationDeadLine();
					}
*/
					*_logStream << "<tr>";
					*_logStream << "<td>";
					*_logStream << its->getDepartureDateTime();
					*_logStream << "</td>";

					// Line
					const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
					const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
					*_logStream << "<td";
					if (ls)
						*_logStream << " class=\"" + ls->getLine()->getCommercialLine()->getStyle() << "\"";
					*_logStream << ">";
					*_logStream << (
							ls ?
							ls->getLine()->getCommercialLine()->getShortName() :
							road->getRoadPlace()->getName()
						) <<
						"</td>"
					;

					// Transfers
					if (its == journey->getServiceUses().end() -1)
					{
						*_logStream << "<td colspan=\"4\">(trajet direct)</td>";
					}
					else
					{
						while(true)
						{
							// Arrival
							*_logStream << "<td>";
							*_logStream << its->getArrivalDateTime();
							*_logStream << "</td>";

							// Place
							*_logStream << "<td>";
							if(dynamic_cast<const NamedPlace*>(its->getArrivalEdge()->getHub()))
							{
								*_logStream << dynamic_cast<const NamedPlace*>(its->getArrivalEdge()->getHub())->getFullName();
							}
							*_logStream << "</td>";

							// Next service use
							++its;

							// Departure
							*_logStream << "<td>" << its->getDepartureDateTime() << "</td>";

							// Line
							const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
							const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
							*_logStream << "<td";
							if (ls)
								*_logStream << " class=\"" << ls->getLine()->getCommercialLine()->getStyle() << "\"";
							*_logStream << ">";
							*_logStream <<
								(	ls ?
									ls->getLine()->getCommercialLine()->getShortName() :
									road->getRoadPlace()->getName()
								) <<
								"</td>"
							;

							// Exit if last service use
							if (its == journey->getServiceUses().end() -1)
								break;

							// Empty final arrival col
							*_logStream << "<td></td>";

							// New row and empty origin departure cols;
							*_logStream << "</tr><tr>";
							*_logStream << "<td></td>";
							*_logStream << "<td></td>";
						}
					}

					// Final arrival
					*_logStream << "<td>" << its->getArrivalDateTime() << "</td>";


//					*_logStream << todo.getLog();
				}

				Journey currentJourney(startJourney, *journey);

				// Loop on each origin vertex
				for(map<const Vertex*, VertexAccess>::const_iterator itVertex(curVam.getMap ().begin());
					itVertex != curVam.getMap().end();
					++itVertex
				){
					this_thread::interruption_point();

					// Initialization of loop local variables
					const Vertex* origin(itVertex->first);

					// TODO Ensure that this test is not useless
					if(origin->getGraphType() != _graphToUse)
						continue;

					// Approach to the vertex
					Journey fullApproachJourney(currentJourney);
					if(fullApproachJourney.empty())
						fullApproachJourney.setStartApproachDuration(itVertex->second.approachTime);

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
						const Path& path(*itEdge.first);
						if(	!path.isCompatibleWith(_accessParameters) ||
							!_accessParameters.isAllowedPathClass(path.getPathClass() ? path.getPathClass()->getIdentifier() : 0)
						){
							continue;
						}

						assert(itEdge.second);
						const Edge& edge(*itEdge.second);

						optional<Edge::DepartureServiceIndex::Value> departureServiceNumber;
						optional<Edge::ArrivalServiceIndex::Value> arrivalServiceNumber;
						set<const Edge*> nonServedEdges;
						ptime departureMoment(correctedDesiredTime);
						while(true)
						{
							this_thread::interruption_point();

							// Reach of the next/previous service serving the edge
							ServicePointer serviceInstance(
								(_accessDirection == DEPARTURE_TO_ARRIVAL)
								?	edge.getNextService(
										_accessParameters.getUserClassRank(),
										departureMoment,
										correctedMinMaxDateTimeAtOrigin
										, true
										, departureServiceNumber
										, _inverted
									)
								:	edge.getPreviousService(
										_accessParameters.getUserClassRank(),
										departureMoment,
										correctedMinMaxDateTimeAtOrigin
										, true
										, arrivalServiceNumber
										, _inverted
									)
							);

							// If no service, advance to the next edge
							if (!serviceInstance.getService())
								break;

							if(_accessDirection == DEPARTURE_TO_ARRIVAL)
							{
								++*departureServiceNumber; // To the next service
							}
							else
							{
								++*arrivalServiceNumber; // To the previous service (reverse iterator increment)
							}
							departureMoment = serviceInstance.getActualDateTime();

							// Check for service compliance rules.
							if (!serviceInstance.getService()->isCompatibleWith(_accessParameters))
								continue;

							PtrEdgeStep step(	
								(_accessDirection == DEPARTURE_TO_ARRIVAL)
								?(	!_searchOnlyNodes || _destinationVam.needFineSteppingForArrival (edge.getParentPath())
									? (&Edge::getFollowingArrivalForFineSteppingOnly)
									: (&Edge::getFollowingConnectionArrival)
								):(	!_searchOnlyNodes || _destinationVam.needFineSteppingForDeparture (edge.getParentPath())
									? (&Edge::getPreviousDepartureForFineSteppingOnly)
									: (&Edge::getPreviousConnectionDeparture)
								)
							);

							bool nonServedEdgesSearch(!nonServedEdges.empty());

							// The path is traversed
							for (const Edge* curEdge = (edge.*step) ();
								curEdge != NULL; curEdge = (curEdge->*step) ())
							{
								this_thread::interruption_point();

								// If the path traversal is only to find non served edges, analyse it only if
								// it belongs to the list
								if(nonServedEdgesSearch)
								{
									set<const Edge*>::iterator it(nonServedEdges.find(curEdge));
									if(it == nonServedEdges.end())
										continue;
									nonServedEdges.erase(it);
								}

								const Vertex* reachedVertex(curEdge->getFromVertex());

								// The reached vertex is analyzed only in 3 cases :
								//  - if the vertex belongs to the goal
								//  - if the type of the vertex corresponds to the searched one (if 
								//		the _searchOnlyNodes parameter is activated, the vertex must 
								//		also belong to a connection place)
								//  - if the vertex belongs to a connection place
								bool isGoalReached(
									_destinationVam.contains(reachedVertex)
								);
								bool isReturnedVertex(
									(	reachedVertex->getHub()->containsAnyVertex(_whatToSearch) &&
										(	!_searchOnlyNodes ||
											(	reachedVertex->getHub()->isConnectionPossible() &&
												(	!_accessParameters.getMaxtransportConnectionsCount() ||
													fullApproachJourney.size() + 1 < *_accessParameters.getMaxtransportConnectionsCount()
								)	)	)	)	);
								bool isARecursionNode(
									reachedVertex->getHub()->isConnectionPossible() &&
									(	!maxDepth || journey->size() < *maxDepth)
								);
								if(	!isGoalReached &&
									!isReturnedVertex &&
									!isARecursionNode
								){
									continue;
								}

								// Storage of the useful solution
								ServiceUse serviceUse(serviceInstance, curEdge);
								if (serviceUse.isUseRuleCompliant() == UseRule::RUN_NOT_POSSIBLE)
								{
									nonServedEdges.insert(curEdge);
									continue;
								}

								// Result journey writing
								shared_ptr<Journey> resultJourney(
									new Journey(fullApproachJourney, serviceUse)
								);
								_setJourneyRoutePlanningInformations(
									*resultJourney,
									isGoalReached,
									totalDuration
								);


								// Analyze of the utility of the edge
								// If the edge is useless, the path is not traversed anymore
								_JourneyUsefulness evaluationResult(evaluateJourney(resultJourney));
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
									shared_ptr<Journey> todoJourney(
										new Journey(*journey, serviceUse)
									);
									todo.add(todoJourney);
								}

								// Storage of the reach time at the goal if applicable
								if (isGoalReached)
								{
									_minMaxDateTimeAtDestination = serviceUse.getSecondActualDateTime();
									if (_accessDirection == DEPARTURE_TO_ARRIVAL)
										_minMaxDateTimeAtDestination += _destinationVam.getVertexAccess(reachedVertex).approachTime;
									else
										_minMaxDateTimeAtDestination -= _destinationVam.getVertexAccess(reachedVertex).approachTime;
								}
							} // next arrival edge

							if(nonServedEdges.empty())
								break;
						} // next service
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

			if(	_logStream
			){
				*_logStream << "<tr><th colspan=\"7\">";
				if (_accessDirection == DEPARTURE_TO_ARRIVAL)
					*_logStream << "DEPARTURE_TO_ARRIVAL";
				else
					*_logStream << "ARRIVAL_TO_DEPARTURE   ";
				*_logStream	<< " IntegralSearch. Start "
					<< " at " << desiredTime
					<< "</th></tr>"
					<< "</table>"
					;
			}
		}

// ------------------------------------------------------------------------- Utilities

		IntegralSearcher::_JourneyUsefulness IntegralSearcher::evaluateJourney(
			shared_ptr<Journey> journeysptr
		) const {

			const Journey& journey(*journeysptr);

			assert(!journey.empty());

			/// <h2>Control of the compliance with the maximal duration</h2>
			if(_maxDuration && journey.getDuration() > *_maxDuration)
			{
				return _JourneyUsefulness(false, false);
			}

			/// <h2>Control of the compliance with the current filters</h2>
			const ServiceUse& serviceUse(journey.getEndServiceUse());
			const Vertex* reachedVertex(serviceUse.getSecondEdge()->getFromVertex());
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
			const ptime& reachDateTime(serviceUse.getSecondActualDateTime());
			const AccessDirection& method(journey.getMethod());
			if(	(	(method == ARRIVAL_TO_DEPARTURE)
				&&	(reachDateTime < _minMaxDateTimeAtDestination)
				)
			||	(	(method == DEPARTURE_TO_ARRIVAL)
				&&	(reachDateTime > _minMaxDateTimeAtDestination)
				)
			)	return _JourneyUsefulness(false,false);

			/** - If the reached vertex does not belong to the goal, comparison with the known best time at the goal, to determinate 
				if there is any chance to reach the goal more efficiently by using this path
				The time used for comparison corresponds to the minimal time to reach the goal from the vertex, constituted of : 
					-# the known time to reach the current vertex
					-# the minimal time to do a transfer in the connecting place
					-# the minimal travel time from the connecting place and the goal (=1 minute)

				@todo Replace the third value (1 minute) by a more accurate value ("VMAX algorithm")
			*/
			if(	!_destinationVam.contains(reachedVertex) &&
				reachedVertex->getHub()->isConnectionPossible() &&
				_searchOnlyNodes
			){

/* Extract of the old VMAX code
				SquareDistance sqd;
				if (sqd.getSquareDistance () == UNKNOWN_VALUE)
				{
				sqd.setFromPoints (*goalVertex, _destinationVam.getIsobarycenter ());  
				sqd.setSquareDistance (sqd.getSquareDistance () - 
				_destinationVam.getIsobarycenterMaxSquareDistance ().getSquareDistance ());
				}
*/
				ptime bestHopedGoalAccessDateTime (reachDateTime);
				posix_time::time_duration minimalGoalReachDuration(
					reachedVertex->getHub()->getMinTransferDelay()	// Minimal time to transfer
					+ posix_time::minutes(1)						// Minimal time to reach the goal
				);

				if (method == DEPARTURE_TO_ARRIVAL)
					bestHopedGoalAccessDateTime += minimalGoalReachDuration;
				else
					bestHopedGoalAccessDateTime -= minimalGoalReachDuration;

				if(	(	(method == ARRIVAL_TO_DEPARTURE)
					&&	(bestHopedGoalAccessDateTime < _minMaxDateTimeAtDestination)
					)
				||	(	(method == DEPARTURE_TO_ARRIVAL)
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



		void IntegralSearcher::_setJourneyRoutePlanningInformations(
			Journey& journey,
			bool endIsReached,
			boost::optional<boost::posix_time::time_duration> totalDuration
		) const {
			journey.setEndIsReached(endIsReached);

			if(endIsReached)
			{
				journey.setEndApproachDuration(_destinationVam.getVertexAccess(journey.getEndEdge()->getFromVertex()).approachTime);
				journey.setDistanceToEnd(0);
			}
			else
			{
				journey.setDistanceToEnd(
					_destinationVam.getIsobarycenter().getDistanceTo(journey.getEndEdge()->getHub()->getPoint())
				);
				setJourneyScore(journey, totalDuration);
			}
		}



		void IntegralSearcher::setJourneyScore(
			Journey& journey,
			boost::optional<boost::posix_time::time_duration> totalDuration
		) const {
			if(!totalDuration && logic::indeterminate(journey.getSimilarity()) && _journeyTemplates)
			{
				journey.setSimilarity(_journeyTemplates->testJourneySimilarity(journey));
			}
			if(journey.getSimilarity() == true)
			{
				journey.setScore(0);
				return;
			}

			long long unsigned int estimatedTotalDuration;
			if(totalDuration)
			{
				estimatedTotalDuration = totalDuration->total_seconds();
			}
			else
			{
				estimatedTotalDuration = ceil(_totalDistance * 0.36);
			}
			long long unsigned int distanceToEnd(journey.getDistanceToEnd());
			long long unsigned int journeyDuration(
				(	journey.getMethod() == DEPARTURE_TO_ARRIVAL ?
					journey.getEndTime() - _originDateTime :
					_originDateTime - journey.getEndTime()
				).total_seconds()
			);


			Journey::Score score(
				(_totalDistance == 0 || estimatedTotalDuration == 0) ?
				1000 :
				static_cast<long long unsigned int>(1000 * distanceToEnd * distanceToEnd * journeyDuration) /
				static_cast<long long unsigned int>(_totalDistance * _totalDistance * estimatedTotalDuration)
			);


			HubScore hubScore(journey.getEndEdge()->getHub()->getScore());
			if(hubScore > 1)
			{
				score /= hubScore;
			}

			if(score > 1000)
			{
				score = 1000;
			}
			journey.setScore(score);
		}



		const ptime& IntegralSearcher::getOriginDateTime() const
		{
			return _originDateTime;
		}


		IntegralSearcher::_JourneyUsefulness::_JourneyUsefulness(
			bool _canBeAResultPart,
			bool _continueToTraverseThePath
		):	canBeAResultPart(_canBeAResultPart),
			continueToTraverseThePath(_continueToTraverseThePath)
		{}
	}
}
