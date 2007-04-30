#include "PlaceAlias.h"


namespace synthese
{
namespace env
{


PlaceAlias::PlaceAlias (uid id,
			std::string name,
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

void PlaceAlias::setAliasedPlace( const Place* place )
{
	_includedPlaces.clear();
	addIncludedPlace(place);
}









}
}



