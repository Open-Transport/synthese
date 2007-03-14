#include "XmlBuilder.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"



using namespace synthese::util::XmlToolkit;



namespace synthese
{
namespace env
{


City* 
XmlBuilder::CreateCity (XMLNode& node)
{
    uid id (GetLongLongAttr (node, "id"));

    std::string name (GetStringAttr (node, "name"));
    
    return new City (id, name);
}




 Axis* 
 XmlBuilder::CreateAxis (XMLNode& node)
 {
    uid id (GetLongLongAttr (node, "id"));

    std::string name (GetStringAttr (node, "name"));

    bool free (GetBoolAttr (node, "free"));
    bool authorized (GetBoolAttr (node, "authorized"));
    
    return new synthese::env::Axis (id, name, free, authorized);
 }



    
ConnectionPlace* 
XmlBuilder::CreateConnectionPlace (XMLNode& node, 
				   const City::Registry& cities)
{
    // assert ("connectionPlace" == node.getName ());
    
    uid id (GetLongLongAttr (node, "id"));

    std::string name (GetStringAttr (node, "name"));
    uid cityId (GetLongLongAttr (node, "cityId"));
    
    std::string typeStr (GetStringAttr (node, "connectionType"));

    ConnectionPlace::ConnectionType type = ConnectionPlace::CONNECTION_TYPE_FORBIDDEN;
    if (typeStr == "road-road") 
	type = ConnectionPlace::CONNECTION_TYPE_ROADROAD;
    else if (typeStr == "road-line") 
	type = ConnectionPlace::CONNECTION_TYPE_ROADLINE;
    else if (typeStr == "line-line") 
	type = ConnectionPlace::CONNECTION_TYPE_LINELINE;
    else if (typeStr == "recommendedShort") 
	type = ConnectionPlace::CONNECTION_TYPE_RECOMMENDED_SHORT;
    else if (typeStr == "recommended") 
	type = ConnectionPlace::CONNECTION_TYPE_RECOMMENDED;


    int defaultTransferDelay (GetIntAttr (node, 
					  "defaultTransferDelay", 
					  ConnectionPlace::FORBIDDEN_TRANSFER_DELAY));

    const City* city = cities.get (cityId);

    return new ConnectionPlace (id, name, city,
				type, defaultTransferDelay);
    

}


CommercialLine* 
XmlBuilder::CreateCommercialLine (XMLNode& node)
{
    uid id (GetLongLongAttr (node, "id"));

    CommercialLine* commercialLine = new CommercialLine ();

    commercialLine->setKey (id);

    std::string shortName (GetStringAttr (node, "shortName"));
    commercialLine->setShortName (shortName);

    std::string longName (GetStringAttr (node, "longName"));
    commercialLine->setLongName (longName);

    std::string color (GetStringAttr (node, "color"));
    commercialLine->setColor (synthese::util::RGBColor (color));

    std::string style (GetStringAttr (node, "style"));
    commercialLine->setStyle (style);

    return commercialLine;
}




Line* 
XmlBuilder::CreateLine (XMLNode& node, 
			const Axis::Registry& axes,
			const CommercialLine::Registry& commercialLines)
{
    uid id (GetLongLongAttr (node, "id"));

    std::string name (GetStringAttr (node, "name"));
    uid axisId (GetLongLongAttr (node, "axisId"));

    synthese::env::Line* line = new synthese::env::Line (id, name, 
							 axes.get (axisId));

    uid commercialLineId (GetLongLongAttr (node, "commercialLineId"));
    line->setCommercialLine (commercialLines.get (commercialLineId));
 
    std::string direction (GetStringAttr (node, "direction"));
    line->setDirection (direction);
   
    return line;
}




LineStop* 
XmlBuilder::CreateLineStop (XMLNode& node, 
			    Line::Registry& lines,
			    const PhysicalStop::Registry& physicalStops)
{
    // assert ("lineStop" == node.getName ());

    uid id (GetLongLongAttr (node, "id"));

    uid physicalStopId (GetLongLongAttr (node, "physicalStopId"));
    uid lineId (GetLongLongAttr (node, "lineId"));
    int rankInPath (GetIntAttr (node, "rankInPath"));
    bool isDeparture (GetBoolAttr (node, "isDeparture"));
    bool isArrival (GetBoolAttr (node, "isArrival"));
    double metricOffset (GetDoubleAttr (node, "metricOffset"));
   
    Line* line = lines.get (lineId);

    LineStop* lineStop = new LineStop (
	id,
	line, 
	rankInPath, 
	isDeparture, isArrival,
	metricOffset, 
	physicalStops. get (physicalStopId));

    // Add via points
    int nbPoints = GetChildNodeCount(node, "point");
    for (int i=0; i<nbPoints; ++i) 
    {
	XMLNode pointNode = GetChildNode (node, "point", i);
	lineStop->addViaPoint (CreatePoint (pointNode));
    }

    line->addEdge (lineStop);

    return lineStop;

}





PhysicalStop* 
XmlBuilder::CreatePhysicalStop (XMLNode& node, const ConnectionPlace::Registry& connectionPlaces)
{
    // assert ("physicalStop" == node.getName ());
    uid id (GetLongLongAttr (node, "id"));
    std::string name (GetStringAttr (node, "name"));
    uid placeId (GetLongLongAttr (node, "placeId"));
    double x (GetDoubleAttr (node, "x"));
    double y (GetDoubleAttr (node, "y"));

    return new synthese::env::PhysicalStop (
	id,
	name, 
	connectionPlaces.get (placeId), 
	x, y);

}



Point
XmlBuilder::CreatePoint (XMLNode& node)
{
    // assert ("point" == node.getName ());

    double x (GetDoubleAttr (node, "x"));
    double y (GetDoubleAttr (node, "y"));

    return Point (x, y);
}






}
}


