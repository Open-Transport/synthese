
/** RoutePlanner class implementation.
	@file RoutePlanner.cpp

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

#include "33_route_planner/RoutePlanner.h"
#include "33_route_planner/IntegralSearcher.h"

#include "15_env/Axis.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Edge.h"
#include "15_env/Line.h"
#include "15_env/Service.h"
#include "15_env/SquareDistance.h"
#include "15_env/Vertex.h"
#include "15_env/Journey.h"
#include "15_env/VertexAccessMap.h"
#include "15_env/BikeCompliance.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/ReservationRule.h"
#include "15_env/Fare.h"

#include <algorithm>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace env;

	namespace routeplanner
	{

		RoutePlanner::RoutePlanner(
			const Place* origin,
			const Place* destination,
			const AccessParameters& accessParameters,
			const PlanningOrder& planningOrder,
			const DateTime& journeySheetStartTime,
			const DateTime& journeySheetEndTime
			, int maxSolutionsNumber
		)	: _origin (origin)
			, _destination (destination)
			, _accessParameters (accessParameters)
			, _planningOrder (planningOrder)
			, _journeySheetStartTime (journeySheetStartTime)
			, _journeySheetEndTime (journeySheetEndTime)
			, _maxSolutionsNumber(maxSolutionsNumber)
		{
			origin->getImmediateVertices (
				_originVam
				, TO_DESTINATION
				, accessParameters
				, SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
			);
			destination->getImmediateVertices(
				_destinationVam
				, FROM_ORIGIN
				, accessParameters
				, SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
			);
		}


		   
		RoutePlanner::~RoutePlanner ()
		{
		}



		const Place* 
		RoutePlanner::getOrigin () const
		{
			return _origin;
		}



		const Place* 
		RoutePlanner::getDestination () const
		{
			return _destination;
		}


// --------------------------------------------------------- Journey sheet calculation

		Journeys RoutePlanner::computeJourneySheetDepartureArrival()
		{
			Journey journey;
			Journeys result;

			// Create origin vam from integral search on roads
			_bestVertexReachesMap.clear(TO_DESTINATION);
			IntegralSearcher iso(
				TO_DESTINATION
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, USE_ROADS
				, DO_NOT_USE_LINES
				, _bestVertexReachesMap
				, _destinationVam
				, _calculationTime
				, DateTime(_journeySheetEndTime)
				, 0
				, DateTime(TIME_UNKNOWN)
				);
			Journeys originJourneys(iso.integralSearch (
				_originVam
				, Journey(TO_DESTINATION)
				, _journeySheetStartTime
				, std::numeric_limits<int>::max ()
				));
			
			VertexAccessMap ovam;
			// Include physical stops from originVam into result of integral search
			// (cos not taken into account in returned journey vector).
			ovam.merge (_originVam, 
				VertexAccessMap::DO_NOT_MERGE_ADDRESSES,
				VertexAccessMap::MERGE_PHYSICALSTOPS);
			
			Journey candidate;
			for (Journeys::const_iterator itoj = originJourneys.begin ();
			 itoj != originJourneys.end (); ++itoj)
			{
				const Journey& oj = (*itoj);
			
				// Store each reached physical stop with full approach time addition :
				//	- approach time in departure place
				//	- duration of the approach journey
				//	- transfer delay between approach journey end address and physical stop
				int commonApproachTime(
					_originVam.getVertexAccess(oj.getOrigin()->getFromVertex()).approachTime
					+ oj.getDuration ()
				);
				int commonApproachDistance(
					_originVam.getVertexAccess(oj.getOrigin()->getFromVertex()).approachDistance
					+ oj.getDistance ()
				);
				VertexAccessMap vam;
				const ConnectionPlace* cp(oj.getDestination()->getConnectionPlace());
				const Vertex* v(oj.getDestination()->getFromVertex());
				cp->getImmediateVertices(
					vam
					, TO_DESTINATION
					, _accessParameters
					, DO_NOT_SEARCH_ADDRESSES
					, SEARCH_PHYSICALSTOPS
					, v
				);
				for (VertexAccessMap::VamMap::const_iterator it(vam.getMap().begin()); it != vam.getMap().end(); ++it)
				{
					ovam.insert(
						it->first
						, VertexAccess(
							commonApproachTime + cp->getTransferDelay(v, it->first)
							, commonApproachDistance
							, oj
						)
					);
				}

				// Store the journey as a candidate if it goes directly to the destination
				if(	_destinationVam.contains(oj.getDestination()->getFromVertex())
					&& oj.isBestThan(candidate)
				)	candidate = oj;
			}

			// If a candidate was elected, store it in the result array
			if (!candidate.empty())
				result.push_back(candidate);


			// Create destination vam from integral search on roads
			_bestVertexReachesMap.clear(FROM_ORIGIN);
			IntegralSearcher isd(
				FROM_ORIGIN
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, USE_ROADS
				, DO_NOT_USE_LINES
				, _bestVertexReachesMap
				, _originVam
				, _calculationTime
				, DateTime(_journeySheetStartTime)
				, 0
				, DateTime(TIME_UNKNOWN)
				);
			Journeys destinationJourneys(isd.integralSearch (
					_destinationVam
					, Journey(FROM_ORIGIN)
					, _journeySheetEndTime
					, std::numeric_limits<int>::max ()
					));

			VertexAccessMap dvam;
			// Include physical stops from destinationVam into result of integral search
			// (cos not taken into account in returned journey vector).
			dvam.merge (_destinationVam, 
				VertexAccessMap::DO_NOT_MERGE_ADDRESSES,
				VertexAccessMap::MERGE_PHYSICALSTOPS);


			for (Journeys::const_iterator itdj = destinationJourneys.begin ();
			 itdj != destinationJourneys.end (); ++itdj)
			{
				const Journey& j(*itdj);

				// Store each reached physical stop with full approach time addition :
				//	- approach time in destination place
				//	- duration of the approach journey
				//	- transfer delay between approach journey end address and physical stop
				int commonApproachTime(
					_destinationVam.getVertexAccess(j.getDestination()->getFromVertex()).approachTime
					+ j.getDuration ()
				);
				int commonApproachDistance(
					_destinationVam.getVertexAccess(j.getDestination()->getFromVertex()).approachDistance
					+ j.getDistance ()
				);
				VertexAccessMap vam;
				const ConnectionPlace* cp(j.getOrigin()->getConnectionPlace());
				const Vertex* v(j.getOrigin()->getFromVertex());
				cp->getImmediateVertices(
					vam
					, FROM_ORIGIN
					, _accessParameters
					, DO_NOT_SEARCH_ADDRESSES
					, SEARCH_PHYSICALSTOPS
					, v
					);
				for (VertexAccessMap::VamMap::const_iterator it(vam.getMap().begin()); it != vam.getMap().end(); ++it)
				{
					dvam.insert(
						it->first
						, VertexAccess(
							commonApproachTime + cp->getTransferDelay(it->first, v)
							, commonApproachDistance
							, j
						)
					);
				}
			}


			_previousContinuousServiceDuration = 0;
			// Time loop
			for(_minDepartureTime = _journeySheetStartTime; 
				(_minDepartureTime < _journeySheetEndTime
				&&	(_maxSolutionsNumber == UNKNOWN_VALUE 
					|| _maxSolutionsNumber > result.size()
					)
				);
			){
				journey.clear();
				computeRoutePlanningDepartureArrival (journey, ovam, dvam);
				
				//! <li> If no journey was found and last service is continuous, 
					//! then repeat computation after continuous service range. </li>
				if ( (journey.empty()) &&
					 (result.empty () == false) && 
					 (result.back ().getContinuousServiceRange () > 0) )
				{
					_minDepartureTime = _previousContinuousServiceLastDeparture;
					_minDepartureTime += 1;
					_previousContinuousServiceDuration = 0;
					computeRoutePlanningDepartureArrival (journey, ovam, dvam);	
				}
				
				if (journey.empty())
					break;				
				
				//! <li>If last continuous service was broken, update its range</li>
				if ( (result.empty () == false) &&
					 (result.back ().getContinuousServiceRange () > 0) &&
					 (journey.getDepartureTime () <= _previousContinuousServiceLastDeparture) )
				{
					int duration = journey.getArrivalTime () - result.back ().getArrivalTime () - 1;
					result.back ().setContinuousServiceRange (duration);
				}

				/*!	<li>En cas de nouveau service continu, enregistrement de valeur pour le calcul de la prochaine solution</li>	*/
				if (journey.getContinuousServiceRange() > 1)
				{
					_previousContinuousServiceDuration = journey.getArrivalTime() - journey.getDepartureTime();
					_previousContinuousServiceLastDeparture = journey.getDepartureTime();
					_previousContinuousServiceLastDeparture += journey.getContinuousServiceRange();
				}
				else
					_previousContinuousServiceDuration = 0;


				result.push_back (journey);
				
				_minDepartureTime = journey.getDepartureTime ();
				_minDepartureTime += 1;
			}
			
			return result;
			
		}


// -------------------------------------------------------------------- Column computing

		void RoutePlanner::computeRoutePlanningDepartureArrival(
			Journey& result
			, const VertexAccessMap& ovam
			, const VertexAccessMap& dvam
		){
			_bestVertexReachesMap.clear (TO_DESTINATION);
			_maxArrivalTime = _journeySheetEndTime;
			_maxArrivalTime.addDaysDuration(7);	/// @todo Replace 7 by a parameter
		    
			for(map<const Vertex*, VertexAccess>::const_iterator itVertex(dvam.getMap().begin());
				itVertex != dvam.getMap ().end ();
				++itVertex
			){
				_bestVertexReachesMap.insert (itVertex->first, 
								 _maxArrivalTime - itVertex->second.approachTime);
			}
			for(map<const Vertex*, VertexAccess>::const_iterator itVertex(ovam.getMap().begin());
				itVertex != ovam.getMap().end();
				++itVertex
			){
				_bestVertexReachesMap.insert (itVertex->first, 
								 _minDepartureTime + itVertex->second.approachTime);
			}
		    

			// Look for best arrival
			findBestJourney (result, ovam, dvam, Journey(TO_DESTINATION), false);
		    
			if (result.empty() || result.getDepartureTime() > _journeySheetEndTime)
			{
				result.clear();
				return;
			}
		    
			// If a journey was found, try to optimize by delaying departure hour as much as possible.
			_bestVertexReachesMap.clear (FROM_ORIGIN);

			const JourneyLegs& jl(result.getJourneyLegs());
			for (JourneyLegs::const_iterator it(jl.begin()); it != jl.end(); ++it)
			{
				_bestVertexReachesMap.insert (
					it->getDepartureEdge()->getFromVertex (),
					it->getDepartureDateTime ()
				);
			}
		    
			for(map<const Vertex*, VertexAccess>::const_iterator itVertex(dvam.getMap().begin());
				itVertex != dvam.getMap().end ();
				++itVertex
			){
				_bestVertexReachesMap.insert (
					itVertex->first, 
					result.getArrivalTime () + 
					dvam.getVertexAccess (result.getDestination ()->getFromVertex ()).approachTime - 
					itVertex->second.approachTime);
			}
			for(map<const Vertex*, VertexAccess>::const_iterator itVertex(ovam.getMap().begin());
				itVertex != ovam.getMap().end();
				++itVertex
			){
				_bestVertexReachesMap.insert (
					itVertex->first, 
					result.getDepartureTime () -
					ovam.getVertexAccess (result.getOrigin ()->getFromVertex ()).approachTime + 
					itVertex->second.approachTime);
			}
		    
			// Update bounds
			_minDepartureTime = result.getDepartureTime () -
			ovam.getVertexAccess (result.getOrigin ()->getFromVertex ()).approachTime;
			_maxArrivalTime = result.getArrivalTime () + 
			dvam.getVertexAccess (result.getDestination ()->getFromVertex ()).approachTime;

			result.reverse();
		
			// Look for best departure
			// A revoir il faut surement faire currentJourney = result
			findBestJourney (result, dvam, ovam, Journey(FROM_ORIGIN), true);

			if (result.getDepartureTime() > _journeySheetEndTime)
			{
				result.clear();
				return;
			}

			// Inclusion of approach journeys
			result.setStartApproachDuration(0);
			result.setEndApproachDuration(0);

			Journey goalApproachJourney(dvam.getVertexAccess(result.getDestination()->getFromVertex()).approachJourney);
			if (!goalApproachJourney.empty())
			{
				goalApproachJourney.shift(
					(result.getArrivalTime() - goalApproachJourney.getDepartureTime())
					+ goalApproachJourney.getDestination()->getFromVertex()->getConnectionPlace()->getTransferDelay(
					result.getDestination()->getFromVertex()
					, goalApproachJourney.getOrigin()->getFromVertex()
					)
					, result.getContinuousServiceRange()
					);
				goalApproachJourney.setContinuousServiceRange(result.getContinuousServiceRange());
				result.append(goalApproachJourney);
			}

			result.reverse();
			Journey originApproachJourney(ovam.getVertexAccess(result.getOrigin()->getFromVertex()).approachJourney);
			if (!originApproachJourney.empty())
			{
				originApproachJourney.shift(
					(result.getDepartureTime() - originApproachJourney.getDepartureTime())
					- originApproachJourney.getDuration()
					- originApproachJourney.getDestination()->getFromVertex()->getConnectionPlace()->getTransferDelay(
					originApproachJourney.getDestination()->getFromVertex()
					, result.getOrigin()->getFromVertex()
					)
					, result.getContinuousServiceRange()
					);
				originApproachJourney.setContinuousServiceRange(result.getContinuousServiceRange());
				result.prepend(originApproachJourney);
			}
		}

// -------------------------------------------------------------------------- Recursion

		void RoutePlanner::findBestJourney(
			Journey& result
		   , const VertexAccessMap& ovam
		   , const VertexAccessMap& dvam
		   , const Journey& currentJourney
		   , bool strictTime
		){
			if (currentJourney.getJourneyLegCount () > 
			_accessParameters.maxTransportConnectionCount) return;

			const AccessDirection& accessDirection(currentJourney.getMethod());

			IntegralSearcher is(
				accessDirection
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, DO_NOT_USE_ROADS
				, USE_LINES
				, _bestVertexReachesMap
				, dvam
				, _calculationTime
				, (accessDirection == TO_DESTINATION) ? _maxArrivalTime : _minDepartureTime
				, _previousContinuousServiceDuration
				, _previousContinuousServiceLastDeparture
				);
			Journeys journeyParts(is.integralSearch(
				ovam
				, currentJourney
				, (accessDirection == TO_DESTINATION)
					? (currentJourney.empty() ? _minDepartureTime : currentJourney.getArrivalTime())
					: (currentJourney.empty() ? _maxArrivalTime : currentJourney.getDepartureTime())
				, 0
				, strictTime
				));

			for (Journeys::const_iterator itj = journeyParts.begin ();
			 itj != journeyParts.end (); ++itj)
			{
				if (!is.evaluateJourney(*itj))
					continue;

				bool recursion(true);
				
				// Case the journey goes to a final destination
				const Vertex* reachedVertex(itj->getEndEdge()->getFromVertex());

				if (dvam.contains(reachedVertex))
				{
					// A destination without any approach time stops the recursion
					const VertexAccess& va = dvam.getVertexAccess(reachedVertex);
					if (va.approachTime == 0)
						recursion = false;
					
					// Attempt to elect the solution as the result
					if (itj->isBestThan(result))
						result = *itj;
				}
			

				// Recursion if needed
				if (recursion)
				{
					Journey recursiveCandidate(accessDirection);
					
					VertexAccessMap nextVam;
					reachedVertex->getPlace()->getImmediateVertices(
						nextVam,
						accessDirection,
						_accessParameters
						, DO_NOT_SEARCH_ADDRESSES
						, SEARCH_PHYSICALSTOPS
						, reachedVertex
					);

					findBestJourney (recursiveCandidate, nextVam, dvam, *itj, false);

					// Attempt to elect the solution as the result
					if (recursiveCandidate.isBestThan(result))
						result = recursiveCandidate;
				}
			}
		}
	}
}
