#ifndef SYNTHESE_ENVLSXML_PUBLICPLACELS_H
#define SYNTHESE_ENVLSXML_PUBLICPLACELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class PublicPlace;
}

namespace envlsxml
{


/** Public place XML loading/saving service class.

Sample XML format :

@code
<publicPlace id="1"
             name="Central station"
	     cityId="2"/>
@endcode

 @ingroup m16
*/
class PublicPlaceLS
{
 public:

    static const std::string PUBLICPLACE_TAG;
    static const std::string PUBLICPLACE_ID_ATTR;
    static const std::string PUBLICPLACE_NAME_ATTR;
    static const std::string PUBLICPLACE_CITYID_ATTR;



 private:

    PublicPlaceLS ();
    ~PublicPlaceLS();


 public:
    
    
    //! @name Query methods.
    //@{
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::PublicPlace* publicPlace);
    //@}


};



}
}


#endif

