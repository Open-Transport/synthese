#include "PublicPlaceLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/PublicPlace.h"


using synthese::env::PublicPlace;
using synthese::env::City;
using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{

const std::string PublicPlaceLS::PUBLICPLACE_TAG ("publicPlace");
const std::string PublicPlaceLS::PUBLICPLACE_ID_ATTR ("id");
const std::string PublicPlaceLS::PUBLICPLACE_NAME_ATTR ("name");
const std::string PublicPlaceLS::PUBLICPLACE_CITYID_ATTR ("cityId");



void
PublicPlaceLS::Load (XMLNode& node,
			 synthese::env::Environment& environment)
{
    assert (PUBLICPLACE_TAG == node.getName ());
    
    uid id (GetLongLongAttr (node, PUBLICPLACE_ID_ATTR));

    if (environment.getPublicPlaces ().contains (id)) return;

    std::string name (GetStringAttr (node, PUBLICPLACE_NAME_ATTR));
    uid cityId (GetLongLongAttr (node, PUBLICPLACE_CITYID_ATTR));
    
    City* city = environment.getCities ().get (cityId);

    PublicPlace* pp = new PublicPlace (id, name, city);
    
    city->getPublicPlacesMatcher ().add (pp->getName (), pp);
    environment.getPublicPlaces ().add (pp);
}




XMLNode* 
PublicPlaceLS::Save (const synthese::env::PublicPlace* publicPlace)
{
    // ...
    return 0;

}






}
}


