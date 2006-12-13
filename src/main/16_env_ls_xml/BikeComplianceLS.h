#ifndef SYNTHESE_ENVLSXML_BIKECOMPLIANCELS_H
#define SYNTHESE_ENVLSXML_BIKECOMPLIANCELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class BikeCompliance;
}

namespace envlsxml
{


/** BikeCompliance XML loading/saving service class.

Sample XML format :

@code
<axis id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class BikeComplianceLS
{
 public:

    static const std::string BIKECOMPLIANCE_TAG;
    static const std::string BIKECOMPLIANCE_ID_ATTR;
    static const std::string BIKECOMPLIANCE_STATUS_ATTR;
    static const std::string BIKECOMPLIANCE_CAPACITY_ATTR;


 private:

    BikeComplianceLS ();
    ~BikeComplianceLS();


 public:
    
    
    //! @name Query methods.
    //@{

    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::BikeCompliance* axis);
    //@}


};



}
}


#endif

