#include "LineLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
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
const std::string LineLS::LINE_AXISID_ATTR ("axisId");
const std::string LineLS::LINE_FIRSTYEAR_ATTR ("firstYear");
const std::string LineLS::LINE_LASTYEAR_ATTR ("lastYear");



synthese::env::Line* 
LineLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (LINE_TAG == node.getName ());

    std::string id (node.getAttribute (LINE_ID_ATTR.c_str()));
    std::string axisId (node.getAttribute (LINE_AXISID_ATTR.c_str()));
    
    int firstYear (su::Conversion::ToInt (
		       node.getAttribute (LINE_FIRSTYEAR_ATTR.c_str())));
    int lastYear (su::Conversion::ToInt (
		       node.getAttribute (LINE_LASTYEAR_ATTR.c_str())));

    synthese::env::Line* line = 
	new synthese::env::Line (id,	
				 environment.getAxes ().get (axisId),
				 firstYear,
				 lastYear);
    
    // Load line stops
    int nbLineStops = node.nChildNode(LineStopLS::LINESTOP_TAG.c_str());
    for (int i=0; i<nbLineStops; ++i) 
    {
	XMLNode lineStopNode = node.getChildNode (LineStopLS::LINESTOP_TAG.c_str(), i);
	synthese::env::LineStop* lineStop = LineStopLS::Load (lineStopNode, line, environment);
	environment.getLineStops ().add (lineStop);
	line->addLineStop (lineStop);
    }
    
    return line;
}




XMLNode* 
LineLS::Save (const synthese::env::Line* line)
{
    // ...
}






}
}

