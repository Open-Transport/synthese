#ifndef SYNTHESE_ENVLSXML_SCHEDULEDSERVICELS_H
#define SYNTHESE_ENVLSXML_SCHEDULEDSERVICELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class ScheduledService;
}

namespace envlsxml
{


/** ScheduledService XML loading/saving service class.

Sample XML format :

@code
<axis id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class ScheduledServiceLS
{
 public:

    static const std::string SCHEDULEDSERVICE_TAG;
    static const std::string SCHEDULEDSERVICE_ID_ATTR;
    static const std::string SCHEDULEDSERVICE_SERVICENUMBER_ATTR;
    static const std::string SCHEDULEDSERVICE_PATHID_ATTR;
    static const std::string SCHEDULEDSERVICE_BIKECOMPLIANCEID_ATTR;
    static const std::string SCHEDULEDSERVICE_HANDICAPPEDCOMPLIANCEID_ATTR;
    static const std::string SCHEDULEDSERVICE_PEDESTRIANCOMPLIANCEID_ATTR;
    static const std::string SCHEDULEDSERVICE_RESERVATIONRULEID_ATTR;
    static const std::string SCHEDULEDSERVICE_SCHEDULES_ATTR;
    static const std::string SERVICEDATE_TAG;
    static const std::string SERVICEDATE_DATE_ATTR;


 private:

    ScheduledServiceLS ();
    ~ScheduledServiceLS();


 public:
    
    
    //! @name Query methods.
    //@{

    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::ScheduledService* axis);
    //@}


};



}
}


#endif
