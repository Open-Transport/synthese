#ifndef SYNTHESE_ENVLSXML_FARELS_H
#define SYNTHESE_ENVLSXML_FARELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Fare;
}

namespace envlsxml
{


/** Fare XML loading/saving service class.

Sample XML format :

@code
<fare id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class FareLS
{
 public:

    static const std::string FARE_TAG;
    static const std::string FARE_ID_ATTR;
    static const std::string FARE_NAME_ATTR;
    static const std::string FARE_TYPE_ATTR;
    static const std::string FARE_TYPE_ATTR_ZONING;
    static const std::string FARE_TYPE_ATTR_SECTION;
    static const std::string FARE_TYPE_ATTR_DISTANCE;



 private:

    FareLS ();
    ~FareLS();


 public:
    
    
    //! @name Query methods.
    //@{
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Fare* fare);
    //@}


};



}
}


#endif

