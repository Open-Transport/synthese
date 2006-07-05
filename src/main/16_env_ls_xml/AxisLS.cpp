#include "AxisLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"
#include "15_env/Axis.h"


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string AxisLS::AXIS_TAG ("axis");
const std::string AxisLS::AXIS_ID_ATTR ("id");
const std::string AxisLS::AXIS_NAME_ATTR ("name");
const std::string AxisLS::AXIS_FREE_ATTR ("free");
const std::string AxisLS::AXIS_AUTHORIZED_ATTR ("authorized");


synthese::env::Axis* 
AxisLS::Load (XMLNode& node,
	      const synthese::env::Environment& environment)
{
    // assert (AXIS_TAG == node.getName ());

    uid id (su::Conversion::ToLongLong (
		   node.getAttribute (AXIS_ID_ATTR.c_str())));

    std::string name (node.getAttribute (AXIS_NAME_ATTR.c_str()));

    bool free (su::Conversion::ToBool (
		   node.getAttribute (AXIS_FREE_ATTR.c_str())));
    bool authorized (su::Conversion::ToBool (
		   node.getAttribute (AXIS_AUTHORIZED_ATTR.c_str())));

    return new synthese::env::Axis (id,	name, free, authorized);
}




XMLNode* 
AxisLS::Save (const synthese::env::Axis* axis)
{
    // ...
    return 0;
}






}
}

