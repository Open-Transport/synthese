#ifndef SYNTHESE_LSXML_PHYSICALSTOPLS_H
#define SYNTHESE_LSXML_PHYSICALSTOPLS_H


#include "module.h"
#include <string>

class XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class PhysicalStop;
}

namespace lsxml
{


/** Physical stop XML loading/saving service class.

Sample XML format :

<physicalStop name="City Hall" 
              logicalStopId="2"
              rankInLogicalStop="1"/>

 @ingroup m16
*/
class PhysicalStopLS
{
 public:

    static const std::string PHYSICAL_STOP_TAG;
    static const std::string PHYSICAL_STOP_NAME_ATTR;
    static const std::string PHYSICAL_STOP_LOGICALSTOPID_ATTR;
    static const std::string PHYSICAL_STOP_RANKINLOGICALSTOP_ATTR;

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
