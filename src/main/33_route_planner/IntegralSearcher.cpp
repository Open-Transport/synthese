
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
#include "33_route_planner/Journey.h"
#include "33_route_planner/JourneyLegComparator.h"

#include "15_env/VertexAccessMap.h"
#include "15_env/Vertex.h"
#include "15_env/Edge.h"
#include "15_env/Path.h"
#include "15_env/ConnectionPlace.h"

#include "04_time/DateTime.h"

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace time;

	namespace routeplanner
	{


		IntegralSearcher::IntegralSearcher(
			const AccessDirection& accessDirection
			, const AccessParameters&	accessParameters
			, const SearchAddresses& searchAddresses
			, const SearchPhysicalStops& searchPhysicalStops
			, const UseRoads& useRoads
			, const UseLines& useLines
			, BestVertexReachesMap& bestVertexReachesMap
			, const VertexAccessMap& destinationVam
			, const DateTime& calculationDateTime
			, DateTime&	minMaxDateTimeAtDestination
			, int previousContinuousServiceDuration
			, const DateTime& previousContinuousServiceLastDeparture
			, const Journey& accessJourney
		)	: _accessDirection(accessDirection)
			, _accessParameters(accessParameters)
			, _searchAddresses(searchAddresses)
			, _searchPhysicalStops(searchPhysicalStops)
			, _useRoads(useRoads)
			, _useLines(useLines)
			, _bestVertexReachesMap(bestVertexReachesMap)
			, _destinationVam(destinationVam)
			, _calculationTime(calculationDateTime)
			, _minMaxDateTimeAtDestination(minMaxDateTimeAtDestination)
			, _previousContinuousServiceDuration(previousContinuousServiceDuration)
			, _previousContinuousServiceLastDeparture(previousContinuousServiceLastDeparture)
			, _accessJourney(accessJourney)
		{	}

		void IntegralSearcher::_integralSearchRecursion(
			const env::VertexAccessMap& vam
			, const time::DateTime& desiredTime
			, const Journey& currentJourney
			, int maxDepth
			, bool strictTime /*= false  */ 
		){
			SimplifiedResult simplifiedResult;
			SimplifiedResultIndex simplifiedResultIndex;

			for (std::map<const Vertex*, VertexAccess>::const_iterator itVertex = vam.getMap ().begin ();
				itVertex != vam.getMap ().end (); ++itVertex)
			{
				const Vertex* origin = itVertex->first;

				if (origin->isAddress() && _useRoads != USE_ROADS
					|| origin->isPhysicalStop() && _useLines != USE_LINES)
					continue;

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

					DateTime departureMoment(desiredTime);
					DateTime originDateTime;
					if (_accessDirection == TO_DESTINATION)
						departureMoment += static_cast<int>(itVertex->second.approachTime);
					else
						departureMoment -= static_cast<int>(itVertex->second.approachTime);

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

					if (!serviceInstance.getService())
						continue;

					if (strictTime && serviceInstance.getActualDateTime() != desiredTime)
						continue;

					// Check for service compliancy rules.
					if (!serviceInstance.getService()->isCompatibleWith(_accessParameters.complyer))
						continue;

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
						if (!reachedVertex->isConnectionAllowed() && !_destinationVam.contains(reachedVertex))
							continue;
						
						// Analyze of the utility of the edge
						// If the edge is useless, the path is not traversed anymore
						ServiceUse serviceUse(serviceInstance, curEdge);
						if (!_evaluateServiceUse(serviceUse,currentJourney))
							break;

						// Storage of the useful solution
						SimplifiedResultIndex::iterator it(simplifiedResultIndex.find(reachedVertex));
						if (it != simplifiedResultIndex.end())
						{
							*(it->second) = serviceUse;
						}
						else
						{
							simplifiedResultIndex.insert(make_pair(
								reachedVertex
								, simplifiedResult.insert(
									simplifiedResult.begin()
									, serviceUse
									)
								));
						}

						_bestVertexReachesMap.insert (serviceUse);

						if (_destinationVam.contains (reachedVertex))
						{
							_minMaxDateTimeAtDestination = serviceUse.getSecondActualDateTime();
							if (_accessDirection == TO_DESTINATION)
								_minMaxDateTimeAtDestination += _destinationVam.getVertexAccess(reachedVertex).approachTime;
							else
								_minMaxDateTimeAtDestination -= _destinationVam.getVertexAccess(reachedVertex).approachTime;
							/// @todo verify code above
						}
					}

				} // next edge

			} // next vertex in vam

			// Validating all the service uses compared to the final result list
			// HANDLE SIMPLIFIED RESULT INDEX
			simplifiedResult.erase(
				remove_if(simplifiedResult.begin(), simplifiedResult.end(), UselessServiceUse(*this, currentJourney, strictTime))
				, simplifiedResult.end()
				);


			for (SimplifiedResult::iterator it(simplifiedResult.begin()); it != simplifiedResult.end(); ++it)
			{
				ServiceUse serviceUse(*it);
				const Vertex* vertex(serviceUse.getSecondEdge()->getFromVertex());


				//		if (_destinationVam.contains (vertex)

				// Now, prepend each resulting journey with nextCurrentJourney.
				if(	(_searchAddresses == SEARCH_ADDRESSES && vertex->isAddress())
					|| (_searchPhysicalStops == SEARCH_PHYSICALSTOPS && vertex->isPhysicalStop()) 
				){
					Journey newJourney (currentJourney);
					if (_accessDirection == TO_DESTINATION)
						newJourney.append (serviceUse);
					else
						newJourney.prepend(serviceUse);
					_result[vertex] = newJourney;
				}
			}


			// Recursion on each service use

			if (maxDepth > 0)
			{
				// Now iterate on each journey leg and call recursively the integral search
				for (SimplifiedResult::const_iterator itLeg(simplifiedResult.begin()); itLeg != simplifiedResult.end (); ++itLeg)
				{
					const ServiceUse& serviceUse(*itLeg);
					const Vertex* nextVertex(serviceUse.getSecondEdge()->getFromVertex());

					VertexAccessMap nextVam;
					nextVertex->getPlace ()->getImmediateVertices(
						nextVam,
						_accessDirection,
						_accessParameters
						, (_useRoads == USE_ROADS) ? SEARCH_ADDRESSES : DO_NOT_SEARCH_ADDRESSES
						, (_useLines == USE_LINES) ? SEARCH_PHYSICALSTOPS : DO_NOT_SEARCH_PHYSICALSTOPS
						, nextVertex
					);

					Journey nextCurrentJourney (currentJourney);
					nextCurrentJourney.append (serviceUse);

					_integralSearchRecursion(
						nextVam
						, serviceUse.getSecondActualDateTime()
						, nextCurrentJourney
						, maxDepth - 1
						, false
						);
				}
			}
		}

		Journeys IntegralSearcher::integralSearch( const env::VertexAccessMap& vertices  , const time::DateTime& desiredTime  , int maxDepth  , bool strictTime /*= false  */ )
		{
			_result.clear();

			Journey emptyJourney;
			_integralSearchRecursion(
				vertices
				, desiredTime
				, emptyJourney
				, maxDepth
				, strictTime
				);

			Journeys result;
			for (IntegralSearchWorkingResult::const_iterator it(_result.begin()); it != _result.end(); ++it)
				result.push_back(it->second);

			std::sort (result.begin(), result.end(), JourneyLegComparator(_accessDirection));

			return result;
		}

		bool IntegralSearcher::_evaluateServiceUse(
			const env::ServiceUse& serviceUse
			, const Journey& currentJourney
		) const {

			/// <h2>Initialization of local variables</h2>
			const Edge*	fromEdge(serviceUse.getEdge());
			const Vertex* reachedVertex(serviceUse.getSecondEdge()->getFromVertex());
			const DateTime& reachDateTime(serviceUse.getSecondActualDateTime());

			/// <h2>Control of the compliance with the current filters</h2>

			/** - If the edge is an address, the currentJourney necessarily contains
				only road legs, filter on max approach distance (= walk distance).
			*/
			if ((reachedVertex->isAddress ()) &&
				(currentJourney.getDistance () + fromEdge->getLength () > 
				_accessParameters.maxApproachDistance) )
				return false;

			/** - If the edge is an address, the currentJourney necessarily contains
				only road legs, filter on max approach time (= walk time).
			*/
			if ( (reachedVertex->isAddress ()) &&
				(currentJourney.getEffectiveDuration () + 
				(fromEdge->getLength () / _accessParameters.approachSpeed) > 
				_accessParameters.maxApproachTime) )
				return false;

			/// <h2>Determination of the utility to store the service use</h2>

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

				if (serviceUse.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
					bestHopedGoalAccessDateTime += minimalGoalReachDuration;
				else
					bestHopedGoalAccessDateTime -= minimalGoalReachDuration;

				if(	(	(serviceUse.getMethod() == ServicePointer::ARRIVAL_TO_DEPARTURE)
					&&	(bestHopedGoalAccessDateTime < _minMaxDateTimeAtDestination)
					)
				||	(	(serviceUse.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
					&&	(bestHopedGoalAccessDateTime > _minMaxDateTimeAtDestination)
					)
				)	return false;

			}

			/** - Best vertex map control : the service use is useful only if no other already founded
				service use reaches the vertex at a strictly better time.
			*/
			if( (	(serviceUse.getMethod() == ServicePointer::ARRIVAL_TO_DEPARTURE)
				&&	(reachDateTime < _bestVertexReachesMap.getBestTime(reachedVertex, reachDateTime))
				)
			||	(	(serviceUse.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
				&&	(reachDateTime > _bestVertexReachesMap.getBestTime (reachedVertex, reachDateTime))
				)
			)	return false;


			/** - Continuous service breaking test : if the solution is between a service continuous range
				then it is stored only if its duration is better than the one of the continuous service.
			*/
			if (_previousContinuousServiceDuration > 0)
			{
				DateTime departureTime(
					(serviceUse.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
					? (_accessJourney.empty() ? serviceUse.getActualDateTime() : _accessJourney.getDepartureTime())
					: serviceUse.getSecondActualDateTime()
				);
				if (departureTime < _previousContinuousServiceLastDeparture)
				{
					DateTime arrivalTime(
						(serviceUse.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
						? serviceUse.getSecondActualDateTime()
						: (_accessJourney.empty() ? serviceUse.getActualDateTime() : _accessJourney.getArrivalTime())
					);
					if ((arrivalTime - departureTime) >= _previousContinuousServiceDuration)
						return false;
				}
			}

			return true;
		}


		IntegralSearcher::UselessServiceUse::UselessServiceUse(
			const IntegralSearcher& integralSearcher
			, const Journey& currentJourney
			, bool strictTime
		)	: _integralSearcher(integralSearcher)
			, _currentJourney(currentJourney)
			, _strictTime(strictTime)
		{	}

		bool IntegralSearcher::UselessServiceUse::operator() (const env::ServiceUse& serviceUse)
		{
			return !_integralSearcher._evaluateServiceUse(serviceUse, _currentJourney);
		}
	}
}
