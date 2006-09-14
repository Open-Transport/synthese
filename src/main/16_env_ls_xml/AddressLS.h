#ifndef SYNTHESE_ENVLSXML_ADDRESSLS_H
#define SYNTHESE_ENVLSXML_ADDRESSLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Address;
}

namespace envlsxml
{


/** Address XML loading/saving service class.

Sample XML format :

@code
<address id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class AddressLS
{
 public:

    static const std::string ADDRESS_TAG;
    static const std::string ADDRESS_ID_ATTR;
    static const std::string ADDRESS_ROADID_ATTR;
    static const std::string ADDRESS_METRICOFFSET_ATTR;
    static const std::string ADDRESS_PLACEID_ATTR;
    static const std::string ADDRESS_X_ATTR;
    static const std::string ADDRESS_Y_ATTR;

 private:

    AddressLS ();
    ~AddressLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads an address from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Address* address);
    //@}


};



}
}


#endif
