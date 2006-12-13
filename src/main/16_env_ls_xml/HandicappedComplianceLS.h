#ifndef SYNTHESE_ENVLSXML_HANDICAPPEDCOMPLIANCELS_H
#define SYNTHESE_ENVLSXML_HANDICAPPEDCOMPLIANCELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class HandicappedCompliance;
}

namespace envlsxml
{


/** HandicappedCompliance XML loading/saving service class.

Sample XML format :

@code
<axis id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class HandicappedComplianceLS
{
 public:

    static const std::string HANDICAPPEDCOMPLIANCE_TAG;
    static const std::string HANDICAPPEDCOMPLIANCE_ID_ATTR;
    static const std::string HANDICAPPEDCOMPLIANCE_STATUS_ATTR;
    static const std::string HANDICAPPEDCOMPLIANCE_CAPACITY_ATTR;


 private:

    HandicappedComplianceLS ();
    ~HandicappedComplianceLS();


 public:
    
    
    //! @name Query methods.
    //@{

    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::HandicappedCompliance* axis);
    //@}


};



}
}


#endif

