#include "LineStopLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/LineStop.h"
#include "15_env/Line.h"

#include "PointLS.h"
#include "Exception.h"

#include <boost/algorithm/string.hpp>



using namespace synthese::util::XmlToolkit;

using synthese::env::Line;
using synthese::env::LineStop;


namespace synthese
{
namespace envlsxml
{

const std::string LineStopLS::LINESTOP_TAG ("lineStop");
const std::string LineStopLS::LINESTOP_ID_ATTR ("id");
const std::string LineStopLS::LINESTOP_PHYSICALSTOPID_ATTR ("physicalStopId");
const std::string LineStopLS::LINESTOP_LINEID_ATTR ("lineId");
const std::string LineStopLS::LINESTOP_RANKINPATH_ATTR ("rankInPath");
const std::string LineStopLS::LINESTOP_ISDEPARTURE_ATTR ("isDeparture");
const std::string LineStopLS::LINESTOP_ISARRIVAL_ATTR ("isArrival");
const std::string LineStopLS::LINESTOP_METRICOFFSET_ATTR ("metricOffset");




void
LineStopLS::Load (XMLNode& node,
		  synthese::env::Environment& environment)
{
    // assert (LINESTOP_TAG == node.getName ());

    uid id (GetLongLongAttr (node, LINESTOP_ID_ATTR));

    if (environment.getLineStops ().contains (id)) return;

    uid physicalStopId (GetLongLongAttr (node, LINESTOP_PHYSICALSTOPID_ATTR));
    uid lineId (GetLongLongAttr (node, LINESTOP_LINEID_ATTR));
    int rankInPath (GetIntAttr (node, LINESTOP_RANKINPATH_ATTR));
    bool isDeparture (GetBoolAttr (node, LINESTOP_ISDEPARTURE_ATTR));
    bool isArrival (GetBoolAttr (node, LINESTOP_ISARRIVAL_ATTR));
    double metricOffset (GetDoubleAttr (node, LINESTOP_METRICOFFSET_ATTR));

    Line* line = environment.getLines ().get (lineId);
    
    LineStop* lineStop = new LineStop (
	id,
	line, 
	rankInPath, 
	isDeparture, isArrival,
	metricOffset, 
	environment.getPhysicalStops (). get (physicalStopId));

    // Add via points
    int nbPoints = GetChildNodeCount(node, PointLS::POINT_TAG);
    for (int i=0; i<nbPoints; ++i) 
    {
	XMLNode pointNode = GetChildNode (node, PointLS::POINT_TAG, i);
	lineStop->addViaPoint (PointLS::Load (pointNode));
    }

    line->addEdge (lineStop);

    environment.getLineStops ().add (lineStop);
}




XMLNode* 
LineStopLS::Save (const synthese::env::LineStop* lineStop)
{
    // ...
    return 0;

}






}
}


