
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

#include "33_route_planner/IntegralSearcher.h"
#include "33_route_planner/BestVertexReachesMap.h"
#include "33_route_planner/JourneysResult.h"

#include "15_env/VertexAccessMap.h"
#include "15_env/Vertex.h"
#include "15_env/Edge.h"
#include "15_env/Path.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Journey.h"
#include "15_env/JourneyComparator.h"

// To be removed by a log class
#include "15_env/LineStop.h"
#include "15_env/Road.h"
#include "15_env/Line.h"
#include "15_env/CommercialLine.h"

#include "04_time/DateTime.h"

#include "01_util/Log.h"

#include <sstream>

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace time;
	using namespace util;

	namespace routeplanner
	{

// -----------------------------------------------------------------------1 Construction

		IntegralSearcher::IntegralSearcher(
			AccessDirection accessDirection
			, const AccessParameters&	accessParameters
			, SearchAddresses searchAddresses
			, SearchPhysicalStops searchPhysicalStops
			, UseRoads useRoads
			, UseLines useLines
			, JourneysResult<env::JourneyComparator>&				result
			, BestVertexReachesMap& bestVertexReachesMap
			, const VertexAccessMap& destinationVam
			, const DateTime& calculationDateTime
			, DateTime&	minMaxDateTimeAtDestination
			, int previousContinuousServiceDuration
			, const DateTime& previousContinuousServiceLastDeparture
			, int maxDepth
			, bool optim
			, bool inverted
			, ostream* const logStream
			, Log::Level logLevel
		)	: _accessDirection(accessDirection)
			, _accessParameters(accessParameters)
			, _searchAddresses(searchAddresses)
			, _searchPhysicalStops(searchPhysicalStops)
			, _useRoads(useRoads)
			, _useLines(useLines)
			, _result(result)
			, _bestVertexReachesMap(bestVertexReachesMap)
			, _destinationVam(destinationVam)
			, _calculationTime(calculationDateTime)
			, _minMaxDateTimeAtDestination(minMaxDateTimeAtDestination)
			, _previousContinuousServiceDuration(previousContinuousServiceDuration)
			, _previousContinuousServiceLastDeparture(previousContinuousServiceLastDeparture)
			, _maxDepth(maxDepth)
			, _optim(optim)
			, _inverted(inverted)
			, _logStream(logStream)
			, _logLevel(logLevel)
		{	}



		void IntegralSearcher::integralSearch(
			const env::VertexAccessMap& startVam
			, const time::DateTime& startTime
			, const Journey& startJourney
			, int maxDepth
			, bool strictTime /*= false  */ 
		){
			// Recusrions to do
			JourneysResult<_JourneyComparator> todo;
			todo.addEmptyJourney(startJourney.getMethod());

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
					stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getName()) << "</td>";

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
							stream << "<td>" << its->getArrivalEdge()->getPlace()->getFullName() << "</td>";

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
							stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getName()) << "</td>";

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
					journey->getEndEdge()->getFromVertex()->getConnectionPlace()->getImmediateVertices(
						vam,
						_accessDirection,
						_accessParameters
						, (_useRoads == USE_ROADS) ? SEARCH_ADDRESSES : DO_NOT_SEARCH_ADDRESSES
						, (_useLines == USE_LINES) ? SEARCH_PHYSICALSTOPS : DO_NOT_SEARCH_PHYSICALSTOPS
						, journey->getEndEdge()->getFromVertex()
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

					if (origin->isAddress() && _useRoads != USE_ROADS
						|| origin->isPhysicalStop() && _useLines != USE_LINES)
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

						if (!_accessParameters.isCompatibleWith(*edge->getParentPath()))
							continue;

						/// @todo reintroduce optimization on following axis departure/arrival ?
						if (!currentJourney.verifyAxisConstraints(edge->getParentPath()->getAxis()))
							continue;

						int serviceNumber(UNKNOWN_VALUE);
						for(bool loopOnServices(true); loopOnServices;)
						{
							// Reach of the next/previous service serving the edge
							DateTime departureMoment(correctedDesiredTime);
							ServicePointer serviceInstance(
								(_accessDirection == DEPARTURE_TO_ARRIVAL)
								?	edge->getNextService (
										departureMoment
										, _minMaxDateTimeAtDestination
										, _calculationTime
										, true
										, serviceNumber
										, _inverted
									)
								:	edge->getPreviousService(
										departureMoment
										, _minMaxDateTimeAtDestination
										, _calculationTime
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
							/// @todo ERROR : must be integrated in ServicePointer constructor. A similar line can be written for edge level.
							//					if (!serviceInstance.getService()->isCompatibleWith(_accessParameters.complyer))
							//						continue;

							PtrEdgeStep step(	
								(_accessDirection == DEPARTURE_TO_ARRIVAL)
								?(	_useRoads == USE_ROADS || _destinationVam.needFineSteppingForArrival (edge->getParentPath ())
									? (&Edge::getFollowingArrivalForFineSteppingOnly)
									: (&Edge::getFollowingConnectionArrival)
								):(	_useRoads == USE_ROADS || _destinationVam.needFineSteppingForDeparture (edge->getParentPath ())
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
								if (_useRoads != USE_ROADS && !reachedVertex->isConnectionAllowed() && !isGoalReached)
									continue;

								// Storage of the useful solution
								Journey* resultJourney(new Journey(fullApproachJourney));
								ServiceUse serviceUse(serviceInstance, curEdge);
								if (_accessDirection == ARRIVAL_TO_DEPARTURE && !serviceUse.isReservationRuleCompliant(_calculationTime))
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

								// Storage of the journey as a result
								if(	(	_searchAddresses == SEARCH_ADDRESSES
										&&	reachedVertex->getConnectionPlace()->hasAddresses()
									)||(_searchPhysicalStops == SEARCH_PHYSICALSTOPS
										&&	reachedVertex->getConnectionPlace()->hasPhysicalStops()
									)||	isGoalReached
								)	_result.add(resultJourney);

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
			if (reachedVertex->isAddress ())
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
				&& reachedVertex->isConnectionAllowed()
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
					reachedVertex->getConnectionPlace()->getMinTransferDelay()	// Minimal time to transfer
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
