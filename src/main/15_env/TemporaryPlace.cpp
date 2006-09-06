#include "TemporaryPlace.h"

#include "Address.h"
#include "Road.h"


namespace synthese
{
namespace env
{


TemporaryPlace::TemporaryPlace (const Road* road, double metricOffset)
    : Place ("", road->getCity ())
    , _road (road)
    , _metricOffset (metricOffset)
{
}



TemporaryPlace::~TemporaryPlace ()
{
}


const Road* 
TemporaryPlace::getRoad () const
{
    return _road;
}




double 
TemporaryPlace::getMetricOffset () const
{
    return _metricOffset;
}







VertexAccess 
TemporaryPlace::getVertexAccess (const AccessDirection& accessDirection,
				 const AccessParameters& accessParameters,
				 const Vertex* destination,
				 const Vertex* origin) const
{
    VertexAccess access;
    access.approachDistance = _metricOffset - ((Address*) destination)->getMetricOffset ();
    access.approachTime = access.approachDistance / accessParameters.approachSpeed;
}
    







void
TemporaryPlace::getImmediateVertices (VertexAccessMap& result, 
				      const AccessDirection& accessDirection,
				      const AccessParameters& accessParameters,
				      const Vertex* origin,
				      bool returnAddresses,
				      bool returnPhysicalStops) const
{
    // Find closest addresses on both sides and run search from here.
    const Address* closestBefore = _road->findClosestAddressBefore (_metricOffset);
    const Address* closestAfter = _road->findClosestAddressAfter (_metricOffset);

    if (closestBefore != 0)
    {
	VertexAccess access;
	access.approachDistance = _metricOffset - closestBefore->getMetricOffset ();
	access.approachTime = access.approachDistance / accessParameters.approachSpeed;
	
	result.insert (closestBefore, access);
    }

    if ( (closestAfter != 0) && (closestAfter != closestBefore) )
    {
	VertexAccess access;
	access.approachDistance = _metricOffset - closestAfter->getMetricOffset ();
	access.approachTime = access.approachDistance / accessParameters.approachSpeed;
	
	result.insert (closestAfter, access);
    }
}







}
}


