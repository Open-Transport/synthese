#ifndef SYNTHESE_ENVLSXML_PHYSICALSTOPLS_H
#define SYNTHESE_ENVLSXML_PHYSICALSTOPLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class PhysicalStop;
}

namespace envlsxml
{


/** Physical stop XML loading/saving service class.

Sample XML format :

@code
<physicalStop id="3"
              name="City Hall" 
              connectionPlaceId="2"
              rankInConnectionPlace="1"
	      x="1334.5"
	      y="123.2"/>
@endcode

 @ingroup m16
*/
class PhysicalStopLS
{
 public:

    static const std::string PHYSICALSTOP_TAG;
    static const std::string PHYSICALSTOP_ID_ATTR;
    static const std::string PHYSICALSTOP_NAME_ATTR;
    static const std::string PHYSICALSTOP_CONNECTIONPLACEID_ATTR;
    static const std::string PHYSICALSTOP_RANKINCONNECTIONPLACE_ATTR;

 private:

    PhysicalStopLS ();
    ~PhysicalStopLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a physical stop from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static synthese::env::PhysicalStop* Load (
	XMLNode& node,
	const synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::PhysicalStop* physicalStop);
    //@}


};



}
}


#endif
