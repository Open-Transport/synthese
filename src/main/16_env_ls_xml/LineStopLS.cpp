#include "LineStopLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/LineStop.h"

#include "PointLS.h"
#include "Exception.h"

#include <boost/algorithm/string.hpp>


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string LineStopLS::LINESTOP_TAG ("lineStop");
const std::string LineStopLS::LINESTOP_ID_ATTR ("id");
const std::string LineStopLS::LINESTOP_METRICOFFSET_ATTR ("metricOffset");

const std::string LineStopLS::LINESTOP_TYPE_ATTR ("type");
const std::string LineStopLS::LINESTOP_TYPE_ATTR_DEPARTURE ("departure");
const std::string LineStopLS::LINESTOP_TYPE_ATTR_ARRIVAL ("arrival");
const std::string LineStopLS::LINESTOP_TYPE_ATTR_PASSAGE ("passage");

const std::string LineStopLS::LINESTOP_PHYSICALSTOPID_ATTR ("physicalStopId");


synthese::env::LineStop* 
LineStopLS::Load (XMLNode& node,
		  const synthese::env::Line* line, int rankInLine,
		  const synthese::env::Environment& environment)
{
    // assert (LINESTOP_TAG == node.getName ());
    uid id (su::Conversion::ToLongLong (
		       node.getAttribute (LINESTOP_ID_ATTR.c_str())));

    double metricOffset (su::Conversion::ToDouble (
	node.getAttribute (LINESTOP_METRICOFFSET_ATTR.c_str())));

    int physicalStopId (su::Conversion::ToInt (
	node.getAttribute (LINESTOP_PHYSICALSTOPID_ATTR.c_str())));

    synthese::env::LineStop* lineStop = new synthese::env::LineStop (
	id,
	line, 
	rankInLine, 
	metricOffset, 
	environment.getPhysicalStops (). get (physicalStopId));

    

    // Add via points
    int nbPoints = node.nChildNode(PointLS::POINT_TAG.c_str());
    for (int i=0; i<nbPoints; ++i) 
    {
	XMLNode pointNode = node.getChildNode(PointLS::POINT_TAG.c_str(), i);
	lineStop->addViaPoint (PointLS::Load (pointNode));
    }

    return lineStop;
    
}




XMLNode* 
LineStopLS::Save (const synthese::env::LineStop* lineStop)
{
    // ...
    return 0;

}






}
}

