
#include "Address.h"

#include "ConnectionPlace.h"
#include "Edge.h"

namespace synthese
{
namespace env
{



Address::Address (const uid& id,
		  const ConnectionPlace* connectionPlace,
		  int rankInConnectionPlace,
		  const Road* road, 
		  double metricOffset,
		  double x,
		  double y)
    : synthese::util::Registrable<uid,Address> (id)
    , Vertex (connectionPlace, rankInConnectionPlace, x, y)
    , _road (road)
    , _metricOffset (metricOffset)
{

}


Address::Address (const uid& id,
		  const Road* road, 
		  double metricOffset,
		  double x,
		  double y)
    : synthese::util::Registrable<uid,Address> (id)
    , Vertex (0, -1, x, y)
    , _road (road)
    , _metricOffset (metricOffset)
{

}



Address::~Address()
{
}




const Road* 
Address::getRoad() const
{
    return _road;
}




double 
Address::getMetricOffset () const
{
    return _metricOffset;
}





void 
Address::reachPhysicalStopAccesses (const AccessDirection& accessDirection,
				    const AccessParameters& accessParameters,
				    PhysicalStopAccessMap& result,
				    const PhysicalStopAccess& currentAccess) const
{
    const ConnectionPlace* connection = getConnectionPlace ();
    if (connection != 0)
    {
	connection->reachPhysicalStopAccesses (accessDirection, accessParameters, result, currentAccess);
    }

    for (std::set<const Edge*>::const_iterator itEdge = getDepartureEdges ().begin ();
	 itEdge != getDepartureEdges ().end () ; ++itEdge)
    {
	const Edge* edge = (*itEdge);

	double edgeDistance = edge->getLength ();
	double edgeTime = edgeDistance / accessParameters.approachSpeed;

	if (currentAccess.approachDistance + edgeDistance > accessParameters.maxApproachDistance) continue;
	if (currentAccess.approachTime + edgeTime > accessParameters.maxApproachTime) continue;

	PhysicalStopAccess currentAccessCopy = currentAccess;
	currentAccessCopy.approachDistance += edgeDistance;
	currentAccessCopy.approachTime += edgeTime;
	currentAccessCopy.path.push_back (this);
	
	edge->getNextInPath ()->getFromVertex ()->reachPhysicalStopAccesses (accessDirection,
									     accessParameters, 
									     result,
									     currentAccessCopy);
    }	
    
}



}
}

