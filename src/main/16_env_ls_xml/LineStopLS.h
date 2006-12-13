#ifndef SYNTHESE_ENVLSXML_LINESTOPLS_H
#define SYNTHESE_ENVLSXML_LINESTOPLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Line;
    class LineStop;
}

namespace envlsxml
{


/** Line stop XML loading/saving service class.

Sample XML format :

@code
<lineStop id="1" metricOffset="352.2"
	  type="D" physicalStopId="3">
  <point x="120.5" y="4444.2"/>
  <point x="130.5" y="4434.4"/>
  <point x="140.2" y="4414.2"/>
</lineStop>
@endcode

 @ingroup m16
*/
class LineStopLS
{
 public:


    static const std::string LINESTOP_TAG;
    static const std::string LINESTOP_ID_ATTR;
    static const std::string LINESTOP_PHYSICALSTOPID_ATTR;
    static const std::string LINESTOP_LINEID_ATTR;
    static const std::string LINESTOP_RANKINPATH_ATTR;
    static const std::string LINESTOP_ISDEPARTURE_ATTR;
    static const std::string LINESTOP_ISARRIVAL_ATTR;
    static const std::string LINESTOP_METRICOFFSET_ATTR;


 private:

    LineStopLS ();
    ~LineStopLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a line stop from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::LineStop* lineStop);
    //@}


};



}
}


#endif

