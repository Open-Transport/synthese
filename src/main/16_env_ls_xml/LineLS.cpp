#include "LineLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"
#include "15_env/Line.h"


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string LineLS::LINE_TAG ("line");
const std::string LineLS::LINE_ID_ATTR ("id");
const std::string LineLS::LINE_AXISID_ATTR ("axisId");
const std::string LineLS::LINE_FIRSTYEAR_ATTR ("firstYear");
const std::string LineLS::LINE_LASTYEAR_ATTR ("lastYear");


synthese::env::Line* 
LineLS::Load (XMLNode& node,
	      const synthese::env::Environment& environment)
{
    assert (LINE_TAG == node.getName ());

    std::string id (node.getAttribute (LINE_ID_ATTR.c_str()));
    std::string axisId (node.getAttribute (LINE_AXISID_ATTR.c_str()));
    
    int firstYear (su::Conversion::ToInt (
		       node.getAttribute (LINE_FIRSTYEAR_ATTR.c_str())));
    int lastYear (su::Conversion::ToInt (
		       node.getAttribute (LINE_LASTYEAR_ATTR.c_str())));
    
    return new synthese::env::Line (id,	
				    environment.getAxes ().get (axisId),
				    firstYear,
				    lastYear);
}




XMLNode* 
LineLS::Save (const synthese::env::Line* line)
{
    // ...
}






}
}

