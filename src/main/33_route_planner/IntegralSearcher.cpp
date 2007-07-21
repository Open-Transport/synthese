
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
						edge->getNextService (
						departureMoment
						, _minMaxDateTimeAtDestination
						, _calculationTime
						)	);

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

					for (const Edge* curEdge = (edge->*step) ();
						curEdge != 0; curEdge = (curEdge->*step) ())
					{
						ServiceUse serviceUse(serviceInstance, curEdge);
						const Vertex* reachedVertex(serviceUse.getSecondEdge()->getFromVertex());

						if (!_evaluateServiceUse(serviceUse,currentJourney,strictTime))
							break;

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
						_accessParameters,
						nextVertex,
						_useRoads,
						_useLines);

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
			, bool strictTime
		) const {
			//			if (arrivalEdge == 0) return true;

			// Initialization
			const Edge*	fromEdge = serviceUse.getEdge();
			const Vertex* goalVertex = serviceUse.getSecondEdge()->getFromVertex ();
			const DateTime& goalDateTime(serviceUse.getSecondActualDateTime());
			SquareDistance sqd;


			// If the edge is an address, the currentJourney necessarily contains
			// only road legs, filter on max approach distance (= walk distance).
			if ( (goalVertex->isAddress ()) &&
				(currentJourney.getDistance () + fromEdge->getLength () > 
				_accessParameters.maxApproachDistance) )
				return false;


			// If the edge is an address, the currentJourney necessarily contains
			// only road legs, filter on max approach time (= walk time).
			if ( (goalVertex->isAddress ()) &&
				(currentJourney.getEffectiveDuration () + 
				(fromEdge->getLength () / _accessParameters.approachSpeed) > 
				_accessParameters.maxApproachTime) )
				return false;



			// The vertex is considered useful if it allows a soon access for arrival
			// or late access for departure.
			if (sqd.getSquareDistance () == UNKNOWN_VALUE)
			{
				sqd.setFromPoints (*goalVertex, _destinationVam.getIsobarycenter ());  
				sqd.setSquareDistance (sqd.getSquareDistance () - 
					_destinationVam.getIsobarycenterMaxSquareDistance ().getSquareDistance ());

			}

			// Check that the limit time (min or max) is not exceeded
			DateTime accessMoment (goalDateTime);
			if (!_destinationVam.contains(goalVertex) && goalVertex->isConnectionAllowed())
			{
				if (serviceUse.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
					accessMoment += goalVertex->getConnectionPlace ()->getMinTransferDelay ();
				else
					accessMoment -= goalVertex->getConnectionPlace ()->getMinTransferDelay ();
			}

			if ( (serviceUse.getMethod() == ServicePointer::ARRIVAL_TO_DEPARTURE) &&
				(accessMoment < _minMaxDateTimeAtDestination) )
				return false;

			if ( (serviceUse.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL) &&
				(accessMoment > _minMaxDateTimeAtDestination) )
				return false;

			/// @todo : re-implement VMax control.

			/// @todo Reimplement continuous service break
			// Continuous service breaking
			/*			if (_previousContinuousServiceDuration)
			{
			if ( (currentJourney.getJourneyLegCount () > 0) &&
			(currentJourney.getDepartureTime () <= _previousContinuousServiceLastDeparture) &&
			(arrivalMoment - currentJourney.getDepartureTime () >= _previousContinuousServiceDuration) )
			{
			return false;
			}
			else if ( (departureMoment < _previousContinuousServiceLastDeparture) && 
			(arrivalMoment - departureMoment >= _previousContinuousServiceDuration) )
			{
			return false;
			}
			}
			*/		    


			// Best vertex map control
			if ( (serviceUse.getMethod() == ServicePointer::ARRIVAL_TO_DEPARTURE) &&
				(accessMoment < _bestVertexReachesMap.getBestTime (goalVertex, accessMoment)) )
				return false;

			if ( (serviceUse.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL) &&
				(accessMoment > _bestVertexReachesMap.getBestTime (goalVertex, accessMoment)) )
				return false;


			// Strict time control
			if ( strictTime && 
				(goalDateTime != _bestVertexReachesMap.getBestTime (goalVertex, goalDateTime) )) 
				return false;

			//			return arrivalMoment <= (_maxArrivalTime - _destinationVam.getMinApproachTime ());

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
			return !_integralSearcher._evaluateServiceUse(serviceUse, _currentJourney, _strictTime);
		}
	}
}
