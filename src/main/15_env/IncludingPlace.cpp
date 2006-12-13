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
IncludingPlace::getImmediateVertices (VertexAccessMap& result, 
				      const AccessDirection& accessDirection,
				      const AccessParameters& accessParameters,
				      const Vertex* origin,
				      bool returnAddresses,
				      bool returnPhysicalStops) const
{

    for (std::vector<const Place*>::const_iterator it = _includedPlaces.begin ();
	 it != _includedPlaces.end (); ++it)
    {
	(*it)->getImmediateVertices (result, accessDirection, accessParameters, 
				     origin, returnAddresses, returnPhysicalStops);
    }

}









}
}

