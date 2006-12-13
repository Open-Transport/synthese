#ifndef SYNTHESE_ENVLSXML_TRANSPORTNETWORKLS_H
#define SYNTHESE_ENVLSXML_TRANSPORTNETWORKLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class TransportNetwork;
}

namespace envlsxml
{


/** TransportNetwork XML loading/saving service class.

Sample XML format :

@code
<city id="1"
      name="Toulouse"/>
@endcode

 @ingroup m16
*/
class TransportNetworkLS
{
 public:

    static const std::string TRANSPORTNETWORK_TAG;
    static const std::string TRANSPORTNETWORK_ID_ATTR;
    static const std::string TRANSPORTNETWORK_NAME_ATTR;

 private:

    TransportNetworkLS ();
    ~TransportNetworkLS();


 public:
    
    
    //! @name Query methods.
    //@{

    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::TransportNetwork* city);
    //@}


};



}
}


#endif

