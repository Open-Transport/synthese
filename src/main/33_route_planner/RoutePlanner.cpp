
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
#include "33_route_planner/Journey.h"
#include "33_route_planner/JourneyLeg.h"

#include "15_env/Axis.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Edge.h"
#include "15_env/Line.h"
#include "15_env/Service.h"
#include "15_env/SquareDistance.h"
#include "15_env/Vertex.h"

#include "15_env/VertexAccessMap.h"

#include "15_env/BikeCompliance.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/ReservationRule.h"
#include "15_env/Fare.h"

#include <algorithm>

namespace synthese
{
	using namespace time;
	using namespace env;

	namespace routeplanner
	{

		RoutePlanner::RoutePlanner (const Place* origin,
						const Place* destination,
						const AccessParameters& accessParameters,
						const PlanningOrder& planningOrder,
						const DateTime& journeySheetStartTime,
						const DateTime& journeySheetEndTime)
			: _origin (origin)
			, _destination (destination)
			, _accessParameters (accessParameters)
			, _planningOrder (planningOrder)
			, _journeySheetStartTime (journeySheetStartTime)
			, _journeySheetEndTime (journeySheetEndTime)
			, _bestDepartureVertexReachesMap (FROM_ORIGIN)
			, _bestArrivalVertexReachesMap (TO_DESTINATION)
			, _journeyLegComparatorForBestArrival (TO_DESTINATION)
			, _journeyLegComparatorForBestDeparture (FROM_ORIGIN)

		{
			origin->getImmediateVertices (_originVam, TO_DESTINATION, accessParameters);
			destination->getImmediateVertices (_destinationVam, FROM_ORIGIN, accessParameters);
		    
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







		bool 
		RoutePlanner::areAxisContraintsFulfilled (const synthese::env::Path* path, 
							  const Journey& journey) const
		{
			// Check if axis is allowed.
			if (path->getAxis () && (path->getAxis ()->isAllowed () == false)) 
			{
			return false;
			}

			// Check axis against already followed axes
			if ( path->getAxis () && 
			 (path->getAxis ()->isFree () == false) &&
			 (journey.getJourneyLegCount () > 0) )
			{
			for (int i=0; i<journey.getJourneyLegCount (); ++i)
			{
				if (journey.getJourneyLeg (i)->getAxis () == path->getAxis ()) 
				{
				return false;
				}
			}
			}
			return true;
		}



		bool 
		RoutePlanner::isPathCompliant (const Path* path) const
		{
			if (_accessParameters.bikeCompliance && path->getBikeCompliance () &&
				path->getBikeCompliance ()->isCompliant () == false
			){
				return false;
			}

			if (_accessParameters.handicappedCompliance && path->getHandicappedCompliance () &&
				path->getHandicappedCompliance ()->isCompliant () == false
			){
				return false;
			}

			if (_accessParameters.pedestrianCompliance && path->getPedestrianCompliance ()
				&& path->getPedestrianCompliance ()->isCompliant () == false
			){
				return false;
			}

			if (_accessParameters.withReservation && path->getReservationRule () &&
				path->getReservationRule ()->getType () != ReservationRule::RESERVATION_TYPE_COMPULSORY
			){
				return false;
			}

			// TODO : fare testing...

			return true;
		}



		bool 
		RoutePlanner::isServiceCompliant (const Service* service) const
		{

			if (_accessParameters.bikeCompliance &&
			service->getBikeCompliance ()->isCompliant () == false)
			{
			return false;
			}

			if (_accessParameters.handicappedCompliance &&
			service->getHandicappedCompliance ()->isCompliant () == false)
			{
			return false;
			}

			if (_accessParameters.handicappedCompliance &&
			service->getHandicappedCompliance ()->isCompliant () == false)
			{
			return false;
			}

			if (_accessParameters.pedestrianCompliance &&
			service->getPedestrianCompliance ()->isCompliant () == false)
			{
			return false;
			}

			return true;
		}








		bool
		RoutePlanner::isVertexUseful (const Vertex* vertex,
						  const DateTime& dateTime,
						  const AccessDirection& accessDirection,
						  SquareDistance& sqd) const
		{
			// The vertex is considered useful if it allows a soon access for arrival
			// or late access for departure.

			const VertexAccessMap& vam = (accessDirection == TO_DESTINATION) 
			? _destinationVam 
			: _originVam ;
		    
			if (sqd.getSquareDistance () == UNKNOWN_VALUE)
			{
			sqd.setFromPoints (*vertex, vam.getIsobarycenter ());  
			sqd.setSquareDistance (sqd.getSquareDistance () - 
						   vam.getIsobarycenterMaxSquareDistance ().getSquareDistance ());
			
			}
		    
			// Check that the limit time (min or max) is not exceeded
			DateTime accessMoment (dateTime);
		    
			if ((vam.contains (vertex) && (vertex->getConnectionPlace ())))
			{
			accessMoment += ((accessDirection == TO_DESTINATION) ? 1 : -1) *
				vertex->getConnectionPlace ()->getMinTransferDelay ();
			}
			
		    
			if ( (accessDirection == FROM_ORIGIN) &&
			 (accessMoment < _minDepartureTime) ) return false;

			if ( (accessDirection == TO_DESTINATION) &&
			 (accessMoment > _maxArrivalTime) ) return false;

			// TODO : re-implement VMax control.

			return true;
		    
		}







		bool 
		RoutePlanner::evaluateArrival (const Edge* arrivalEdge,
						   const DateTime& departureMoment,
						   const Edge* departureEdge,
						   const Service* service,
						   std::deque<JourneyLeg*>& journeyPart,
						   const Journey& currentJourney,
						   bool strictTime,
						   int continuousServiceRange)
		{
			if (arrivalEdge == 0) return true;

			const Vertex* arrivalVertex = arrivalEdge->getFromVertex ();
		    
			// If the edge is an address, the currentJourney necessarily contains
			// only road legs, filter on max approach distance (= walk distance).
			if ( (arrivalVertex->isAddress ()) &&
			 (currentJourney.getDistance () + departureEdge->getLength () > 
			  _accessParameters.maxApproachDistance) ) return false;


			// If the edge is an address, the currentJourney necessarily contains
			// only road legs, filter on max approach time (= walk time).
			if ( (arrivalVertex->isAddress ()) &&
			 (currentJourney.getEffectiveDuration () + 
			  (departureEdge->getLength () / _accessParameters.approachSpeed) > 
			  _accessParameters.maxApproachTime) ) return false;


			DateTime arrivalMoment = departureMoment;
			arrivalEdge->calculateArrival (*departureEdge, 
						  service->getServiceNumber (), 
						  departureMoment, 
						  arrivalMoment);


			SquareDistance sqd;
			if (isVertexUseful (arrivalVertex,
					arrivalMoment,
					TO_DESTINATION,
					sqd) == false)
			{
			return false;
			}


			// Continuous service breaking
			if (_previousContinuousServiceDuration)
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
		    



			// Add a journey leg if necessary
			if ( (arrivalMoment < _bestArrivalVertexReachesMap.getBestTime (arrivalVertex, arrivalMoment)) ||
			 (strictTime && 
			  (arrivalMoment == _bestArrivalVertexReachesMap.getBestTime (arrivalVertex, arrivalMoment) )) )
			{

			
			JourneyLeg* journeyLeg = 0;
			if (_bestArrivalVertexReachesMap.contains (arrivalVertex) == false)
			{
				journeyLeg = new JourneyLeg ();
				journeyLeg->setOrigin (departureEdge);
				journeyLeg->setDestination (arrivalEdge);
				journeyLeg->setDepartureTime (departureMoment);
				journeyLeg->setArrivalTime (arrivalMoment);
				journeyLeg->setService (service);
				journeyLeg->setContinuousServiceRange (continuousServiceRange);
				journeyLeg->setSquareDistance (sqd);

				journeyPart.push_front (journeyLeg);
				_bestArrivalVertexReachesMap.insert (arrivalVertex, journeyLeg);
			}
			else
			{
				journeyLeg->setOrigin (departureEdge);
				journeyLeg->setDepartureTime (departureMoment);
				journeyLeg->setArrivalTime (arrivalMoment);
				journeyLeg->setService (service);
				journeyLeg->setContinuousServiceRange (continuousServiceRange);
			}

			
			if (_destinationVam.contains (arrivalVertex))
			{
				_maxArrivalTime = arrivalMoment;
				_maxArrivalTime += _destinationVam.getVertexAccess (arrivalVertex).approachTime;
			}
			
			}
			return arrivalMoment <= (_maxArrivalTime - _destinationVam.getMinApproachTime ());

		}
						    





		/* TODO


		bool 
		RoutePlanner::evaluateDeparture (const Edge* departureEdge,
						 const DateTime& departureMoment,
						 const Edge* arrivalEdge,
						 const AccessDirection& accessDirection,
						 const Service* service,
						 std::deque<JourneyLeg*>& journeyPart,
						 const Journey& currentJourney,
						 bool strictTime,
						 int continuousServiceRange)
		{
			if (departureEdge == 0) return true;

			const Vertex* departureVertex = departureEdge->getFromVertex ();
		    
			// If the edge is an address, the currentJourney necessarily contains
			// only road legs, filter on max approach distance (= walk distance).
			if ( (departureVertex->isAddress ()) &&
			 (currentJourney.getDistance () + arrivalEdge->getLength () > 
			  _accessParameters.maxApproachDistance) ) return false;


			// If the edge is an address, the currentJourney necessarily contains
			// only road legs, filter on max approach time (= walk time).
			if ( (departureVertex->isAddress ()) &&
			 (currentJourney.getEffectiveDuration () + 
			  (arrivalEdge->getLength () / _accessParameters.approachSpeed) > 
			  _accessParameters.maxApproachTime) ) return false;


			DateTime arrivalMoment = departureMoment;
			departureEdge->calculateArrival (*arrivalEdge, 
						  service->getServiceNumber (), 
						  departureMoment, 
						  arrivalMoment);


			SquareDistance sqd;
			if (isVertexUseful (departureVertex,
					arrivalMoment,
					accessDirection, sqd) == false)
			{
			return false;
			}



			// Continuous service breaking
			if (_previousContinuousServiceDuration)
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
		    



			// Add a journey leg if necessary
			if ( (arrivalMoment < _bestArrivalVertexReachesMap.getBestTime (departureVertex, arrivalMoment)) ||
			 (strictTime && 
			  (arrivalMoment == _bestArrivalVertexReachesMap.getBestTime (departureVertex, arrivalMoment) )) )
			{

			
			JourneyLeg* journeyLeg = 0;
			if (_bestArrivalVertexReachesMap.contains (departureVertex) == false)
			{
				journeyLeg = new JourneyLeg ();
				journeyLeg->setOrigin (arrivalEdge);
				journeyLeg->setDestination (departureEdge);
				journeyLeg->setDepartureTime (departureMoment);
				journeyLeg->setArrivalTime (arrivalMoment);
				journeyLeg->setService (service);
				journeyLeg->setContinuousServiceRange (continuousServiceRange);
				journeyLeg->setSquareDistance (sqd);

				journeyPart.push_front (journeyLeg);
				_bestArrivalVertexReachesMap.insert (departureVertex, journeyLeg);
			}
			else
			{
				journeyLeg->setOrigin (arrivalEdge);
				journeyLeg->setDepartureTime (departureMoment);
				journeyLeg->setArrivalTime (arrivalMoment);
				journeyLeg->setService (service);
				journeyLeg->setContinuousServiceRange (continuousServiceRange);
			}

			
			if (_destinationVam.contains (departureVertex))
			{
				_maxArrivalTime = arrivalMoment;
				_maxArrivalTime += _destinationVam.getVertexAccess (departureVertex).approachTime;
			}
			
			}
			return arrivalMoment <= (_maxArrivalTime - _destinationVam.getMinApproachTime ());

		}
						    



		*/


		Journeys 
		RoutePlanner::integralSearch (const VertexAccessMap& vam, 
						  const DateTime& desiredTime,
						  const AccessDirection& accessDirection,
						  const Journey* currentJourney,
						  int maxDepth,
						  SearchAddresses searchAddresses, 
						  SearchPhysicalStops searchPhysicalStops,
						  UseRoads useRoads,
						  UseLines useLines,
						  bool strictTime)
		{
			std::deque<JourneyLeg*> journeyPart;

			// TODO : the whole other way depending on accessdirection !!
			// What follows is in case TO_DESTINATION only

			for (std::map<const Vertex*, VertexAccess>::const_iterator itVertex = vam.getMap ().begin ();
			 itVertex != vam.getMap ().end (); ++itVertex)
			{
			const Vertex* origin = itVertex->first;
			
			const std::set<const Edge*>& edges = origin->getDepartureEdges ();

			for (std::set<const Edge*>::const_iterator itEdge = edges.begin ();
				 itEdge != edges.end () ; ++itEdge)
			{
				const Edge* edge = (*itEdge);

				if (isPathCompliant (edge->getParentPath ()) == false) continue;

				// TODO : reintroduce optimization on following axis departure/arrival ?
				if (areAxisContraintsFulfilled (edge->getParentPath (), *currentJourney) == false) continue;

				int continuousServiceRange = 0;
				int serviceNumber = 0;
			    
				DateTime departureMoment = desiredTime;
				departureMoment += (int) itVertex->second.approachTime;
			    
				serviceNumber = edge->getNextService (departureMoment, 
								  _maxArrivalTime,
								  _calculationTime);
			    
				if (serviceNumber == UNKNOWN_VALUE) continue;
				if (strictTime && departureMoment != desiredTime) continue;
			    
				const Service* service = edge->getParentPath ()->getService (serviceNumber);
			    
				// Check for service compliancy rules.
				if (isServiceCompliant (service) == false) continue;
			    
				if ( service->isContinuous () )
				{
				if ( departureMoment > edge->getDepartureEndSchedule (serviceNumber) )
				{
					continuousServiceRange = 
					60*24 - ( departureMoment.getHour() - 
						  edge->getDepartureEndSchedule (serviceNumber).getHour() );
				}
				else
				{
					continuousServiceRange = 
					edge->getDepartureEndSchedule (serviceNumber).getHour() - 
					departureMoment.getHour();
				}
				}
			    
			    
				bool needFineStepping (
				_destinationVam.needFineSteppingForArrival (edge->getParentPath ()));
			    
			    
				PtrEdgeStep step = needFineStepping 
				? (&Edge::getFollowingArrivalForFineSteppingOnly)
				: (&Edge::getFollowingConnectionArrival);
				
				for (const Edge* curEdge = (edge->*step) ();
				 curEdge != 0; curEdge = (edge->*step) ())
				{
				
				if (evaluateArrival (curEdge, departureMoment, edge, service, 
							 journeyPart, *currentJourney, strictTime,
							 continuousServiceRange) == false) 
				{
					break;
				}
				}

			} // next edge
			
			} // next vertex in vam

			std::deque<JourneyLeg*> legs;
			while (journeyPart.empty () == false)
			{
			JourneyLeg* journeyLeg = journeyPart.front ();
			journeyPart.pop_front ();
			
			if (_destinationVam.contains (journeyLeg->getDestination ()->getFromVertex ()) ||
				isVertexUseful (journeyLeg->getDestination ()->getFromVertex (),
						journeyLeg->getArrivalTime (), 
						accessDirection,
						journeyLeg->getSquareDistance ()) )
			{
				legs.push_back (journeyLeg);
			}
			else
			{ 
				delete journeyLeg;
			}
			}
		    
			std::sort (legs.begin (), legs.end (), _journeyLegComparatorForBestArrival);
		    
		    
			Journeys result;

			// Now iterate on each journey leg and call recursively the integral search
			for (std::deque<JourneyLeg*>::const_iterator itLeg = legs.begin ();
			 itLeg != legs.end (); ++itLeg)
			{
			const Vertex* nextVertex = (*itLeg)->getDestination ()->getFromVertex ();
			VertexAccessMap nextVam;
			nextVertex->getPlace ()->getImmediateVertices (nextVam,
									   accessDirection,
									   _accessParameters,
									   nextVertex,
									   useRoads,
									   useLines);
			Journey nextCurrentJourney (*currentJourney);
			nextCurrentJourney.append (*itLeg);
			
			if ( (searchAddresses && (nextVertex->isAddress ())) ||
				 (searchPhysicalStops && (nextVertex->isPhysicalStop ())) )
			{
				result.push_back (nextCurrentJourney);
			}
			
			
			if (maxDepth > 0)
			{
			    
				Journeys nextParts = integralSearch (nextVam,
									  (*itLeg)->getArrivalTime (),
									  accessDirection,
									  &nextCurrentJourney,
									  --maxDepth,
									  searchAddresses,
									  searchPhysicalStops,
									  USE_ROADS,
									  DO_NOT_USE_LINES,
									  false);
			    
				// Now, prepend each resulting journey with nextCurrentJourney.
				for (Journeys::iterator itj = nextParts.begin (); 
				 itj != nextParts.end (); ++itj)
				{
				Journey newJourney (nextCurrentJourney);
				newJourney.append (*itj);
				result.push_back (newJourney);
				}
			    
			}
			}
			return result;
		}
				     
			
				

			    




		void
		RoutePlanner::findBestJourney (Journey& result,
						   const synthese::env::VertexAccessMap& vam, 
						   const AccessDirection& accessDirection,
						   const Journey& currentJourney,
						   bool strictTime, 
						   bool optim)
		{
			Journey candidate;
		    
			if (currentJourney.getJourneyLegCount () > 
			_accessParameters.maxTransportConnectionCount) return;

			Journeys journeyParts = integralSearch (vam, 
								 _minDepartureTime,
								 accessDirection, 
								 &currentJourney,
								 0, 
								 DO_NOT_SEARCH_ADDRESSES,
								 SEARCH_PHYSICALSTOPS,
								 DO_NOT_USE_ROADS,
								 USE_LINES,
								 strictTime);

			for (Journeys::const_iterator itj = journeyParts.begin ();
			 itj != journeyParts.end (); ++itj)
			{
			
			Journey tempJourney (currentJourney);
			tempJourney.append (*itj);
			
			const Vertex* nextVertex = (accessDirection == TO_DESTINATION) 
				? tempJourney.getDestination ()->getFromVertex ()
				: tempJourney.getOrigin ()->getFromVertex ();

			VertexAccessMap nextVam;
			nextVertex->getPlace ()->getImmediateVertices (nextVam,
									   accessDirection,
									   _accessParameters,
									   nextVertex,
									   false,
									   true);

			findBestJourney (candidate, nextVam, accessDirection, tempJourney, false, optim);

			if (candidate.getJourneyLegCount ())
			{
				candidate.prepend (*itj);
			}
			
			}


			// If one candidate was created : election
			if (candidate.getJourneyLegCount ())
			{
			DateTime candidateTime = (accessDirection == TO_DESTINATION) 
				? candidate.getArrivalTime ()
				: candidate.getDepartureTime ();

			DateTime resultTime = (accessDirection == TO_DESTINATION) 
				? result.getArrivalTime ()
				: result.getDepartureTime ();
			
			bool betterTime = (accessDirection == TO_DESTINATION) 
				? candidateTime < resultTime
				: candidateTime > resultTime; 

			if ( (result.getJourneyLegCount () == 0) ||
				 (betterTime) ||
				 ( (candidateTime  == resultTime) && (candidate > result) ) )
			{
				// Note : clearing journey desallocates associated journey legs
				// (see operator =).
				result = candidate;
			}
			}
		    
		    
		}





		void
		RoutePlanner::computeRoutePlanningDepartureArrival (Journey& result,
									const VertexAccessMap& ovam,
									const VertexAccessMap& dvam)
		{
			_bestArrivalVertexReachesMap.clear ();

		    
			for (std::map<const Vertex*, VertexAccess>::const_iterator itVertex = dvam.getMap ().begin ();
			 itVertex != dvam.getMap ().end (); ++itVertex)
			{
			_bestArrivalVertexReachesMap.insert (itVertex->first, 
								 _maxArrivalTime - itVertex->second.approachTime);
			}
			for (std::map<const Vertex*, VertexAccess>::const_iterator itVertex = ovam.getMap ().begin ();
			 itVertex != ovam.getMap ().end (); ++itVertex)
			{
			_bestArrivalVertexReachesMap.insert (itVertex->first, 
								 _minDepartureTime + itVertex->second.approachTime);
			}
		    

			Journey currentJourney;

			// Look for best arrival
			findBestJourney (result, ovam, TO_DESTINATION, currentJourney, false, false);
		    
			if (result.getJourneyLegCount () == 0) return;
		    
			// If a journey was found, try to optimize by delaying departure hour as much as possible.
			_bestDepartureVertexReachesMap.clear ();
		    
			for (int i=0; i<currentJourney.getJourneyLegCount (); ++i)
			{
			_bestDepartureVertexReachesMap.insert (
				currentJourney.getJourneyLeg (i)->getOrigin ()->getFromVertex (),
				currentJourney.getJourneyLeg (i)->getDepartureTime () );
			}
		    
			for (std::map<const Vertex*, VertexAccess>::const_iterator itVertex = dvam.getMap ().begin ();
			 itVertex != dvam.getMap ().end (); ++itVertex)
			{
			_bestDepartureVertexReachesMap.insert (
				itVertex->first, 
				result.getArrivalTime () + 
				dvam.getVertexAccess (result.getDestination ()->getFromVertex ()).approachTime - 
				itVertex->second.approachTime);
			}
			for (std::map<const Vertex*, VertexAccess>::const_iterator itVertex = ovam.getMap ().begin ();
			 itVertex != ovam.getMap ().end (); ++itVertex)
			{
			_bestDepartureVertexReachesMap.insert (
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
		    
			// Look for best departure
			findBestJourney (result, dvam, FROM_ORIGIN, currentJourney, true, true);

		}






		Journeys
		RoutePlanner::computeJourneySheetDepartureArrival ()
		{
			Journey journey;

			// Create origin vam from integral search on roads
			Journeys originJourneys = integralSearch (_originVam,
								   _journeySheetStartTime,
								   TO_DESTINATION,
								   0,
								   std::numeric_limits<int>::max (),
								   DO_NOT_SEARCH_ADDRESSES,
								   SEARCH_PHYSICALSTOPS,
								   USE_ROADS,
								   DO_NOT_USE_LINES);

			VertexAccessMap ovam;
			// Include physical stops from originVam into result of integral search
			// (cos not taken into account in returned journey vector).
			ovam.merge (_originVam, 
				VertexAccessMap::DO_NOT_MERGE_ADDRESSES,
				VertexAccessMap::MERGE_PHYSICALSTOPS);



			for (Journeys::const_iterator itoj = originJourneys.begin ();
			 itoj != originJourneys.end (); ++itoj)
			{
			const Journey& oj = (*itoj);
			VertexAccess va;
			va.approachTime = _originVam.getVertexAccess (
				oj.getOrigin ()->getFromVertex ()).approachTime + oj.getDuration ();

			va.approachDistance = _originVam.getVertexAccess (
				oj.getOrigin ()->getFromVertex ()).approachDistance + oj.getDistance ();
			
			ovam.insert (oj.getDestination ()->getFromVertex (), va);
			}


			// Create destination vam from integral search on roads
			Journeys destinationJourneys = integralSearch (_destinationVam,
									_journeySheetEndTime,
									FROM_ORIGIN,
									0,
									std::numeric_limits<int>::max (),
									DO_NOT_SEARCH_ADDRESSES,
									SEARCH_PHYSICALSTOPS,
									USE_ROADS,
									DO_NOT_USE_LINES);

			VertexAccessMap dvam;
			// Include physical stops from destinationVam into result of integral search
			// (cos not taken into account in returned journey vector).
			dvam.merge (_destinationVam, 
				VertexAccessMap::DO_NOT_MERGE_ADDRESSES,
				VertexAccessMap::MERGE_PHYSICALSTOPS);


			for (Journeys::const_iterator itdj = destinationJourneys.begin ();
			 itdj != destinationJourneys.end (); ++itdj)
			{
			const Journey& dj = (*itdj);
			VertexAccess va;
			va.approachTime = _destinationVam.getVertexAccess (
				dj.getDestination ()->getFromVertex ()).approachTime + dj.getDuration ();
			va.approachDistance = _destinationVam.getVertexAccess (
				dj.getDestination ()->getFromVertex ()).approachDistance + dj.getDistance ();
			
			dvam.insert (dj.getDestination ()->getFromVertex (), va);
			}


			_previousContinuousServiceDuration = 0;

			Journeys result;
		    
			for (_minDepartureTime = _journeySheetStartTime; 
			 _minDepartureTime < _journeySheetEndTime; )
			{

			computeRoutePlanningDepartureArrival (journey, ovam, dvam);
			    
			//! <li> If no journey was found and last service is continuous, 
				//! then repeat computation after continuous service range. </li>
			if ( (journey.getJourneyLegCount () == 0) &&
				 (result.empty () == false) && 
				 (result.back ().getContinuousServiceRange () > 0) )
			{
				_minDepartureTime = _previousContinuousServiceLastDeparture;
				_minDepartureTime += 1;
				_previousContinuousServiceDuration = 0;
				computeRoutePlanningDepartureArrival (journey, ovam, dvam);	
			}
			
			if (journey.getJourneyLegCount () == 0) break;
			
			
			//! <li>If last continuous service was broken, update its range</li>
			if ( (result.empty () == false) &&
				 (result.back ().getContinuousServiceRange () > 0) &&
				 (journey.getDepartureTime () <= _previousContinuousServiceLastDeparture) )
			{
				int duration = journey.getArrivalTime () - result.back ().getArrivalTime () - 1;
				result.back ().setContinuousServiceRange (duration);
			}
			else
			{
				_previousContinuousServiceDuration = 0;
			}
			
			result.push_back (journey);
			
			_minDepartureTime = journey.getDepartureTime ();
			_minDepartureTime += 1;
			}
			
			return result;
			
		}
	}
}
