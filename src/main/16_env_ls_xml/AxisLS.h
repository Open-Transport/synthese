#ifndef SYNTHESE_ENVLSXML_AXISLS_H
#define SYNTHESE_ENVLSXML_AXISLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Axis;
}

namespace envlsxml
{


/** Axis XML loading/saving service class.

Sample XML format :

@code
<axis id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class AxisLS
{
 public:

    static const std::string AXIS_TAG;
    static const std::string AXIS_ID_ATTR;
    static const std::string AXIS_NAME_ATTR;
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
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Axis* axis);
    //@}


};



}
}


#endif

