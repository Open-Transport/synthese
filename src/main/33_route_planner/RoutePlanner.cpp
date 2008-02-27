
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

#include "RoutePlanner.h"

#include "33_route_planner/IntegralSearcher.h"
#include "33_route_planner/BestVertexReachesMap.h"

#include "15_env/Axis.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Edge.h"
#include "15_env/Line.h"
#include "15_env/Service.h"
#include "15_env/Vertex.h"
#include "15_env/Journey.h"
#include "15_env/VertexAccessMap.h"
#include "15_env/BikeCompliance.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/ReservationRule.h"
#include "15_env/Fare.h"

#include "06_geometry/SquareDistance.h"

#include "01_util/Log.h"

#include <algorithm>
#include <set>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace env;
	using namespace geometry;
	using namespace util;

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
		)	: _accessParameters (accessParameters)
			, _planningOrder (planningOrder)
			, _journeySheetStartTime (journeySheetStartTime)
			, _journeySheetEndTime (journeySheetEndTime)
			, _maxSolutionsNumber(maxSolutionsNumber)
			, _minDepartureTime(TIME_UNKNOWN)
			, _maxArrivalTime(TIME_UNKNOWN)
			, _previousContinuousServiceLastDeparture(TIME_UNKNOWN)
			, _calculationTime(TIME_CURRENT)
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


// --------------------------------------------------------- Journey sheet calculation

		const RoutePlanner::Result& RoutePlanner::computeJourneySheetDepartureArrival()
		{
			_result.clear();

			// Create origin vam from integral search on roads
			JourneysResult originJourneys;
			BestVertexReachesMap bvrmd(TO_DESTINATION, true);
			DateTime dt3(_journeySheetEndTime);
			DateTime dt4(TIME_UNKNOWN);
			IntegralSearcher iso(
				TO_DESTINATION
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, USE_ROADS
				, DO_NOT_USE_LINES
				, originJourneys
				, bvrmd
				, _destinationVam
				, _calculationTime
				, dt3
				, 0
				, dt4
				, std::numeric_limits<int>::max ()
				, false
				, false
				);
			iso.integralSearch (
				_originVam
				, _journeySheetStartTime
			);
			
			VertexAccessMap ovam;
			// Include physical stops from originVam into result of integral search
			// (cos not taken into account in returned journey vector).
			ovam.merge (_originVam, 
				VertexAccessMap::DO_NOT_MERGE_ADDRESSES,
				VertexAccessMap::MERGE_PHYSICALSTOPS);
			
			Journey* candidate(new Journey(TO_DESTINATION));
			for(JourneysResult::ResultSet::const_iterator itoj(originJourneys.getJourneys().begin());
				itoj != originJourneys.getJourneys().end ();
				++itoj
			){
				const Journey& oj = (**itoj);
			
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
					if (!_destinationVam.contains(it->first))
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
					&& oj.isBestThan(*candidate)
				)	*candidate = oj;
			}

			// If a candidate was elected, store it in the result array
			if (!candidate->empty())
				_result.journeys.push_back(candidate);
			else
				delete candidate;

			// Create destination vam from integral search on roads
			JourneysResult destinationJourneys;
			BestVertexReachesMap bvrmo(TO_DESTINATION, true);
			DateTime dt1(_journeySheetStartTime);
			DateTime dt2(TIME_UNKNOWN);
			IntegralSearcher isd(
				FROM_ORIGIN
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, USE_ROADS
				, DO_NOT_USE_LINES
				, destinationJourneys
				, bvrmo
				, _originVam
				, _calculationTime
				, dt1
				, 0
				, dt2
				, std::numeric_limits<int>::max ()
				, false
				, true
			);
			isd.integralSearch (
				_destinationVam
				, _journeySheetEndTime
			);

			VertexAccessMap dvam;
			// Include physical stops from destinationVam into result of integral search
			// (cos not taken into account in returned journey vector).
			dvam.merge (_destinationVam, 
				VertexAccessMap::DO_NOT_MERGE_ADDRESSES,
				VertexAccessMap::MERGE_PHYSICALSTOPS);


			for(JourneysResult::ResultSet::const_iterator itdj(destinationJourneys.getJourneys().begin());
				itdj != destinationJourneys.getJourneys().end ();
				++itdj
			){
				const Journey& j(**itdj);

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
					if (!_originVam.contains(it->first))
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

			if (_result.journeys.empty())
			{
				_previousContinuousServiceDuration = 0;
			}
			else
			{
				const Journey* journey(_result.journeys.front());
				_previousContinuousServiceDuration = journey->getDuration();
				_previousContinuousServiceLastDeparture = journey->getDepartureTime();
				_previousContinuousServiceLastDeparture += journey->getContinuousServiceRange();
			}

			// Time loop
			for(_minDepartureTime = _journeySheetStartTime; 
				(_minDepartureTime <= _journeySheetEndTime
				&&	(_maxSolutionsNumber == UNKNOWN_VALUE 
					|| _maxSolutionsNumber > _result.journeys.size()
					)
				);
			){
				Journey* journey(new Journey(TO_DESTINATION));
				computeRoutePlanningDepartureArrival(*journey, ovam, dvam);
				
				//! <li> If no journey was found and last service is continuous, 
					//! then repeat computation after continuous service range. </li>
				if(	!_result.journeys.empty()
				&&	(_result.journeys.back ()->getContinuousServiceRange () > 0)
				&&	(journey->empty() || journey->getDepartureTime() > _previousContinuousServiceLastDeparture)
				){
					_minDepartureTime = _previousContinuousServiceLastDeparture;
					_minDepartureTime += 1;
					_previousContinuousServiceDuration = 0;
					computeRoutePlanningDepartureArrival (*journey, ovam, dvam);	
				}
				
				if (journey->empty())
					break;				
				
				//! <li>If last continuous service was broken, update its range</li>
				if(	!_result.journeys.empty ()
				&&	(_result.journeys.back ()->getContinuousServiceRange () > 0)
				&&	(journey->getDepartureTime () <= _previousContinuousServiceLastDeparture)
				){
					int duration(journey->getArrivalTime() - _result.journeys.back()->getArrivalTime () - 1);
					_result.journeys.back()->setContinuousServiceRange (duration);
				}

				/*!	<li>En cas de nouveau service continu, enregistrement de valeur pour le calcul de la prochaine solution</li>	*/
				if (journey->getContinuousServiceRange() > 1)
				{
					_previousContinuousServiceDuration = journey->getDuration();
					_previousContinuousServiceLastDeparture = journey->getDepartureTime();
					_previousContinuousServiceLastDeparture += journey->getContinuousServiceRange();
				}
				else
					_previousContinuousServiceDuration = 0;


				_result.journeys.push_back(journey);
				
				_minDepartureTime = journey->getDepartureTime ();
				_minDepartureTime += 1;
			}
			
			return _result;
			
		}


// -------------------------------------------------------------------- Column computing

		void RoutePlanner::computeRoutePlanningDepartureArrival(
			Journey& result
			, const VertexAccessMap& ovam
			, const VertexAccessMap& dvam
		){
			_maxArrivalTime = _journeySheetEndTime;
			_maxArrivalTime.addDaysDuration(7);	/// @todo Replace 7 by a parameter
		    
			// Look for best arrival
			findBestJourney (result, ovam, dvam, _minDepartureTime, false, false);
		    
			if (result.empty() || result.getDepartureTime() > _journeySheetEndTime)
			{
				result.clear();
				return;
			}
		    
			// If a journey was found, try to optimize by delaying departure hour as much as possible.
			// Update bounds
			_minDepartureTime = result.getDepartureTime () -
				ovam.getVertexAccess (result.getOrigin ()->getFromVertex ()).approachTime;
			_maxArrivalTime = result.getArrivalTime () + 
				dvam.getVertexAccess (result.getDestination ()->getFromVertex ()).approachTime;

			result.reverse();
		
			// Look for best departure
			// A revoir il faut surement faire currentJourney = result
			findBestJourney (result, dvam, ovam, _maxArrivalTime, true, true);

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
			env::Journey& result
			, const env::VertexAccessMap& startVam
			, const env::VertexAccessMap& endVam
			, const time::DateTime& startTime
			, bool strictTime
			, bool inverted
		){
			const AccessDirection& accessDirection(result.getMethod());

			JourneysResult todo;
			int integralSerachsNumber(1);
			
			DateTime& bestEndTime((accessDirection == TO_DESTINATION) ? _maxArrivalTime : _minDepartureTime);
			DateTime lastBestEndTime(bestEndTime);
			
			// Initialization of the best vertex reaches map
			BestVertexReachesMap bestVertexReachesMap(accessDirection, strictTime);
			for(VertexAccessMap::VamMap::const_iterator itVertex(startVam.getMap().begin());
				itVertex != startVam.getMap().end();
				++itVertex
			){
				bestVertexReachesMap.insert(
					itVertex->first
					, (accessDirection == TO_DESTINATION)
						? _minDepartureTime + itVertex->second.approachTime
						: _maxArrivalTime - itVertex->second.approachTime
				);
			}
			for(VertexAccessMap::VamMap::const_iterator itVertex(endVam.getMap().begin());
				itVertex != endVam.getMap ().end ();
				++itVertex
			){
				bestVertexReachesMap.insert(
					itVertex->first
					, (accessDirection == TO_DESTINATION)
						? _maxArrivalTime - itVertex->second.approachTime
						: _minDepartureTime + itVertex->second.approachTime
				);
			}

			// Initialization of the integral searcher
			IntegralSearcher is(
				accessDirection
				, _accessParameters
				, DO_NOT_SEARCH_ADDRESSES
				, SEARCH_PHYSICALSTOPS
				, DO_NOT_USE_ROADS
				, USE_LINES
				, todo
				, bestVertexReachesMap
				, endVam
				, _calculationTime
				, bestEndTime
				, _previousContinuousServiceDuration
				, _previousContinuousServiceLastDeparture
				, 0
				, strictTime
				, inverted
				);

			is.integralSearch(startVam, startTime, strictTime);
			
			while(true)
			{

				// Take into account of the end reached journeys
				for(JourneysResult::ResultSet::const_iterator it(todo.getJourneys().begin());
					it != todo.getJourneys().end();
				){
					JourneysResult::ResultSet::const_iterator next(it);
					++next;
					const Journey& journey(**it);
					
					if (!journey.getEndReached())
						break;
					
					// A destination without any approach time stops the recursion
					const Vertex* reachedVertex(journey.getEndEdge()->getFromVertex());
					const VertexAccess& va = endVam.getVertexAccess(reachedVertex);
					
					// Attempt to elect the solution as the result
					bool saved(journey.isBestThan(result));
					if (saved)
						result = journey;

					if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
						Log::GetInstance().trace(reachedVertex->getConnectionPlace()->getFullName() + " was found at " + journey.getEndTime().toString() + (saved ? " (accepted)" : " (rejected)"));

					if (va.approachTime == 0)
						todo.remove(*it);
										
					it = next;
				}

				todo.cleanup(lastBestEndTime != bestEndTime, bestEndTime, bestVertexReachesMap);

				// Loop exit
				if (todo.empty())
					break;

				if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
					todo.log();

				lastBestEndTime = bestEndTime;

				const Journey* journey(todo.front());

				is.integralSearch(*journey);
				++integralSerachsNumber;

				delete journey;
			}

			if (Log::GetInstance().getLevel() <= Log::LEVEL_TRACE)
				Log::GetInstance().trace("End of findJourney computing. Was made with " + Conversion::ToString(integralSerachsNumber) + " integral searches.");

		}

		RoutePlanner::Result::~Result()
		{
			clear();
		}

		void RoutePlanner::Result::clear()
		{
			for(JourneyBoardJourneys::iterator it(journeys.begin()); it != journeys.end(); ++it)
				delete *it;
		}
	}
}
