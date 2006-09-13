#ifndef SYNTHESE_ENVLSXML_ROADLS_H
#define SYNTHESE_ENVLSXML_ROADLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Road;
}

namespace envlsxml
{


/** Road XML loading/saving service class.

Sample XML format :

@code
<road id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class RoadLS
{
 public:

    static const std::string ROAD_TAG;
    static const std::string ROAD_ID_ATTR;
    static const std::string ROAD_NAME_ATTR;
    static const std::string ROAD_CITYID_ATTR;

    static const std::string ROAD_TYPE_ATTR;
    static const std::string ROAD_TYPE_ATTR_UNKNOWN;
    static const std::string ROAD_TYPE_ATTR_MOTORWAY;
    static const std::string ROAD_TYPE_ATTR_MEDIANSTRIPPEDROAD;
    static const std::string ROAD_TYPE_ATTR_PRINCIPLEAXIS;
    static const std::string ROAD_TYPE_ATTR_SECONDARYAXIS;
    static const std::string ROAD_TYPE_ATTR_BRIDGE;
    static const std::string ROAD_TYPE_ATTR_STREET;
    static const std::string ROAD_TYPE_ATTR_PEDESTRIANSTREET;
    static const std::string ROAD_TYPE_ATTR_ACCESSROAD;
    static const std::string ROAD_TYPE_ATTR_PRIVATEWAY;
    static const std::string ROAD_TYPE_ATTR_PEDESTRIANPATH;
    static const std::string ROAD_TYPE_ATTR_TUNNEL;
    static const std::string ROAD_TYPE_ATTR_HIGHWAY;

    static const std::string ROAD_FAREID_ATTR;
    static const std::string ROAD_ALARMID_ATTR;
    static const std::string ROAD_BIKECOMPLIANCEID_ATTR;
    static const std::string ROAD_HANDICAPPEDCOMPLIANCEID_ATTR;
    static const std::string ROAD_PEDESTRIANCOMPLIANCEID_ATTR;
    static const std::string ROAD_RESERVATIONRULEID_ATTR;

 private:

    RoadLS ();
    ~RoadLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads an road from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Road* road);
    //@}


};



}
}


#endif
