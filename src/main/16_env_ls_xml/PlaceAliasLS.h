#ifndef SYNTHESE_ENVLSXML_PLACEALIASLS_H
#define SYNTHESE_ENVLSXML_PLACEALIASLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class PlaceAlias;
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
class PlaceAliasLS
{
 public:

    static const std::string PLACEALIAS_TAG;
    static const std::string PLACEALIAS_ID_ATTR;
    static const std::string PLACEALIAS_NAME_ATTR;
    static const std::string PLACEALIAS_ALIASEDPLACEID_ATTR;
    static const std::string PLACEALIAS_CITYID_ATTR;



 private:

    PlaceAliasLS ();
    ~PlaceAliasLS();


 public:
    
    
    //! @name Query methods.
    //@{
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::PlaceAlias* publicPlace);
    //@}


};



}
}


#endif
