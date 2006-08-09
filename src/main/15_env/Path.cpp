#include "Path.h"

#include "Alarm.h"
#include "ConnectionPlace.h"
#include "Edge.h"
#include "Vertex.h"
#include "Service.h"


#include <assert.h>


namespace synthese
{
namespace env
{



Path::Path ()
    : BikeComplyer (0) // No parent complyer right now
    , HandicappedComplyer (0) // No parent complyer right now
    , PedestrianComplyer (0) // No parent complyer right now
    , ReservationRuleComplyer (0) // No parent complyer right now
    , _calendar ()
{
    
}
    


Path::~Path ()
{
}



bool 
Path::hasApplicableAlarm (const synthese::time::DateTime& start, 
			  const synthese::time::DateTime& end) const
{
    if (_alarm == 0) return false;
    return _alarm->isApplicable (start, end);
}



const Alarm* 
Path::getAlarm() const
{
    return _alarm;
}



void
Path::setAlarm (Alarm* alarm)
{
    _alarm = alarm;
}



const Fare* 
Path::getFare () const
{
    return _fare;
}



void 
Path::setFare (Fare* fare)
{
    _fare = fare;
}





const std::vector<Service*>& 
Path::getServices () const
{
    return _services;
}




const Service* 
Path::getService (int serviceNumber) const
{
    return _services.at (serviceNumber);
}




void 
Path::addService (Service* service)
{
    _services.push_back (service);
}




bool 
Path::isInService (const synthese::time::Date& date) const
{
    return _calendar.isMarked ( date );

}




void 
Path::updateCalendar ()
{
    /* MJ TODO REVIEW this

    _calendar.reset ();
    LineStop* lastLineStop = _lineStops.back();
    for (int s=0; s<_services.size (); ++s)
    {
        if ( lastLineStop->getLastArrivalSchedule (s).getDaysSinceDeparture() != 
	     _lineStops.front()->getFirstDepartureSchedule(s).getDaysSinceDeparture() )
        {
            _calendar.reset (true);
            break;
        }
	// MJ constness problem !
        ((Service*) getService (s))->getCalendar ()->setInclusionToMask (_calendar);
    }
    */

}


const Edge* 
Path::getEdge (int index) const
{
    return _edges[index];
}




const std::vector<Edge*>& 
Path::getEdges() const
{
    return _edges;
}




std::vector<const Point*> 
Path::getPoints (int fromEdgeIndex,
		 int toEdgeIndex) const
{
    if (toEdgeIndex == -1) toEdgeIndex = _edges.size () - 1;
    std::vector<const Point*> points;
    
    for (int i=fromEdgeIndex; i<=toEdgeIndex; ++i)
    {
	points.push_back (_edges[i]->getFromVertex ());
	
	// Adds all the via points of the line stop
	const std::vector<const Point*>& viaPoints = _edges[i]->getViaPoints ();
	for (std::vector<const Point*>::const_iterator it = viaPoints.begin (); 
	     it != viaPoints.end (); 
	     ++it)
	{
	    points.push_back (*it);
	}
    }
    return points;
}



void 
Path::addEdge (Edge* edge)
{
    if (_edges.empty () == false)
    {
	_edges.back ()->setNextInPath (edge);
    }

    _edges.push_back( edge );

    if ( _edges.size() > 1 )
    {
        // Chaining departure/arrival
        for ( std::vector<Edge*>::reverse_iterator riter = _edges.rbegin();
	      ( riter != _edges.rend() )
		  && (
		      ( *riter )->getFollowingArrival () == 0
		      || ( *riter )->getFollowingConnectionArrival() == 0
		      || edge->getPreviousDeparture () == 0
		      || edge->getPreviousConnectionDeparture() == 0
		      );
	      ++riter )
        {
            Edge* currentEdge = *riter;
         
	    // Chain only relations between A and A, D and D, A and D 
	    // if different stops, D and A if different stops
            if ( currentEdge->getFromVertex ()->getConnectionPlace() != 
		 edge->getFromVertex ()->getConnectionPlace() || 
		 currentEdge->getType () == edge->getType () )
            {
                // Chain following arrivals
                if ( currentEdge->getFollowingArrival () == 0 && 
		     edge->isArrival () )
		{
                    currentEdge->setFollowingArrival ( edge );
		}
		if ( currentEdge->getFollowingConnectionArrival () == 0 && 
		     edge->getFromVertex ()->getConnectionPlace()
		     ->isConnectionAuthorized () )
		{
                    currentEdge->setFollowingConnectionArrival ( edge );
		} 
                if ( currentEdge->isDeparture () && 
		     edge->getPreviousDeparture () == 0 )
		{   
		    edge->setPreviousDeparture ( currentEdge );
		}

		if ( currentEdge->isDeparture () && 
		     edge->getPreviousConnectionDeparture () == 0 && 
		     currentEdge->getFromVertex ()
		     ->getConnectionPlace()->isConnectionAuthorized() )
		{
                    edge->setPreviousConnectionDeparture ( currentEdge );
		}
            }
        }
    }

    // TODO : optimize this.
    for ( std::vector<Edge*>::const_iterator iter = _edges.begin();
	  iter != _edges.end();
	  ++iter )
    {
        Edge* edge = *iter;

        if ( edge->getFollowingArrival () == 0 )
            edge->setType ( Edge::EDGE_TYPE_ARRIVAL );
        if ( edge->getPreviousDeparture () == 0 )
            edge->setType ( Edge::EDGE_TYPE_DEPARTURE );
    }
    
}



int 
Path::getEdgesCount () const
{
    return _edges.size ();
}






}
}
