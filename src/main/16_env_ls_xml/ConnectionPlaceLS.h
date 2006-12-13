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

@code
<connectionPlace id="1"
             name="Central station"
	     cityId="2"/>
@endcode

 @ingroup m16
*/
class ConnectionPlaceLS
{
 public:

    static const std::string CONNECTIONPLACE_TAG;
    static const std::string CONNECTIONPLACE_ID_ATTR;
    static const std::string CONNECTIONPLACE_NAME_ATTR;
    static const std::string CONNECTIONPLACE_CITYID_ATTR;
    static const std::string CONNECTIONPLACE_CONNECTIONTYPE_ATTR;

    static const std::string CONNECTIONPLACE_CONNECTIONTYPE_ATTR_FORBIDDEN;
    static const std::string CONNECTIONPLACE_CONNECTIONTYPE_ATTR_ROADROAD;
    static const std::string CONNECTIONPLACE_CONNECTIONTYPE_ATTR_ROADLINE;
    static const std::string CONNECTIONPLACE_CONNECTIONTYPE_ATTR_LINELINE;
    static const std::string CONNECTIONPLACE_CONNECTIONTYPE_ATTR_RECOMMENDEDSHORT;
    static const std::string CONNECTIONPLACE_CONNECTIONTYPE_ATTR_RECOMMENDED;

    static const std::string CONNECTIONPLACE_ISCITYMAINCONNECTION_ATTR;
    static const std::string CONNECTIONPLACE_DEFAULTTRANSFERDELAY_ATTR;
    static const std::string CONNECTIONPLACE_ALARMID_ATTR;

    static const std::string TRANSFERDELAY_TAG;
    static const std::string TRANSFERDELAY_FROMVERTEXID_ATTR;
    static const std::string TRANSFERDELAY_TOVERTEXID_ATTR;
    static const std::string TRANSFERDELAY_VALUE_ATTR;


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
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::ConnectionPlace* connectionPlace);
    //@}


};



}
}


#endif

