#ifndef SYNTHESE_ENVLSXML_CONNECTIONPLACELS_H
#define SYNTHESE_ENVLSXML_CONNECTIONPLACELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class ConnectionPlace;
}

namespace envlsxml
{


/** Connection place XML loading/saving service class.

Sample XML format :

<connectionPlace id="1"
             name="Central station"
	     cityId="2"/>

 @ingroup m16
*/
class ConnectionPlaceLS
{
 public:

    static const std::string CONNECTIONPLACE_TAG;
    static const std::string CONNECTIONPLACE_ID_ATTR;
    static const std::string CONNECTIONPLACE_NAME_ATTR;
    static const std::string CONNECTIONPLACE_CITYID_ATTR;

 private:

    ConnectionPlaceLS ();
    ~ConnectionPlaceLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a connection place from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static synthese::env::ConnectionPlace* Load (
	XMLNode& node,
	const synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::ConnectionPlace* connectionPlace);
    //@}


};



}
}


#endif
