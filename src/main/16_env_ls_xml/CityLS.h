#ifndef SYNTHESE_ENVLSXML_CITYLS_H
#define SYNTHESE_ENVLSXML_CITYLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class City;
}

namespace envlsxml
{


/** City XML loading/saving service class.

Sample XML format :

@code
<city id="1"
      name="Toulouse"/>
@endcode

 @ingroup m16
*/
class CityLS
{
 public:

    static const std::string CITY_TAG;
    static const std::string CITY_ID_ATTR;
    static const std::string CITY_NAME_ATTR;

 private:

    CityLS ();
    ~CityLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a city from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::City* city);
    //@}


};



}
}


#endif

