#include "LineStopLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

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
const std::string LineStopLS::LINESTOP_LINEID_ATTR ("lineId");
const std::string LineStopLS::LINESTOP_METRICOFFSET_ATTR ("metricOffset");

const std::string LineStopLS::LINESTOP_TYPE_ATTR ("type");
const std::string LineStopLS::LINESTOP_TYPE_ATTR_DEPARTURE ("departure");
const std::string LineStopLS::LINESTOP_TYPE_ATTR_ARRIVAL ("arrival");
const std::string LineStopLS::LINESTOP_TYPE_ATTR_PASSAGE ("passage");

const std::string LineStopLS::LINESTOP_PHYSICALSTOPID_ATTR ("physicalStopId");
const std::string LineStopLS::LINESTOP_SCHEDULEINPUT_ATTR ("scheduleInput");


synthese::env::LineStop* 
LineStopLS::Load (XMLNode& node,
		  const synthese::env::Environment& environment)
{
    assert (LINESTOP_TAG == node.getName ());

    int id (su::Conversion::ToInt (
		node.getAttribute (LINESTOP_ID_ATTR.c_str())));

    std::string lineId (node.getAttribute (LINESTOP_LINEID_ATTR.c_str()));

    double metricOffset (su::Conversion::ToDouble (
	node.getAttribute (LINESTOP_METRICOFFSET_ATTR.c_str())));

    std::string typeStr (node.getAttribute (LINESTOP_TYPE_ATTR.c_str()));
    boost::to_lower (typeStr);

    synthese::env::LineStop::EdgeType type;
    if (typeStr == LINESTOP_TYPE_ATTR_DEPARTURE) 
    {
	type = synthese::env::LineStop::EDGE_TYPE_DEPARTURE;
    }
    else if (typeStr == LINESTOP_TYPE_ATTR_ARRIVAL) 
    {
	type = synthese::env::LineStop::EDGE_TYPE_ARRIVAL;
    }
    else if (typeStr == LINESTOP_TYPE_ATTR_PASSAGE) 
    {
	type = synthese::env::LineStop::EDGE_TYPE_PASSAGE;
    }
    else 
    {
	throw Exception ("Invalid value " + 
			 typeStr + 
			 " for type attribute on lineStop element.");
    }
    
    int physicalStopId (su::Conversion::ToInt (
	node.getAttribute (LINESTOP_PHYSICALSTOPID_ATTR.c_str())));

    bool scheduleInput (su::Conversion::ToInt (
	node.getAttribute (LINESTOP_SCHEDULEINPUT_ATTR.c_str())));

    synthese::env::LineStop* lineStop = new synthese::env::LineStop (
	id,
	environment.getLines ().get (lineId), 
	metricOffset, 
	type,
	environment.getPhysicalStops (). get (physicalStopId),
	scheduleInput);


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
}






}
}

