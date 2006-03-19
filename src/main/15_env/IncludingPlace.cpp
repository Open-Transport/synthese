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








}
}
