#ifndef SYNTHESE_ENVLSXML_PEDESTRIANCOMPLIANCELS_H
#define SYNTHESE_ENVLSXML_PEDESTRIANCOMPLIANCELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class PedestrianCompliance;
}

namespace envlsxml
{


/** PedestrianCompliance XML loading/saving service class.

Sample XML format :

@code
<axis id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class PedestrianComplianceLS
{
 public:

    static const std::string PEDESTRIANCOMPLIANCE_TAG;
    static const std::string PEDESTRIANCOMPLIANCE_ID_ATTR;
    static const std::string PEDESTRIANCOMPLIANCE_STATUS_ATTR;
    static const std::string PEDESTRIANCOMPLIANCE_CAPACITY_ATTR;


 private:

    PedestrianComplianceLS ();
    ~PedestrianComplianceLS();


 public:
    
    
    //! @name Query methods.
    //@{

    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::PedestrianCompliance* axis);
    //@}


};



}
}


#endif

