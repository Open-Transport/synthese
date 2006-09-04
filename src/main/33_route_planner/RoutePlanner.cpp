#include "RoutePlanner.h"

#include "Journey.h"
#include "JourneyLeg.h"


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



using synthese::time::DateTime;

using synthese::env::Axis;
using synthese::env::ConnectionPlace;
using synthese::env::Edge;
using synthese::env::Line;
using synthese::env::Path;
using synthese::env::Place;
using synthese::env::Service;
using synthese::env::SquareDistance;
using synthese::env::Vertex;
using synthese::env::ReservationRule;
using synthese::env::Fare;

using synthese::env::AccessParameters;
using synthese::env::AccessDirection;
using synthese::env::VertexAccessMap;
using synthese::env::VertexAccess;

using synthese::env::FROM_ORIGIN;
using synthese::env::TO_DESTINATION;
using synthese::env::UNKNOWN_VALUE;



namespace synthese
{

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
					  const Journey* journey) const
{
    // Check if axis is allowed.
    if (path->getAxis () && (path->getAxis ()->isAllowed () == false)) 
    {
	return false;
    }

    // Check axis against already followed axes
    if ( path->getAxis () && 
	 (path->getAxis ()->isFree () == false) &&
	 (journey->getJourneyLegCount () > 0) )
    {
	for (int i=0; i<journey->getJourneyLegCount (); ++i)
	{
	    if (journey->getJourneyLeg (i)->getAxis () == path->getAxis ()) 
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

    if (_accessParameters.bikeCompliance &&
	path->getBikeCompliance ()->isCompliant () == false)
    {
	return false;
    }

    if (_accessParameters.handicappedCompliance &&
	path->getHandicappedCompliance ()->isCompliant () == false)
    {
	return false;
    }

    if (_accessParameters.handicappedCompliance &&
	path->getHandicappedCompliance ()->isCompliant () == false)
    {
	return false;
    }

    if (_accessParameters.pedestrianCompliance &&
	path->getPedestrianCompliance ()->isCompliant () == false)
    {
	return false;
    }

    if (_accessParameters.withReservation &&
	path->getReservationRule ()->getType () != ReservationRule::RESERVATION_TYPE_COMPULSORY)
    {
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
RoutePlanner::isDestinationUsefulForSoonArrival (const Vertex* vertex,
						 const DateTime& dateTime,
						 SquareDistance& sqd) const
{
    if (sqd.getSquareDistance () == UNKNOWN_VALUE)
    {
	sqd.setFromPoints (*vertex, _destinationVam.getIsobarycenter ());  
	sqd.setSquareDistance (sqd.getSquareDistance () - 
			       _destinationVam.getIsobarycenterMaxSquareDistance ().
			       getSquareDistance ());
	
    }
    
    // Check that the maximal arrival time is not exceeded
    DateTime arrivalMoment (dateTime);
    if ((_destinationVam.contains (vertex) && (vertex->getConnectionPlace ())))
    {
	arrivalMoment += vertex->getConnectionPlace ()->getMinTransferDelay ();
    }
	
    if (arrivalMoment > _maxArrivalTime) return false;

    // TODO : re-implement VMax control.

    return true;
    
}







bool 
RoutePlanner::evaluateArrival (const Edge* arrivalEdge,
			       const DateTime& departureMoment,
			       const Edge* departureEdge,
			       const Service* service,
			       std::deque<JourneyLeg*>& journeyPart,
			       const Journey* currentJourney,
			       bool strictTime,
			       int continuousServiceRange)
{
    if (arrivalEdge == 0) return true;
    const Vertex* departureVertex = departureEdge->getFromVertex ();
    const Vertex* arrivalVertex = arrivalEdge->getFromVertex ();
    
    // Arrival moment
    DateTime arrivalMoment = departureMoment;
    arrivalEdge->calculateArrival (*departureEdge, 
				   service->getServiceNumber (), 
				   departureMoment, 
				   arrivalMoment);

    SquareDistance sqd;
    if (isDestinationUsefulForSoonArrival (arrivalVertex,
					   arrivalMoment, sqd) == false)
    {
	return false;
    }


    // Continuous service breaking
    if (_previousContinuousServiceDuration)
    {
	if ( (currentJourney->getJourneyLegCount () > 0) &&
	     (currentJourney->getDepartureTime () <= _previousContinuousServiceLastDeparture) &&
	     (arrivalMoment - currentJourney->getDepartureTime () >= _previousContinuousServiceDuration) )
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
	    journeyLeg->setOrigin (departureVertex);
	    journeyLeg->setDestination (arrivalVertex);
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
	    journeyLeg->setOrigin (departureVertex);
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
				    
				    




JourneyVector 
RoutePlanner::integralSearch (const VertexAccessMap& vam, 
			      const DateTime& desiredTime,
			      const AccessDirection& accessDirection,
			      const Journey* currentJourney,
			      int maxDepth,
			      bool searchAddresses, 
			      bool searchPhysicalStops,
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
	    if (areAxisContraintsFulfilled (edge->getParentPath (), currentJourney) == false) continue;

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
	    
	    
	    const Line* line = dynamic_cast<const Line*> (edge->getParentPath ());
	    if (line != 0) 
	    {
		
		bool needFineStepping (
		    _destinationVam.needFineSteppingForArrival (line));
		
		
		PtrEdgeStep step = needFineStepping 
		    ? (&Edge::getFollowingArrival)
		    : (&Edge::getFollowingArrivalForFineSteppingOnly);
		
		for (const Edge* curEdge = (edge->*step) ();
		     curEdge != 0; curEdge = (edge->*step) ())
		{
		    
		    if (evaluateArrival (curEdge, departureMoment, edge, service, 
					 journeyPart, currentJourney, strictTime,
					 continuousServiceRange) == false) 
		    {
			break;
		    }
		}
	    }
	    else
	    {
		// TODO : if path is a road...
		
	    }
	} // next edge

    } // next vertex in vam

    std::deque<JourneyLeg*> result;
    while (journeyPart.empty () == false)
    {
	JourneyLeg* journeyLeg = journeyPart.front ();
	journeyPart.pop_front ();
	
	if (_destinationVam.contains (journeyLeg->getDestination ()) ||
	    isDestinationUsefulForSoonArrival (journeyLeg->getDestination (),
					       journeyLeg->getArrivalTime (), 
					       journeyLeg->getSquareDistance ()) )
	{
	    result.push_back (journeyLeg);
	}
	else
	{ 
	    delete journeyLeg;
	}
    }
    
    std::sort (result.begin (), result.end (), _journeyLegComparatorForBestArrival);


    // TODO : reflechir!



}
	
		



	    
	    
/*	    
	   if (currentAccess.approachDistance + edgeDistance > accessParameters.maxApproachDistance) continue;
	    if (currentAccess.approachTime + edgeTime > accessParameters.maxApproachTime) continue;
	    
	    PhysicalStopAccess currentAccessCopy = currentAccess;
	    currentAccessCopy.approachDistance += edgeDistance;
	    currentAccessCopy.approachTime += edgeTime;
	    currentAccessCopy.path.push_back (this);
	    
	}
    }	

    return result;
    
}

*/
	    





}
}

