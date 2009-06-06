
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
#include "PublicTransportStopZoneConnectionPlace.h"
// To be removed by a log class
#include "LineStop.h"
#include "Road.h"
#include "Line.h"
#include "CommercialLine.h"
#include "RoadModule.h"
#include "DateTime.h"
#include "Service.h"
#include "Log.h"

#include <sstream>

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace time;
	using namespace util;
	using namespace graph;
	using namespace road;
	using namespace geography;

	namespace routeplanner
	{

// -----------------------------------------------------------------------1 Construction

		IntegralSearcher::IntegralSearcher(
			AccessDirection accessDirection
			, const AccessParameters&	accessParameters
			, GraphIdType whatToSearch
			, GraphIdType graphToUse
			, JourneysResult<graph::JourneyComparator>&				result
			, BestVertexReachesMap& bestVertexReachesMap
			, const VertexAccessMap& destinationVam
			, DateTime&	minMaxDateTimeAtDestination
			, int previousContinuousServiceDuration
			, const DateTime& previousContinuousServiceLastDeparture
			, bool optim
			, bool inverted
			, ostream* const logStream
			, Log::Level logLevel
		):	_accessDirection(accessDirection),
			_accessParameters(accessParameters),
			_whatToSearch(whatToSearch),
			_graphToUse(graphToUse),
			_result(result)
			, _bestVertexReachesMap(bestVertexReachesMap)
			, _destinationVam(destinationVam)
			, _minMaxDateTimeAtDestination(minMaxDateTimeAtDestination)
			, _previousContinuousServiceDuration(previousContinuousServiceDuration)
			, _previousContinuousServiceLastDeparture(previousContinuousServiceLastDeparture)
			, _optim(optim)
			, _inverted(inverted)
			, _logStream(logStream)
			, _logLevel(logLevel)
		{	}



		void IntegralSearcher::integralSearch(
			const graph::VertexAccessMap& startVam
			, const time::DateTime& startTime
			, const Journey& startJourney
			, int maxDepth
			, bool strictTime /*= false  */ 
		){
			// Recusrions to do
			JourneysResult<_JourneyComparator> todo;
			todo.addEmptyJourney();

			string s("<table class=\"adminresults\">");
			if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
				Log::GetInstance().trace(s);
			if (_logLevel <= Log::LEVEL_TRACE && _logStream)
				*_logStream << s;

			// The Loop
			while(!todo.empty())
			{
				const Journey* journey(todo.front());

				if(	!journey->empty()
				&&	(Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
					|| _logLevel <= Log::LEVEL_TRACE
					)
				){
					stringstream stream;
					stream
						<< "<tr>"
						<< "<th colspan=\"7\">Journey</th>"
						<< "</tr>"
						;
					
					// Departure time
					Journey::ServiceUses::const_iterator its(journey->getServiceUses().begin());

/*					if (journey->getContinuousServiceRange() > 1)
					{
						DateTime endRange(its->getDepartureDateTime());
						endRange += journey->getContinuousServiceRange();
						stream << " - Service continu jusqu'à " << endRange.toString();
					}
					if (journey->getReservationCompliance() == true)
					{
						stream << " - Réservation obligatoire avant le " << journey->getReservationDeadLine().toString();
					}
					if (journey->getReservationCompliance() == boost::logic::indeterminate)
					{
						stream << " - Réservation facultative avant le " << journey->getReservationDeadLine().toString();
					}
*/
					stream << "<tr>";
					stream << "<td>" << its->getDepartureDateTime().toString() << "</td>";

					// Line
					const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
					const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
					stream << "<td";
					if (ls)
						stream << " class=\"" + ls->getLine()->getCommercialLine()->getStyle() << "\"";
					stream << ">";
					stream << (
							ls ?
							ls->getLine()->getCommercialLine()->getShortName() :
							road->getRoadPlace()->getName()
						) <<
						"</td>"
					;

					// Transfers
					if (its == journey->getServiceUses().end() -1)
					{
						stream << "<td colspan=\"4\">(trajet direct)</td>";
					}
					else
					{
						while(true)
						{
							// Arrival
							stream << "<td>" << its->getArrivalDateTime().toString() << "</td>";

							// Place
							stream <<
								"<td>" <<
								dynamic_cast<const NamedPlace*>(its->getArrivalEdge()->getHub())->getFullName() << 
								"</td>"
							;

							// Next service use
							++its;

							// Departure
							stream << "<td>" << its->getDepartureDateTime().toString() << "</td>";

							// Line
							const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
							const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
							stream << "<td";
							if (ls)
								stream << " class=\"" << ls->getLine()->getCommercialLine()->getStyle() << "\"";
							stream << ">";
							stream <<
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
							stream << "<td></td>";

							// New row and empty origin departure cols;
							stream << "</tr><tr>";
							stream << "<td></td>";
							stream << "<td></td>";
						}
					}

					// Final arrival
					stream << "<td>" << its->getArrivalDateTime().toString() << "</td>";


					string s(todo.getLog());
					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
					{
						Log::GetInstance().trace(stream.str());
						Log::GetInstance().trace(s);
					}
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
					{
						*_logStream << stream.str();
						*_logStream << s;
					}
				}

				VertexAccessMap vam;
				DateTime desiredTime(TIME_UNKNOWN);
				if (journey->empty())
				{
					vam = startVam;
					desiredTime = startTime;
				}
				else
				{
					journey->getEndEdge()->getHub()->getVertexAccessMap(
						vam,
						_accessDirection,
						_graphToUse,
						*journey->getEndEdge()->getFromVertex()
					);
					desiredTime = journey->getEndTime();
				}

				Journey currentJourney(startJourney);
				currentJourney.push(*journey);

				// Loop on each origin vertex
				for(std::map<const Vertex*, VertexAccess>::const_iterator itVertex(vam.getMap ().begin())
					; itVertex != vam.getMap ().end ()
					; ++itVertex
				){
					// Initialization of loop local variables
					const Vertex* origin(itVertex->first);

					// TODO Ensure that this test is not useless
					if(origin->getGraphType() != _graphToUse)
						continue;

					// Approach to the vertex
					Journey fullApproachJourney(currentJourney);
					if(fullApproachJourney.empty())
						fullApproachJourney.setStartApproachDuration(itVertex->second.approachTime);

					DateTime correctedDesiredTime(desiredTime);
					if (_accessDirection == DEPARTURE_TO_ARRIVAL)
						correctedDesiredTime += static_cast<int>(itVertex->second.approachTime);
					else
						correctedDesiredTime -= static_cast<int>(itVertex->second.approachTime);

					// Goal edges loop
					const std::set<const Edge*>& edges((_accessDirection == DEPARTURE_TO_ARRIVAL) ? origin->getDepartureEdges() : origin->getArrivalEdges());

					for(std::set<const Edge*>::const_iterator itEdge = edges.begin ()
						; itEdge != edges.end ()
						; ++itEdge
					){
						const Edge* edge = (*itEdge);

						if (!edge->getParentPath()->isCompatibleWith(_accessParameters))
							continue;

						int serviceNumber(UNKNOWN_VALUE);
						for(bool loopOnServices(true); loopOnServices;)
						{
							// Reach of the next/previous service serving the edge
							DateTime departureMoment(correctedDesiredTime);
							ServicePointer serviceInstance(
								(_accessDirection == DEPARTURE_TO_ARRIVAL)
								?	edge->getNextService(
										_accessParameters.getUserClass(),
										departureMoment
										, _minMaxDateTimeAtDestination
										, true
										, serviceNumber
										, _inverted
									)
								:	edge->getPreviousService(
										_accessParameters.getUserClass(),
										departureMoment
										, _minMaxDateTimeAtDestination
										, true
										, serviceNumber
										, _inverted
									)
							);
							loopOnServices = false;

							// If no service, advance to the next edge
							if (!serviceInstance.getService())
								continue;

							// Strict time control if the departure time must be exactly the desired one (optimization only)
							if (strictTime && serviceInstance.getActualDateTime() != correctedDesiredTime)
								continue;

							serviceNumber = serviceInstance.getServiceIndex() + 1;

							// Check for service compliance rules.
							if (!serviceInstance.getService()->isCompatibleWith(_accessParameters))
								continue;

							PtrEdgeStep step(	
								(_accessDirection == DEPARTURE_TO_ARRIVAL)
								?(	_destinationVam.needFineSteppingForArrival (edge->getParentPath())
									? (&Edge::getFollowingArrivalForFineSteppingOnly)
									: (&Edge::getFollowingConnectionArrival)
								):(	_destinationVam.needFineSteppingForDeparture (edge->getParentPath())
									? (&Edge::getPreviousDepartureForFineSteppingOnly)
									: (&Edge::getPreviousConnectionDeparture)
								)
							);

							// The path is traversed
							for (const Edge* curEdge = (edge->*step) ();
								curEdge != 0; curEdge = (curEdge->*step) ())
							{
								// The reached vertex is analyzed only in two cases :
								//  - if the vertex belongs to the goal
								//  - if the vertex is a connecting vertex
								const Vertex* reachedVertex(curEdge->getFromVertex());
								bool isGoalReached(_destinationVam.contains(reachedVertex));
								
								// In road integral search, all nodes are potential connection points
								if(	!reachedVertex->getHub()->isConnectionPossible() &&
									!isGoalReached
								){
									continue;
								}

								// Storage of the useful solution
								Journey* resultJourney(new Journey(fullApproachJourney));
								ServiceUse serviceUse(serviceInstance, curEdge);
								if (serviceUse.isUseRuleCompliant() == UseRule::RUN_NOT_POSSIBLE)
									continue;

								resultJourney->push(serviceUse);

								if (isGoalReached)
									resultJourney->setEndApproachDuration(_destinationVam.getVertexAccess(reachedVertex).approachTime);


								// Analyze of the utility of the edge
								// If the edge is useless, the path is not traversed anymore
								pair<bool,bool> evaluationResult(evaluateJourney(*resultJourney, _optim));
								if (!evaluationResult.first)
								{
									delete resultJourney;

									if (!evaluationResult.second)
										break;
									else
										continue;
								}

								resultJourney->setEndReached(isGoalReached);
								resultJourney->setSquareDistanceToEnd(_destinationVam);
								resultJourney->setMinSpeedToEnd(_minMaxDateTimeAtDestination);

								// Storage of the journey for recursion
								if (journey->getJourneyLegCount() < maxDepth)
								{
									Journey* todoJourney(new Journey(*journey));
									todoJourney->push(serviceUse);
									todo.add(todoJourney);
								}

								// Storage of the journey as a result :
								//	- if goal reached
								//	- if useful for a transfer
								if(	(	(	!_accessParameters.getMaxtransportConnectionsCount() ||
											resultJourney->getServiceUses().size() < *_accessParameters.getMaxtransportConnectionsCount()
										) &&
										reachedVertex->getHub()->containsAnyVertex(_whatToSearch)
									) ||
									isGoalReached
								){
									_result.add(resultJourney);
								}

								// Storage of the reach time at the vertex in the best vertex reaches map
								_bestVertexReachesMap.insert (serviceUse);

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
			if(	Log::GetInstance().getLevel() <= Log::LEVEL_TRACE
				|| _logLevel <= Log::LEVEL_TRACE
				){
					stringstream s;
					s << "<tr><th colspan=\"7\">";
					if (_accessDirection == DEPARTURE_TO_ARRIVAL)
						s << "DEPARTURE_TO_ARRIVAL";
					else
						s << "ARRIVAL_TO_DEPARTURE   ";
					s	<< " IntegralSearch. Start "
						<< " at " << startTime.toString()
						<< "</th></tr>"
						<< "</table>"
						;

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(s.str());
					if (_logLevel <= Log::LEVEL_TRACE && _logStream)
						*_logStream << s.str();
			}

		}

// ------------------------------------------------------------------------- Utilities

		pair<bool,bool> IntegralSearcher::evaluateJourney(
			const Journey& journey
			, bool optim
		) const {

			assert(!journey.empty());
			
			/// <h2>Control of the compliance with the current filters</h2>
			const ServiceUse& serviceUse(journey.getEndServiceUse());
			const Vertex* reachedVertex(serviceUse.getSecondEdge()->getFromVertex());
			if (reachedVertex->getGraphType() == RoadModule::GRAPH_ID)
			{
				/** - If the edge is an address, the currentJourney necessarily contains
					only road legs, filter approach (= walk distance and duration).
				*/
				if(!_accessParameters.isCompatibleWithApproach(journey.getDistance(), journey.getEffectiveDuration()))
					return make_pair(false,false);
			}

			/// <h2>Determination of the utility to store the service use</h2>

			/** - Continuous service breaking test : if the solution is between a service continuous range
				then it is stored only if its duration is better than the one of the continuous service.
			*/
			if(	(_previousContinuousServiceDuration > 0)
			&&	(journey.getDepartureTime() < _previousContinuousServiceLastDeparture)
			&&	(journey.getDuration() >= _previousContinuousServiceDuration)
			)	return make_pair(false,false);


			/** - To be worse than the absolute best time is forbidden. */
			const DateTime& reachDateTime(serviceUse.getSecondActualDateTime());
			const AccessDirection& method(journey.getMethod());
			if(	(	(method == ARRIVAL_TO_DEPARTURE)
				&&	(reachDateTime < _minMaxDateTimeAtDestination)
				)
			||	(	(method == DEPARTURE_TO_ARRIVAL)
				&&	(reachDateTime > _minMaxDateTimeAtDestination)
				)
			)	return make_pair(false,false);

			/** - If the reached vertex does not belong to the goal, comparison with the known best time at the goal, to determinate 
				if there is any chance to reach the goal more efficiently by using this path
				The time used for comparison corresponds to the minimal time to reach the goal from the vertex, constituted of : 
					-# the known time to reach the current vertex
					-# the minimal time to do a transfer in the connecting place
					-# the minimal travel time from the connecting place and the goal (=1 minute)

				@todo Replace the third value (1 minute) by a more accurate value ("VMAX algorithm")
			*/
			if(	!_destinationVam.contains(reachedVertex)
				&& reachedVertex->getHub()->isConnectionPossible()
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
				DateTime bestHopedGoalAccessDateTime (reachDateTime);
				int minimalGoalReachDuration(
					reachedVertex->getHub()->getMinTransferDelay()	// Minimal time to transfer
					+ 1															// Minimal time to reach the goal
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
				)	return make_pair(false,true);
			}

			/** - Best vertex map control : the service use is useful only if no other already founded
				service use reaches the vertex at a strictly better time.
			*/
			if( (	(method == ARRIVAL_TO_DEPARTURE)
				&&	(reachDateTime < _bestVertexReachesMap.getBestTime(reachedVertex, reachDateTime))
				)
			||	(	(method == DEPARTURE_TO_ARRIVAL)
				&&	(reachDateTime > _bestVertexReachesMap.getBestTime (reachedVertex, reachDateTime))
				)
			)	return make_pair(false,true);

			return make_pair(true,true);
		}



		bool IntegralSearcher::_JourneyComparator::operator() (const Journey* j1, const Journey* j2) const
		{
			assert(j1 != NULL);
			assert(j2 != NULL);
			assert(j1->getMethod() == j2->getMethod());

			int duration1(j1->getDuration());
			int duration2(j2->getDuration());

			if (duration1 != duration2)
				return duration1 < duration2;

			return j1 < j2;
		}
	}
}
