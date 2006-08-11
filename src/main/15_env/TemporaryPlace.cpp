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



void 
TemporaryPlace::reachPhysicalStopAccesses (const AccessDirection& accessDirection,
					   const AccessParameters& accessParameters,
					   PhysicalStopAccessMap& result,
					   const PhysicalStopAccess& currentAccess) const
{
    // Find closest addresses on both sides and run search from here.
    const Address* closestBefore = _road->findClosestAddressBefore (_metricOffset);
    const Address* closestAfter = _road->findClosestAddressAfter (_metricOffset);

    if (closestBefore != 0)
    {
	PhysicalStopAccess currentAccessCopy = currentAccess;
	double deltaDistance = _metricOffset - closestBefore->getMetricOffset ();
	double deltaTime = deltaDistance / accessParameters.approachSpeed;
	currentAccessCopy.approachDistance += deltaDistance;
	currentAccessCopy.approachTime += deltaTime;

	closestBefore->reachPhysicalStopAccesses (accessDirection,
						  accessParameters, 
						  result,
						  currentAccessCopy);
    }

    if ( (closestAfter != 0) && (closestAfter != closestBefore) )
    {
	PhysicalStopAccess currentAccessCopy = currentAccess;
	double deltaDistance =  closestAfter->getMetricOffset () - _metricOffset;
	double deltaTime = deltaDistance / accessParameters.approachSpeed;
	currentAccessCopy.approachDistance += deltaDistance;
	currentAccessCopy.approachTime += deltaTime;

	closestAfter->reachPhysicalStopAccesses (accessDirection,
						  accessParameters, 
						  result,
						  currentAccessCopy);
    }

}




}
}


