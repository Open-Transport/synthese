#include "RoutePlanner.h"

#include "15_env/Vertex.h"
#include "15_env/Edge.h"
#include "15_env/Path.h"
#include "15_env/Service.h"

using synthese::time::DateTime;

using synthese::env::Place;
using synthese::env::Vertex;
using synthese::env::Edge;
using synthese::env::Path;
using synthese::env::Service;
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
			      const DateTime& dateTime,
			      const AccessDirection& accessDirection,
			      Journey* currentJourney,
			      int maxDepth,
			      bool searchAddresses, 
			      bool searchPhysicalStops,
			      bool strictTime) const
{
    JourneyVector result;

    for (VertexAccessMap::const_iterator itVertex = vertices.begin ();
	 itVertex != vertices.end (); ++itVertex)
    {
	const Vertex* origin = itVertex->first;
	
	for (std::set<const Edge*>::const_iterator itEdge = origin->getDepartureEdges ().begin ();
	     itEdge != origin->getDepartureEdges ().end () ; ++itEdge)
	{
	    const Edge* edge = (*itEdge);
	    const Path* path = edge->getParentPath ();
	    
	    // Check for next available service for this edge at desired time
	    if (path->isInService (dateTime.getDate ()) == false) continue;
	    
	    for (std::vector<Service*>::const_iterator itService = path->getServices ().begin ();
		 itService != path->getServices ().end (); ++itService)
	    {
		
	    }
		
	    
	    double edgeDistance = edge->getLength ();
	    double edgeTime = edgeDistance / _accessParameters.approachSpeed;
	    
	    double totalDistance = itVertex->second.approachDistance + edgeTime;
	    double totalTime = itVertex->second.approachTime + edgeTime;
	    
	    
/*	    
	   if (currentAccess.approachDistance + edgeDistance > accessParameters.maxApproachDistance) continue;
	    if (currentAccess.approachTime + edgeTime > accessParameters.maxApproachTime) continue;
	    
	    PhysicalStopAccess currentAccessCopy = currentAccess;
	    currentAccessCopy.approachDistance += edgeDistance;
	    currentAccessCopy.approachTime += edgeTime;
	    currentAccessCopy.path.push_back (this);
	    
*/
	    
	}
    }	

    return result;
    
}






}
}

