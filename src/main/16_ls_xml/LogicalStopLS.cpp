#include "LogicalStopLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"
#include "15_env/LogicalStop.h"


namespace su = synthese::util;

namespace synthese
{
namespace lsxml
{

const std::string LogicalStopLS::LOGICALSTOP_TAG ("logicalStop");
const std::string LogicalStopLS::LOGICALSTOP_ID_ATTR ("id");
const std::string LogicalStopLS::LOGICALSTOP_NAME_ATTR ("name");
const std::string LogicalStopLS::LOGICALSTOP_CITYID_ATTR ("cityId");


synthese::env::LogicalStop* 
LogicalStopLS::Load (XMLNode& node,
	      const synthese::env::Environment& environment)
{
    assert (LOGICALSTOP_TAG == node.getName ());

    int id (su::Conversion::ToInt (
		node.getAttribute (LOGICALSTOP_ID_ATTR.c_str())));
    std::string name (node.getAttribute (LOGICALSTOP_NAME_ATTR.c_str()));

    int cityId (su::Conversion::ToInt (
		    node.getAttribute (LOGICALSTOP_CITYID_ATTR.c_str())));

    return new synthese::env::LogicalStop (id, name,
					   environment.getCities ().get (cityId));
}




XMLNode* 
LogicalStopLS::Save (const synthese::env::LogicalStop* logicalStop)
{
    // ...
}






}
}

