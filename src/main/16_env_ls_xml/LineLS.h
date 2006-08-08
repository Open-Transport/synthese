#ifndef SYNTHESE_ENVLSXML_LINELS_H
#define SYNTHESE_ENVLSXML_LINELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Line;
}

namespace envlsxml
{


/** Line XML loading/saving service class.

Sample XML format :

@code
<line id="5"
      name="L5"
      axisId="A2"
      color="red">

   <lineStop id="1" metricOffset="0"
             type="departure" physicalStopId="1">
     <point x="120.5" y="4444.2"/>
     <point x="130.5" y="4434.4"/>
     <point x="140.2" y="4414.2"/>
   </lineStop>

   <lineStop id="2" metricOffset="100"
             type="passage" physicalStopId="2"/>

</line>
@endcode

 @ingroup m16
*/
class LineLS
{
 public:

    static const std::string LINE_TAG;
    static const std::string LINE_ID_ATTR;
    static const std::string LINE_NAME_ATTR;
    static const std::string LINE_AXISID_ATTR;
    static const std::string LINE_COLOR_ATTR;

 private:

    LineLS ();
    ~LineLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a line from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static synthese::env::Line* Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Line* line);
    //@}


};



}
}


#endif
