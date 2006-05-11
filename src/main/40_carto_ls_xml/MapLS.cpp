#include "MapLS.h"
#include "DrawableLineLS.h"


#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"
#include "01_util/Log.h"

#include "15_env/Environment.h"

#include "39_carto/Map.h"
#include "39_carto/MapBackgroundManager.h"
#include "39_carto/Rectangle.h"


using synthese::carto::MapBackgroundManager;
using synthese::util::Log;

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
const std::string MapLS::MAP_OUTPUTHORIZONTALMARGIN_ATTR ("outputHorizontalMargin");
const std::string MapLS::MAP_OUTPUTVERTICALMARGIN_ATTR ("outputVerticalMargin");

const std::string MapLS::MAP_BACKGROUNDID_ATTR ("backgroundId");
const std::string MapLS::MAP_URLPATTERN_ATTR ("urlPattern");


synthese::carto::Map* 
MapLS::Load (XMLNode& node,
	      const synthese::env::Environment& environment)
{
    // assert (MAP_TAG == node.getName ());

    int environmentId (su::Conversion::ToInt (
			   node.getAttribute (MAP_ENVIRONMENTID_ATTR.c_str())));

	int outputWidth = -1;
	int outputHeight = -1;
	if (node.getAttribute (MAP_OUTPUTWIDTH_ATTR.c_str()) != 0) 
	{
		outputWidth = (su::Conversion::ToInt (
			   node.getAttribute (MAP_OUTPUTWIDTH_ATTR.c_str())));
	}
	if (node.getAttribute (MAP_OUTPUTHEIGHT_ATTR.c_str()) != 0) 
	{
	    outputHeight = (su::Conversion::ToInt (
			   node.getAttribute (MAP_OUTPUTHEIGHT_ATTR.c_str())));
	}

    // Drawable lines
    std::set<synthese::carto::DrawableLine*> selectedLines;
    int nbDrawableLines = node.nChildNode(DrawableLineLS::DRAWABLELINE_TAG.c_str());
    for (int i=0; i<nbDrawableLines; ++i) 
    {
	XMLNode drawableLineNode = node.getChildNode (DrawableLineLS::DRAWABLELINE_TAG.c_str(), i);
	selectedLines.insert (DrawableLineLS::Load (drawableLineNode, environment));
    }

    const MapBackgroundManager* mbm = 0;

    if (node.getAttribute (MAP_BACKGROUNDID_ATTR.c_str()) != 0)
    {
	std::string backgroundId (node.getAttribute (MAP_BACKGROUNDID_ATTR.c_str()));
	try 
	{
	    mbm = MapBackgroundManager::GetMapBackgroundManager (backgroundId);
	}
	catch (synthese::util::Exception& ex)
	{
	    Log::GetInstance ().warn ("Cannot find background", ex);
	}
    }

    std::string urlPattern ("");
    if (node.getAttribute (MAP_URLPATTERN_ATTR.c_str()) != 0)
    {
	urlPattern  = node.getAttribute (MAP_URLPATTERN_ATTR.c_str());
    }

    synthese::carto::Map* map = 0;

    // If one of the 4 coordinates is missing, let the autofit 
    // feature process the right rectangle
    if ( (node.getAttribute (MAP_LOWERLEFTLATITUDE_ATTR.c_str()) == 0)  ||
	 (node.getAttribute (MAP_LOWERLEFTLONGITUDE_ATTR.c_str()) == 0) ||
	 (node.getAttribute (MAP_UPPERRIGHTLATITUDE_ATTR.c_str()) == 0) ||
	 (node.getAttribute (MAP_UPPERRIGHTLONGITUDE_ATTR.c_str()) == 0) )
    {
	map = new synthese::carto::Map (selectedLines,
					 outputWidth, 
					 outputHeight, 
					 mbm, urlPattern); 

    }
    else 
    {
	double lowerLeftLatitude (su::Conversion::ToDouble (
				      node.getAttribute (MAP_LOWERLEFTLATITUDE_ATTR.c_str())));
	double lowerLeftLongitude (su::Conversion::ToDouble (
				       node.getAttribute (MAP_LOWERLEFTLONGITUDE_ATTR.c_str())));
	
	double upperRightLatitude (su::Conversion::ToDouble (
				       node.getAttribute (MAP_UPPERRIGHTLATITUDE_ATTR.c_str())));
	double upperRightLongitude (su::Conversion::ToDouble (
					node.getAttribute (MAP_UPPERRIGHTLONGITUDE_ATTR.c_str())));


	 map = new synthese::carto::Map (selectedLines,
					 synthese::carto::Rectangle (lowerLeftLatitude,
								     lowerLeftLongitude,
								     upperRightLatitude - lowerLeftLatitude,
								     upperRightLongitude - lowerLeftLongitude),
					 outputWidth, 
					 outputHeight, 
					 mbm, urlPattern); 


    }

    if (node.getAttribute (MAP_OUTPUTHORIZONTALMARGIN_ATTR.c_str()) != 0)
    {
	    int outputHorizontalMargin = su::Conversion::ToInt (
            (node.getAttribute (MAP_OUTPUTHORIZONTALMARGIN_ATTR.c_str())));
        map->setHorizontalMargin (outputHorizontalMargin);
    }
    if (node.getAttribute (MAP_OUTPUTVERTICALMARGIN_ATTR.c_str()) != 0)
    {
	    int outputVerticalMargin = su::Conversion::ToInt (
            (node.getAttribute (MAP_OUTPUTVERTICALMARGIN_ATTR.c_str())));
        map->setVerticalMargin (outputVerticalMargin);
    }

    return map;
}




XMLNode* 
MapLS::Save (const synthese::carto::Map* map)
{
    // ...
    return 0;
}






}
}

