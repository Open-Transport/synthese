#include "LineLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/RGBColor.h"
#include "01_util/UId.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"
#include "15_env/Line.h"

#include "LineStopLS.h"


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string LineLS::LINE_TAG ("line");
const std::string LineLS::LINE_ID_ATTR ("id");
const std::string LineLS::LINE_NAME_ATTR ("name");
const std::string LineLS::LINE_AXISID_ATTR ("axisId");
const std::string LineLS::LINE_COLOR_ATTR ("color");



synthese::env::Line* 
LineLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (LINE_TAG == node.getName ());

    uid id (su::Conversion::ToLongLong (node.getAttribute (LINE_ID_ATTR.c_str())));
    std::string name (node.getAttribute (LINE_NAME_ATTR.c_str()));

    uid axisId (su::Conversion::ToLongLong (
		    node.getAttribute (LINE_AXISID_ATTR.c_str())));
    
    synthese::env::Line* line = 
	new synthese::env::Line (id,	
				 name,
				 environment.getAxes ().get (axisId));    

    // Optional attributes
    if (node.getAttribute (LINE_COLOR_ATTR.c_str()) != 0)
    {
	line->setColor (synthese::util::RGBColor (node.getAttribute (LINE_COLOR_ATTR.c_str())));
    }
    

    return line;
}




XMLNode* 
LineLS::Save (const synthese::env::Line* line)
{
    // ...
    return 0;

}






}
}

