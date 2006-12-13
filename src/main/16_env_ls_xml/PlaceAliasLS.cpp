#include "PlaceAliasLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/PlaceAlias.h"


using synthese::env::PlaceAlias;
using synthese::env::City;
using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{

    const std::string PlaceAliasLS::PLACEALIAS_TAG ("placeAlias");
    const std::string PlaceAliasLS::PLACEALIAS_ID_ATTR ("id");
    const std::string PlaceAliasLS::PLACEALIAS_NAME_ATTR ("name");
    const std::string PlaceAliasLS::PLACEALIAS_ALIASEDPLACEID_ATTR ("aliasedPlaceId");
    const std::string PlaceAliasLS::PLACEALIAS_CITYID_ATTR ("cityId");



void
PlaceAliasLS::Load (XMLNode& node,
			 synthese::env::Environment& environment)
{
    assert (PLACEALIAS_TAG == node.getName ());
    
    uid id (GetLongLongAttr (node, PLACEALIAS_ID_ATTR));

    if (environment.getPlaceAliases ().contains (id)) return;

    std::string name (GetStringAttr (node, PLACEALIAS_NAME_ATTR));
    uid aliasedPlaceId (GetLongLongAttr (node, PLACEALIAS_ALIASEDPLACEID_ATTR));
    uid cityId (GetLongLongAttr (node, PLACEALIAS_CITYID_ATTR));
    
    City* city = environment.getCities ().get (cityId);

    PlaceAlias* pa = new PlaceAlias (id, name,
				     environment.fetchPlace (aliasedPlaceId),
				     city);
    
    city->getPlaceAliasesMatcher ().add (pa->getName (), pa);
    environment.getPlaceAliases ().add (pa);
}




XMLNode* 
PlaceAliasLS::Save (const synthese::env::PlaceAlias* publicPlace)
{
    // ...
    return 0;

}






}
}


