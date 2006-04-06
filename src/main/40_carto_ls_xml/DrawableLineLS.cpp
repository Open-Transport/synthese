#include "DrawableLineLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"
#include "39_carto/DrawableLine.h"



namespace su = synthese::util;

namespace synthese
{

namespace cartolsxml
{


const std::string DrawableLineLS::DRAWABLELINE_TAG ("drawableLine");
const std::string DrawableLineLS::DRAWABLELINE_LINEID_ATTR ("lineId");
const std::string DrawableLineLS::DRAWABLELINE_FROMLINESTOPINDEX_ATTR ("fromLineStopId");
const std::string DrawableLineLS::DRAWABLELINE_TOLINESTOPINDEX_ATTR ("toLineStopId");
const std::string DrawableLineLS::DRAWABLELINE_WITHPHYSICALSTOPS_ATTR ("withPhysicalStops");




synthese::carto::DrawableLine* 
DrawableLineLS::Load (XMLNode& node,
		      const synthese::env::Environment& environment)
{
    // assert (DRAWABLELINE_TAG == node.getName ());

    std::string lineId (node.getAttribute (DRAWABLELINE_LINEID_ATTR.c_str()));

    const synthese::env::Line* line = environment.getLines ().get (lineId);
    const std::vector<synthese::env::LineStop*>& lineStops = line->getLineStops ();
    
    assert (lineStops.size () >= 2);

    int fromLineStopIndex (0);
    int toLineStopIndex (lineStops.size () - 1);

    if (node.getAttribute (DRAWABLELINE_FROMLINESTOPINDEX_ATTR.c_str()) != 0)
    {
	fromLineStopIndex = su::Conversion::ToInt (
	    node.getAttribute (DRAWABLELINE_FROMLINESTOPINDEX_ATTR.c_str()));
    }
    if (node.getAttribute (DRAWABLELINE_TOLINESTOPINDEX_ATTR.c_str()) != 0)
    {
	toLineStopIndex = su::Conversion::ToInt (
	    node.getAttribute (DRAWABLELINE_TOLINESTOPINDEX_ATTR.c_str()));
    }
    bool withPhysicalStops = false;
    if (node.getAttribute (DRAWABLELINE_WITHPHYSICALSTOPS_ATTR.c_str()) != 0)
    {
	withPhysicalStops = su::Conversion::ToBool (
	    node.getAttribute (DRAWABLELINE_WITHPHYSICALSTOPS_ATTR.c_str()));
    }

    return new synthese::carto::DrawableLine (line, 
					      fromLineStopIndex, 
					      toLineStopIndex,
                          withPhysicalStops);
}




XMLNode* 
DrawableLineLS::Save (const synthese::carto::DrawableLine* drawableLine)
{
    // ...
    return 0;
}






}
}

