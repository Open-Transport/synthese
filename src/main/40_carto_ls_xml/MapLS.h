#ifndef SYNTHESE_CARTOLSXML_MAPLS_H
#define SYNTHESE_CARTOLSXML_MAPLS_H


#include "module.h"
#include <string>

class XMLNode;


namespace synthese
{
namespace carto
{
    class DrawableLine;
    class Map;
}
namespace env
{
    class Environment;
}


namespace cartolsxml
{


/** Map XML loading/saving service class.

Sample XML format :

<map environmentId="0"
     lowerLeftLatitude="526408" lowerLeftLongitude="1840480"
     upperRightLatitude="526908" upperRightLongitude="1840980"
     outputWidth="800" outputHeight="800"
     outputHorizontalMargin="100" outputVerticalMargin="20"
     backgroundId="TLSE" urlPattern="http://localhost:8080?id=$id">

  <drawableLine lineId="L1" fromLineStopId="5" toLineStopId="10"/>
  <drawableLine lineId="2" fromLineStopId="3"/>
  <drawableLine lineId="N4" toLineStopId="2"/>
  <drawableLine lineId="5"/>

</map>

 @ingroup m40
*/
class MapLS
{
 public:

    static const std::string MAP_TAG;
    static const std::string MAP_ENVIRONMENTID_ATTR;

    static const std::string MAP_LOWERLEFTLATITUDE_ATTR;
    static const std::string MAP_LOWERLEFTLONGITUDE_ATTR;
    static const std::string MAP_UPPERRIGHTLATITUDE_ATTR;
    static const std::string MAP_UPPERRIGHTLONGITUDE_ATTR;
    static const std::string MAP_OUTPUTWIDTH_ATTR;
    static const std::string MAP_OUTPUTHEIGHT_ATTR;
    static const std::string MAP_OUTPUTHORIZONTALMARGIN_ATTR;
    static const std::string MAP_OUTPUTVERTICALMARGIN_ATTR;
    static const std::string MAP_BACKGROUNDID_ATTR;
    static const std::string MAP_URLPATTERN_ATTR;

 private:

    MapLS ();
    ~MapLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a map from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static synthese::carto::Map* Load (
	XMLNode& node,
	const synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::carto::Map* map);
    //@}


};



}
}


#endif
