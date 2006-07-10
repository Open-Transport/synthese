#include "City.h"



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










}
}


