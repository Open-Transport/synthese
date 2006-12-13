#include "AxisLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Environment.h"
#include "15_env/Axis.h"


using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{

const std::string AxisLS::AXIS_TAG ("axis");
const std::string AxisLS::AXIS_ID_ATTR ("id");
const std::string AxisLS::AXIS_NAME_ATTR ("name");
const std::string AxisLS::AXIS_FREE_ATTR ("free");
const std::string AxisLS::AXIS_AUTHORIZED_ATTR ("authorized");


void 
AxisLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (AXIS_TAG == node.getName ());
    uid id (GetLongLongAttr (node, AXIS_ID_ATTR));

    if (environment.getAxes ().contains (id)) return;

    std::string name (GetStringAttr (node, AXIS_NAME_ATTR));

    bool free (GetBoolAttr (node, AXIS_FREE_ATTR));
    bool authorized (GetBoolAttr (node, AXIS_AUTHORIZED_ATTR));
    
    environment.getAxes ().add (new synthese::env::Axis (id, name, free, authorized));
}




XMLNode* 
AxisLS::Save (const synthese::env::Axis* axis)
{
    // ...
    return 0;
}






}
}


