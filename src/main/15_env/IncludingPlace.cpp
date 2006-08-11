#include "IncludingPlace.h"



namespace synthese
{
namespace env
{

IncludingPlace::IncludingPlace (const std::string& name,
				const City* city)
    : Place (name, city)
{
}



IncludingPlace::~IncludingPlace ()
{
}



const std::vector<const Place*>& 
IncludingPlace::getIncludedPlaces () const
{
    return _includedPlaces;
}
    


void 
IncludingPlace::addIncludedPlace (const Place* place)
{
    _includedPlaces.push_back (place);
}




void
IncludingPlace::reachPhysicalStopAccesses (const AccessDirection& accessDirection,
					   const AccessParameters& accessParameters,
					   PhysicalStopAccessMap& result,
					   const PhysicalStopAccess& currentAccess) const
{
    for (std::vector<const Place*>::const_iterator it = _includedPlaces.begin ();
	 it != _includedPlaces.end (); ++it)
    {
	(*it)->reachPhysicalStopAccesses (accessDirection, accessParameters, result, currentAccess);
    }
}







}
}
