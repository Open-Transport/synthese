#include "MapLS.h"
#include "DrawableLineLS.h"


#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"

#include "15_env/Environment.h"

#include "39_carto/Map.h"
#include "39_carto/Rectangle.h"


namespace su = synthese::util;

namespace synthese
{
namespace cartolsxml
{

const std::string MapLS::MAP_TAG ("map");
const std::string MapLS::MAP_ENVIRONMENTID_ATTR ("environmentId");
    
const std::string MapLS::MAP_LOWERLEFTLATITUDE_ATTR ("lowerLeftLatitude");
const std::string MapLS::MAP_LOWERLEFTLONGITUDE_ATTR ("lowerLeftLongitude");
const std::string MapLS::MAP_UPPERRIGHTLATITUDE_ATTR ("upperRightLatitude");
const std::string MapLS::MAP_UPPERRIGHTLONGITUDE_ATTR ("upperRightLongitude");
const std::string MapLS::MAP_OUTPUTWIDTH_ATTR ("outputWidth");
const std::string MapLS::MAP_OUTPUTHEIGHT_ATTR ("outputHeight");
const std::string MapLS::MAP_BACKGROUNDID_ATTR ("backgroundId");


synthese::carto::Map* 
MapLS::Load (XMLNode& node,
	      const synthese::env::Environment& environment)
{
    // assert (MAP_TAG == node.getName ());

    int environmentId (su::Conversion::ToInt (
			   node.getAttribute (MAP_ENVIRONMENTID_ATTR.c_str())));

    double lowerLeftLatitude (su::Conversion::ToDouble (
			 node.getAttribute (MAP_LOWERLEFTLATITUDE_ATTR.c_str())));
    double lowerLeftLongitude (su::Conversion::ToDouble (
			 node.getAttribute (MAP_LOWERLEFTLONGITUDE_ATTR.c_str())));

    double upperRightLatitude (su::Conversion::ToDouble (
			 node.getAttribute (MAP_UPPERRIGHTLATITUDE_ATTR.c_str())));
    double upperRightLongitude (su::Conversion::ToDouble (
			 node.getAttribute (MAP_UPPERRIGHTLONGITUDE_ATTR.c_str())));

    int outputWidth (su::Conversion::ToInt (
			   node.getAttribute (MAP_OUTPUTWIDTH_ATTR.c_str())));
    int outputHeight (su::Conversion::ToInt (
			   node.getAttribute (MAP_OUTPUTHEIGHT_ATTR.c_str())));

    int backgroundId (su::Conversion::ToInt (
			   node.getAttribute (MAP_BACKGROUNDID_ATTR.c_str())));

    // Drawable lines
    std::set<synthese::carto::DrawableLine*> selectedLines;
    int nbDrawableLines = node.nChildNode(DrawableLineLS::DRAWABLELINE_TAG.c_str());
    for (int i=0; i<nbDrawableLines; ++i) 
    {
	XMLNode drawableLineNode = node.getChildNode (DrawableLineLS::DRAWABLELINE_TAG.c_str(), i);
	selectedLines.insert (DrawableLineLS::Load (drawableLineNode, environment));
    }

    return new synthese::carto::Map (selectedLines,
				     synthese::carto::Rectangle (lowerLeftLatitude,
								 lowerLeftLongitude,
								 upperRightLatitude - lowerLeftLatitude,
								 upperRightLongitude - lowerLeftLongitude),
				     outputWidth, outputHeight, 0); // TODO add bm management
}




XMLNode* 
MapLS::Save (const synthese::carto::Map* map)
{
    // ...
}






}
}

