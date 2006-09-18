#include "ContinuousServiceLS.h"

#include <assert.h>

#include <boost/tokenizer.hpp>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "04_time/Schedule.h"

#include "15_env/Environment.h"
#include "15_env/Path.h"
#include "15_env/ContinuousService.h"


using namespace synthese::util::XmlToolkit;

using synthese::env::Path;
using synthese::time::Schedule;
using synthese::env::ContinuousService;



namespace synthese
{
namespace envlsxml
{

const std::string ContinuousServiceLS::CONTINUOUSSERVICE_TAG ("continuousService");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_ID_ATTR ("id");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_SERVICENUMBER_ATTR ("serviceNumber");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_PATHID_ATTR ("pathId");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_RANGE_ATTR ("range");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_MAXWAITINGTIME_ATTR ("maxWaitingTime");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_BIKECOMPLIANCEID_ATTR ("bikeComplianceId");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_HANDICAPPEDCOMPLIANCEID_ATTR ("handicappedComplianceId");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_PEDESTRIANCOMPLIANCEID_ATTR ("pedestrianComplianceId");
const std::string ContinuousServiceLS::CONTINUOUSSERVICE_SCHEDULES_ATTR ("schedules");




void 
ContinuousServiceLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (CONTINUOUSSERVICE_TAG == node.getName ());
    uid id (GetLongLongAttr (node, CONTINUOUSSERVICE_ID_ATTR));

    if (environment.getContinuousServices ().contains (id)) return;

    uid pathId (GetLongLongAttr (node, CONTINUOUSSERVICE_PATHID_ATTR));

    int serviceNumber (GetIntAttr (node, CONTINUOUSSERVICE_SERVICENUMBER_ATTR));
    
    Path* path = environment.fetchPath (pathId);
    int range (GetIntAttr (node, CONTINUOUSSERVICE_RANGE_ATTR));
    int maxWaitingTime (GetIntAttr (node, CONTINUOUSSERVICE_MAXWAITINGTIME_ATTR));
    
    uid bikeComplianceId (GetLongLongAttr (node, CONTINUOUSSERVICE_BIKECOMPLIANCEID_ATTR));
    uid handicappedComplianceId (GetLongLongAttr (node, CONTINUOUSSERVICE_HANDICAPPEDCOMPLIANCEID_ATTR));
    uid pedestrianComplianceId (GetLongLongAttr (node, CONTINUOUSSERVICE_PEDESTRIANCOMPLIANCEID_ATTR));

    std::string schedules (GetStringAttr (node, CONTINUOUSSERVICE_SCHEDULES_ATTR));
    
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    // Parse all schedules arrival#departure,arrival#departure...
    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 ("#");
    tokenizer schedulesTokens (schedules, sep1);
    
    std::vector<synthese::time::Schedule> departureSchedules;
    std::vector<synthese::time::Schedule> arrivalSchedules;

    for (tokenizer::iterator schedulesIter = schedulesTokens.begin();
	 schedulesIter != schedulesTokens.end (); ++schedulesIter)
    {
	tokenizer schedulesTokens2 (*schedulesIter, sep2);
	tokenizer::iterator schedulesIter2 = schedulesTokens2.begin();
	
	departureSchedules.push_back (Schedule::FromString (*schedulesIter2));
	arrivalSchedules.push_back (Schedule::FromString (*(++schedulesIter2)));
    }
    
    assert (departureSchedules.size () > 0);
    assert (arrivalSchedules.size () > 0);

    
    ContinuousService* ss = new synthese::env::ContinuousService (id, serviceNumber, 
								  path, 
								  departureSchedules.at (0),
								  range,
								  maxWaitingTime);

    ss->setBikeCompliance (environment.getBikeCompliances ().get (bikeComplianceId));
    ss->setHandicappedCompliance (environment.getHandicappedCompliances ().get (handicappedComplianceId));
    ss->setPedestrianCompliance (environment.getPedestrianCompliances ().get (pedestrianComplianceId));

    path->addService (ss, departureSchedules, arrivalSchedules);
    environment.getContinuousServices ().add (ss);
}




XMLNode* 
ContinuousServiceLS::Save (const synthese::env::ContinuousService* continuousService)
{
    // ...
    return 0;
}






}
}

