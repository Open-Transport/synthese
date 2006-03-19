#include "PlaceAlias.h"


namespace synthese
{
namespace env
{


PlaceAlias::PlaceAlias (const std::string& name,
			const Place* aliasedPlace)
    : IncludingPlace (name, aliasedPlace->getCity ())
{
    addIncludedPlace (aliasedPlace);
}



PlaceAlias::~PlaceAlias ()
{

}



const Place* 
PlaceAlias::getAliasedPlace () const
{
    return _includedPlaces[0];
}


const std::string& 
PlaceAlias::getOfficialName () const
{
    return getAliasedPlace ()->getOfficialName ();
}










}
}


