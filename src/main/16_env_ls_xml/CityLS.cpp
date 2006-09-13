#include "CityLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/City.h"


using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{

const std::string CityLS::CITY_TAG ("city");
const std::string CityLS::CITY_ID_ATTR ("id");
const std::string CityLS::CITY_NAME_ATTR ("name");


void
CityLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    uid id (GetLongLongAttr (node, CITY_ID_ATTR));

    if (environment.getCities ().contains (id)) return;

    std::string name (GetStringAttr (node, CITY_NAME_ATTR));

    environment.getCities ().add (new synthese::env::City (id, name));
}




XMLNode* 
CityLS::Save (const synthese::env::City* city)
{
    // ...
    return 0;
}






}
}

