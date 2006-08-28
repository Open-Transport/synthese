#include "RoutePlanner.h"


using synthese::time::DateTime;
using synthese::env::Place;
using synthese::env::AccessParameters;
using synthese::env::AccessDirection;
using synthese::env::VertexAccessMap;




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
{



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








JourneyVector 
RoutePlanner::integralSearch (const VertexAccessMap& vertices, 
			      const DateTime& desiredTime,
			      const AccessDirection& accessDirection,
			      Journey* currentJourney,
			      int maxDepth,
			      bool searchAddresses, 
			      bool searchPhysicalStops,
			      bool strictTime) const
{
    JourneyVector result;

/*
    for (VertexAccessMap::const_iterator it = vertices.begin ();
	 it != vertices.end (); ++it)
    {
	const Vertex* origin = it->first;
	
	for (std::set<const Edge*>::const_iterator itEdge = origin->getDepartureEdges ().begin ();
	     itEdge != origin->getDepartureEdges ().end () ; ++itEdge)
	{
	    const Edge* edge = (*itEdge);
	    
	    double edgeDistance = edge->getLength ();
	    double edgeTime = edgeDistance / _accessParameters.approachSpeed;
	    
	    double totalDistance = it->second.approachDistance + edgeTime;
	    double totalTime = it->second.approachTime + edgeTime;
	    
	    
	    
	    if (currentAccess.approachDistance + edgeDistance > accessParameters.maxApproachDistance) continue;
	    if (currentAccess.approachTime + edgeTime > accessParameters.maxApproachTime) continue;
	    
	    PhysicalStopAccess currentAccessCopy = currentAccess;
	    currentAccessCopy.approachDistance += edgeDistance;
	    currentAccessCopy.approachTime += edgeTime;
	    currentAccessCopy.path.push_back (this);
	    
	}
    }	
*/
    return result;
    
}






}
}

