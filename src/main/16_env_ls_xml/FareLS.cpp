#include "FareLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Environment.h"
#include "15_env/Fare.h"

#include "04_time/DateTime.h"



using namespace synthese::util::XmlToolkit;
using synthese::time::DateTime;
using synthese::env::Fare;


namespace synthese
{
namespace envlsxml
{



const std::string FareLS::FARE_TAG ("fare");
const std::string FareLS::FARE_ID_ATTR ("id");
const std::string FareLS::FARE_NAME_ATTR ("name");

const std::string FareLS::FARE_TYPE_ATTR ("type");
const std::string FareLS::FARE_TYPE_ATTR_ZONING ("zoning");
const std::string FareLS::FARE_TYPE_ATTR_SECTION ("section");
const std::string FareLS::FARE_TYPE_ATTR_DISTANCE ("distance");





void 
FareLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (FARE_TAG == node.getName ());
    uid id (GetLongLongAttr (node, FARE_ID_ATTR));

    if (environment.getFares ().contains (id)) return;

    std::string name (GetStringAttr (node, FARE_NAME_ATTR));

    std::string typeStr (GetStringAttr (node, FARE_TYPE_ATTR));
    Fare::FareType type (Fare::FARE_TYPE_ZONING);
    if (typeStr == FARE_TYPE_ATTR_ZONING) type = Fare::FARE_TYPE_ZONING;
    else if (typeStr == FARE_TYPE_ATTR_SECTION) type = Fare::FARE_TYPE_SECTION;
    else if (typeStr == FARE_TYPE_ATTR_DISTANCE) type = Fare::FARE_TYPE_DISTANCE;
    
    environment.getFares ().add (new synthese::env::Fare (id, 
							  name, 
							  type));    
}




XMLNode* 
FareLS::Save (const synthese::env::Fare* fare)
{
    // ...
    return 0;
}






}
}

