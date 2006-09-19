#ifndef SYNTHESE_ENVLSXML_CONTINUOUSSERVICELS_H
#define SYNTHESE_ENVLSXML_CONTINUOUSSERVICELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class ContinuousService;
}

namespace envlsxml
{


/** Continuous service XML loading/saving service class.

Sample XML format :

@code
<address id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class ContinuousServiceLS
{
 public:

    static const std::string CONTINUOUSSERVICE_TAG;
    static const std::string CONTINUOUSSERVICE_ID_ATTR;
    static const std::string CONTINUOUSSERVICE_SERVICENUMBER_ATTR;
    static const std::string CONTINUOUSSERVICE_PATHID_ATTR;
    static const std::string CONTINUOUSSERVICE_RANGE_ATTR;
    static const std::string CONTINUOUSSERVICE_MAXWAITINGTIME_ATTR;
    static const std::string CONTINUOUSSERVICE_BIKECOMPLIANCEID_ATTR;
    static const std::string CONTINUOUSSERVICE_HANDICAPPEDCOMPLIANCEID_ATTR;
    static const std::string CONTINUOUSSERVICE_PEDESTRIANCOMPLIANCEID_ATTR;
    static const std::string CONTINUOUSSERVICE_SCHEDULES_ATTR;
    static const std::string SERVICEDATE_TAG;
    static const std::string SERVICEDATE_DATE_ATTR;

 private:

    ContinuousServiceLS ();
    ~ContinuousServiceLS();


 public:
    
    
    //! @name Query methods.
    //@{

    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::ContinuousService* continuousService);
    //@}


};



}
}


#endif
