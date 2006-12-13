#include "PlaceAlias.h"


namespace synthese
{
namespace env
{


PlaceAlias::PlaceAlias (const uid& id,
			const std::string& name,
			const Place* aliasedPlace,
			const City* city)
: synthese::util::Registrable<uid,PlaceAlias> (id)
, IncludingPlace (name, city)
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



