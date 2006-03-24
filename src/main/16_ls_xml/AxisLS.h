#ifndef SYNTHESE_LSXML_AXISLS_H
#define SYNTHESE_LSXML_AXISLS_H


#include "module.h"
#include <string>

class XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Axis;
}

namespace lsxml
{


/** Axis XML loading/saving service class.

Sample XML format :

<axis id="1"
      free="true"
      authorized="false"/>

 @ingroup m16
*/
class AxisLS
{
 public:

    static const std::string AXIS_TAG;
    static const std::string AXIS_ID_ATTR;
    static const std::string AXIS_FREE_ATTR;
    static const std::string AXIS_AUTHORIZED_ATTR;

 private:

    AxisLS ();
    ~AxisLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads an axis from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static synthese::env::Axis* Load (
	XMLNode& node,
	const synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Axis* axis);
    //@}


};



}
}


#endif
