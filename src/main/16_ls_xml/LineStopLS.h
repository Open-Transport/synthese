#ifndef SYNTHESE_LSXML_LINESTOPLS_H
#define SYNTHESE_LSXML_LINESTOPLS_H


#include "module.h"
#include <string>

class XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class LineStop;
}

namespace lsxml
{


/** Line stop XML loading/saving service class.

Sample XML format :

<lineStop id="4" lineId="1" metricOffset="352.2"
	  type="D" physicalStopId="3" scheduleInput="true">
  <point x="120.5" y="4444.2"/>
  <point x="130.5" y="4434.4"/>
  <point x="140.2" y="4414.2"/>
</lineStop>

 @ingroup m16
*/
class LineStopLS
{
 public:

    static const std::string LINESTOP_TAG;
    static const std::string LINESTOP_ID_ATTR;
    static const std::string LINESTOP_LINEID_ATTR;
    static const std::string LINESTOP_METRICOFFSET_ATTR;

    static const std::string LINESTOP_TYPE_ATTR;
    static const std::string LINESTOP_TYPE_ATTR_DEPARTURE;
    static const std::string LINESTOP_TYPE_ATTR_ARRIVAL;
    static const std::string LINESTOP_TYPE_ATTR_PASSAGE;

    static const std::string LINESTOP_PHYSICALSTOPID_ATTR;
    static const std::string LINESTOP_SCHEDULEINPUT_ATTR;

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
    static synthese::env::LineStop* Load (
	XMLNode& node,
	const synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::LineStop* lineStop);
    //@}


};



}
}


#endif
