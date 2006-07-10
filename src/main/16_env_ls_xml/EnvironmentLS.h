#ifndef SYNTHESE_ENVLSXML_ENVIRONMENTLS_H
#define SYNTHESE_ENVLSXML_ENVIRONMENTLS_H


#include "module.h"
#include <string>

class XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
}

namespace envlsxml
{


/** Environment XML loading/saving service class.

Sample XML format :

Warning: ordering of child elements is important!

<environment id="0">

 <cities>
   <city id="1" name="Toulouse"/>
   <city id="2" name="Colomiers"/>
   <!-- ... -->
 </cities>

 <axes>
   <axis id="a1" free="true" authorized="false">  
   <axis id="a2" free="true" authorized="false">  
   <!-- ... -->
 </axes>

 <connectionPlaces>
   <connectionPlace id="1" name="Matabiau" cityId="1"/>
   <!-- ... -->
 </connectionPlaces>

 <physicalStops>
   <physicalStop id="1" name="Metro gare SNCF" 
                 connectionPlaceId="1" rankInConnectionPlace="1"
	         x="1334.5" y="123.2"/>
   <!-- ... -->
 </physicalStops>

 <lines>
   <line id="l1" axisId="a1" firstYear="2006" lastYear="2007">

     <lineStop id="1" metricOffset="0"
	       type="departure" physicalStopId="1" scheduleInput="true">
       <point x="120.5" y="4444.2"/>
       <point x="130.5" y="4434.4"/>
       <point x="140.2" y="4414.2"/>
     </lineStop>

     <lineStop id="2" metricOffset="100"
	       type="passage" physicalStopId="2" scheduleInput="true"/>

   </line>

   <line id="l2" axisId="a2" firstYear="2006" lastYear="2007">

   </line>

   <!-- ... -->
 </lines>


</environment>


 @ingroup m16
*/
class EnvironmentLS
{
 public:

    static const std::string ENVIRONMENT_TAG;
    static const std::string ENVIRONMENT_ID_ATTR;
    static const std::string ENVIRONMENT_CITIES_TAG;
    static const std::string ENVIRONMENT_AXES_TAG;
    static const std::string ENVIRONMENT_CONNECTIONPLACES_TAG;
    static const std::string ENVIRONMENT_PHYSICALSTOPS_TAG;
    static const std::string ENVIRONMENT_LINES_TAG;

 private:

    EnvironmentLS ();
    ~EnvironmentLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads an environment from an XML DOM node.
     */
    static synthese::env::Environment* Load (
	XMLNode& node);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Environment* environment);
    //@}


};



}
}


#endif
