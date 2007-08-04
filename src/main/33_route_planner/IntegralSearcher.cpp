
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
			, JourneysResult&				result
			, BestVertexReachesMap& bestVertexReachesMap
			, const VertexAccessMap& destinationVam
			, const DateTime& calculationDateTime
			, DateTime&	minMaxDateTimeAtDestination
			, int previousContinuousServiceDuration
			, const DateTime& previousContinuousServiceLastDeparture
			, int maxDepth
			, bool optim
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
		{	}

// ------------------------------------------------------------------- 2 Initialization

		void IntegralSearcher::integralSearch(
			const VertexAccessMap& vertices
			, const DateTime& desiredTime
			, bool strictTime /*= false  */
		){
			_integralSearchRecursion(
				vertices
				, desiredTime
				, Journey(_accessDirection)
				, _maxDepth
				, strictTime
				);

			if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
			{
				stringstream s;
				if (_accessDirection == TO_DESTINATION)
					s << "TO_DESTINATION";
				else
					s << "FROM_ORIGIN   ";
				s	<< " Initial IntegralSearch. Start : "
					<< " at " << desiredTime.toString()
				;

				Log::GetInstance().trace(s.str());
			}
		}

		void IntegralSearcher::integralSearch(
			const env::Journey& journey
		){
			assert(!journey.empty());

			VertexAccessMap vertices;
			const Vertex* vertex(journey.getEndEdge()->getFromVertex());
			vertex->getPlace()->getImmediateVertices(
				vertices
				, _accessDirection
				, _accessParameters
				, _searchAddresses
				, _searchPhysicalStops
				, vertex
				);

			_integralSearchRecursion(
				vertices
				, journey.getEndTime()
				, journey
				, _maxDepth
				, false
			);


			if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
			{
				string spaces(journey.getServiceUses().size(), '*');

				stringstream s;
				s	<< spaces;
				if (_accessDirection == TO_DESTINATION)
					s << "TO_DESTINATION";
				else
					s << "FROM_ORIGIN   ";
				s	<< " IntegralSearch. Start : "
					<< journey.getEndEdge()->getFromVertex()->getConnectionPlace()->getFullName()
					<< " at " << journey.getEndTime().toString();

				Log::GetInstance().trace(s.str());
			}
		}
// ------------------------------------------------------------------------ 3 Recursion

		void IntegralSearcher::_integralSearchRecursion(
			const env::VertexAccessMap& vam
			, const time::DateTime& desiredTime
			, const Journey& currentJourney
			, int maxDepth
			, bool strictTime /*= false  */ 
		){
			// Local variables initialization
			map<const Vertex*, Journey*> recursionVertices;

			// Loop on each origin vertex
			for(std::map<const Vertex*, VertexAccess>::const_iterator itVertex(vam.getMap ().begin());
				itVertex != vam.getMap ().end ()
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
				if (_accessDirection == TO_DESTINATION)
					correctedDesiredTime += static_cast<int>(itVertex->second.approachTime);
				else
					correctedDesiredTime -= static_cast<int>(itVertex->second.approachTime);

				// Goal edges loop
				const std::set<const Edge*>& edges((_accessDirection == TO_DESTINATION) ? origin->getDepartureEdges() : origin->getArrivalEdges());

				for (std::set<const Edge*>::const_iterator itEdge = edges.begin ();
					itEdge != edges.end () ; ++itEdge)
				{
					const Edge* edge = (*itEdge);

					if (!edge->getParentPath ()->isCompatibleWith(_accessParameters.complyer))
						continue;

					/// @todo reintroduce optimization on following axis departure/arrival ?
					if (!currentJourney.verifyAxisConstraints(edge->getParentPath()->getAxis()))
						continue;

					// Reach of the next/previous service serving the edge
					DateTime departureMoment(correctedDesiredTime);
					ServicePointer serviceInstance(
						(_accessDirection == TO_DESTINATION)
						?	edge->getNextService (
								departureMoment
								, _minMaxDateTimeAtDestination
								, _calculationTime
							)
						:	edge->getPreviousService(
								departureMoment
								, _minMaxDateTimeAtDestination
								, _calculationTime
							)
					);

					// If no service, advande to the next edge
					if (!serviceInstance.getService())
						continue;

					// Strict time control if the departure time must be exactly the desired one (optimization only)
					if (strictTime && serviceInstance.getActualDateTime() != correctedDesiredTime)
						continue;

					// Check for service compliancy rules.
					/// @todo ERROR : must be integrated in ServicePointer constructor. A similar line can be written for edge level.
//					if (!serviceInstance.getService()->isCompatibleWith(_accessParameters.complyer))
//						continue;

					PtrEdgeStep step(	
						(_accessDirection == TO_DESTINATION)
							? (	_destinationVam.needFineSteppingForArrival (edge->getParentPath ())
								? (&Edge::getFollowingArrivalForFineSteppingOnly)
								: (&Edge::getFollowingConnectionArrival)
							):( _destinationVam.needFineSteppingForDeparture (edge->getParentPath ())
								? (&Edge::getPreviousDepartureForFineSteppingOnly)
								: (&Edge::getPreviousConnectionDeparture)
							))
						;

					// The path is traversed
					for (const Edge* curEdge = (edge->*step) ();
						curEdge != 0; curEdge = (curEdge->*step) ())
					{
						// The reached vertex is analyzed only in two cases :
						//  - if the vertex belongs to the goal
						//  - if the vertex is a connecting vertex
						const Vertex* reachedVertex(curEdge->getFromVertex());
						bool isGoalReached(_destinationVam.contains(reachedVertex));
						if (!reachedVertex->isConnectionAllowed() && !isGoalReached)
							continue;
						
						// Storage of the useful solution
						Journey* resultJourney = new Journey(fullApproachJourney);
						ServiceUse serviceUse(serviceInstance, curEdge);
						if (_accessDirection == FROM_ORIGIN && !serviceUse.isReservationRuleCompliant(_calculationTime))
							continue;

						if (_accessDirection == TO_DESTINATION)
							resultJourney->append (serviceUse);
						else
							resultJourney->prepend(serviceUse);
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

						// Storage of the journey for recursion
						if (maxDepth > 0)
						{
							map<const Vertex*, Journey*>::iterator itr(recursionVertices.find(reachedVertex));
							if (itr != recursionVertices.end())
							{
								delete itr->second;
								itr->second = new Journey(*resultJourney);
							}
							else
								recursionVertices.insert(make_pair<const Vertex*, Journey*>(reachedVertex, new Journey(*resultJourney)));
						}

						resultJourney->setEndReached(isGoalReached);
						resultJourney->setSquareDistanceToEnd(_destinationVam);
						resultJourney->setMinSpeedToEnd(_minMaxDateTimeAtDestination);

						// Storage of the journey as a result
						if(	(	_searchAddresses == SEARCH_ADDRESSES
							&&	!reachedVertex->getConnectionPlace()->getAddresses().empty()
							)
						||	(	_searchPhysicalStops == SEARCH_PHYSICALSTOPS
							&&	!reachedVertex->getConnectionPlace()->getPhysicalStops().empty()
							)
						||	isGoalReached
						)	_result.add(resultJourney);

						// Storage of the reach time at the vertex in the best vertex reaches map
						_bestVertexReachesMap.insert (serviceUse);

						// Storage of the reach time at the goal if applicable
						if (isGoalReached)
						{
							_minMaxDateTimeAtDestination = serviceUse.getSecondActualDateTime();
							if (_accessDirection == TO_DESTINATION)
								_minMaxDateTimeAtDestination += _destinationVam.getVertexAccess(reachedVertex).approachTime;
							else
								_minMaxDateTimeAtDestination -= _destinationVam.getVertexAccess(reachedVertex).approachTime;
						}
					}

				} // next edge

			} // next vertex in vam


			// Recursion on each service use
			if (maxDepth > 0)
			{
				// Now iterate on each journey leg and call recursively the integral search
				for(map<const Vertex*, Journey*>::const_iterator itLeg(recursionVertices.begin());
					itLeg != recursionVertices.end();
					++itLeg
				){
					const Journey& journey(*itLeg->second);
					const ServiceUse& serviceUse(journey.getEndServiceUse());
					const Vertex* nextVertex(itLeg->first);

					VertexAccessMap nextVam;
					nextVertex->getPlace ()->getImmediateVertices(
						nextVam,
						_accessDirection,
						_accessParameters
						, (_useRoads == USE_ROADS) ? SEARCH_ADDRESSES : DO_NOT_SEARCH_ADDRESSES
						, (_useLines == USE_LINES) ? SEARCH_PHYSICALSTOPS : DO_NOT_SEARCH_PHYSICALSTOPS
						, nextVertex
					);

					_integralSearchRecursion(
						nextVam
						, serviceUse.getSecondActualDateTime()
						, journey
						, maxDepth - 1
						, false
					);
				}
			}

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
					only road legs, filter on max approach distance (= walk distance).
				*/
				if(journey.getDistance () > _accessParameters.maxApproachDistance)
					return make_pair(false,false);

				/** - If the edge is an address, the currentJourney necessarily contains
					only road legs, filter on max approach time (= walk time).
				*/
				if(journey.getEffectiveDuration () > _accessParameters.maxApproachTime)
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
			if(	(	(method == FROM_ORIGIN)
				&&	(reachDateTime < _minMaxDateTimeAtDestination)
				)
			||	(	(method == TO_DESTINATION)
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

				if (method == TO_DESTINATION)
					bestHopedGoalAccessDateTime += minimalGoalReachDuration;
				else
					bestHopedGoalAccessDateTime -= minimalGoalReachDuration;

				if(	(	(method == FROM_ORIGIN)
					&&	(bestHopedGoalAccessDateTime < _minMaxDateTimeAtDestination)
					)
				||	(	(method == TO_DESTINATION)
					&&	(bestHopedGoalAccessDateTime > _minMaxDateTimeAtDestination)
					)
				)	return make_pair(false, true);
			}

			/** - Best vertex map control : the service use is useful only if no other already founded
				service use reaches the vertex at a strictly better time.
			*/
			if( (	(method == FROM_ORIGIN)
				&&	(reachDateTime < _bestVertexReachesMap.getBestTime(reachedVertex, reachDateTime))
				)
			||	(	(method == TO_DESTINATION)
				&&	(reachDateTime > _bestVertexReachesMap.getBestTime (reachedVertex, reachDateTime))
				)
			)	return make_pair(false, true);

			return make_pair(true,true);
		}
	}
}
