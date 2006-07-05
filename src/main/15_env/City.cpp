#include "City.h"
#include "LogicalStop.h"


namespace synthese
{
namespace env
{

City::City (const uid& key,
	    const std::string& name)
    : synthese::util::Registrable<uid,City> (key)
    , IncludingPlace (name, 0)  // Note this city's city is null ?
{
}



City::~City ()
{

}



void 
City::addMainLogicalStop (const LogicalStop* logicalStop)
{
    _includedPlaces.push_back (logicalStop);
}







}
}


