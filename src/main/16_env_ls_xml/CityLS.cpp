#include "CityLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"
#include "15_env/City.h"


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string CityLS::CITY_TAG ("city");
const std::string CityLS::CITY_ID_ATTR ("id");
const std::string CityLS::CITY_NAME_ATTR ("name");


synthese::env::City* 
CityLS::Load (XMLNode& node,
	      const synthese::env::Environment& environment)
{
    int id (su::Conversion::ToInt (
		node.getAttribute (CITY_ID_ATTR.c_str())));
    std::string name (node.getAttribute (CITY_NAME_ATTR.c_str()));

    return new synthese::env::City (id,	name);
}




XMLNode* 
CityLS::Save (const synthese::env::City* city)
{
    // ...
}






}
}

