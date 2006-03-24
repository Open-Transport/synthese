#ifndef SYNTHESE_ENVLSXML_LOGICALSTOPLS_H
#define SYNTHESE_ENVLSXML_LOGICALSTOPLS_H


#include "module.h"
#include <string>

class XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class LogicalStop;
}

namespace envlsxml
{


/** Logical stop XML loading/saving service class.

Sample XML format :

<logicalStop id="1"
             name="Central station"
	     cityId="2"/>

 @ingroup m16
*/
class LogicalStopLS
{
 public:

    static const std::string LOGICALSTOP_TAG;
    static const std::string LOGICALSTOP_ID_ATTR;
    static const std::string LOGICALSTOP_NAME_ATTR;
    static const std::string LOGICALSTOP_CITYID_ATTR;

 private:

    LogicalStopLS ();
    ~LogicalStopLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a logical stop from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static synthese::env::LogicalStop* Load (
	XMLNode& node,
	const synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::LogicalStop* logicalStop);
    //@}


};



}
}


#endif
