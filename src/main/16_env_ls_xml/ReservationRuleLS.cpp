#include "ReservationRuleLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "04_time/Hour.h"

#include "15_env/Environment.h"
#include "15_env/ReservationRule.h"


using synthese::env::ReservationRule;
using synthese::env::City;
using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{


const std::string ReservationRuleLS::RESERVATIONRULE_TAG ("reservationRule");
const std::string ReservationRuleLS::RESERVATIONRULE_ID_ATTR ("id");

const std::string ReservationRuleLS::RESERVATIONRULE_TYPE_ATTR ("type");
const std::string ReservationRuleLS::RESERVATIONRULE_TYPE_ATTR_IMPOSSIBLE ("impossible");
const std::string ReservationRuleLS::RESERVATIONRULE_TYPE_ATTR_OPTIONAL ("optional");
const std::string ReservationRuleLS::RESERVATIONRULE_TYPE_ATTR_COMPULSORY ("compulsory");
const std::string ReservationRuleLS::RESERVATIONRULE_TYPE_ATTR_ATLEASTONE ("atLeastOne");

const std::string ReservationRuleLS::RESERVATIONRULE_ONLINE_ATTR ("online");
const std::string ReservationRuleLS::RESERVATIONRULE_ORIGINISREFERENCE_ATTR ("originIsReference");
const std::string ReservationRuleLS::RESERVATIONRULE_MINDELAYMINUTES_ATTR ("minDelayMinutes");
const std::string ReservationRuleLS::RESERVATIONRULE_MINDELAYDAYS_ATTR ("minDelayDays");
const std::string ReservationRuleLS::RESERVATIONRULE_MAXDELAYDAYS_ATTR ("maxDelayDays");
const std::string ReservationRuleLS::RESERVATIONRULE_HOURDEADLINE_ATTR ("hourDeadline");
const std::string ReservationRuleLS::RESERVATIONRULE_PHONEEXCHANGENUMBER_ATTR ("phoneExchangeNumber");
const std::string ReservationRuleLS::RESERVATIONRULE_PHONEEXCHANGEOPENINGHOURS_ATTR ("phoneExchangeOpeningHours");
const std::string ReservationRuleLS::RESERVATIONRULE_DESCRIPTION_ATTR ("description");
const std::string ReservationRuleLS::RESERVATIONRULE_WEBSITEURL_ATTR ("websiteURL");



void
ReservationRuleLS::Load (XMLNode& node,
			 synthese::env::Environment& environment)
{
    assert (RESERVATIONRULE_TAG == node.getName ());
    
    uid id (GetLongLongAttr (node, RESERVATIONRULE_ID_ATTR));

    if (environment.getReservationRules ().contains (id)) return;

    std::string typeStr (GetStringAttr (node, RESERVATIONRULE_TYPE_ATTR));
    
    ReservationRule::ReservationType type (ReservationRule::RESERVATION_TYPE_IMPOSSIBLE);
    if (typeStr == RESERVATIONRULE_TYPE_ATTR_IMPOSSIBLE) 
	type = ReservationRule::RESERVATION_TYPE_IMPOSSIBLE;

    else if (typeStr == RESERVATIONRULE_TYPE_ATTR_OPTIONAL) 
	type = ReservationRule::RESERVATION_TYPE_OPTIONAL;

    else if (typeStr == RESERVATIONRULE_TYPE_ATTR_COMPULSORY) 
	type = ReservationRule::RESERVATION_TYPE_COMPULSORY;

    else if (typeStr == RESERVATIONRULE_TYPE_ATTR_ATLEASTONE) 
	type = ReservationRule::RESERVATION_TYPE_AT_LEAST_ONE_REQUIRED;

    bool online (GetBoolAttr (node, RESERVATIONRULE_ONLINE_ATTR));
    bool originIsReference (GetBoolAttr (node, RESERVATIONRULE_ORIGINISREFERENCE_ATTR));

    int minDelayMinutes (GetIntAttr (node, RESERVATIONRULE_MINDELAYMINUTES_ATTR));
    int minDelayDays (GetIntAttr (node, RESERVATIONRULE_MINDELAYDAYS_ATTR));
    int maxDelayDays (GetIntAttr (node, RESERVATIONRULE_MAXDELAYDAYS_ATTR));

    synthese::time::Hour hourDeadLine (
	synthese::time::Hour::FromString (GetStringAttr (node, RESERVATIONRULE_HOURDEADLINE_ATTR)));
    
    std::string phoneExchangeNumber (GetStringAttr (node, RESERVATIONRULE_PHONEEXCHANGENUMBER_ATTR));  
    std::string phoneExchangeOpeningHours (GetStringAttr (node, RESERVATIONRULE_PHONEEXCHANGEOPENINGHOURS_ATTR));  
    std::string description (GetStringAttr (node, RESERVATIONRULE_DESCRIPTION_ATTR));  
    std::string webSiteUrl (GetStringAttr (node, RESERVATIONRULE_WEBSITEURL_ATTR));  
    
    ReservationRule* rr = new ReservationRule (id,
					       type,
					       online,
					       originIsReference,
					       minDelayMinutes,
					       minDelayDays,
					       maxDelayDays,
					       hourDeadLine,
					       phoneExchangeNumber,
					       phoneExchangeOpeningHours,
					       description,
					       webSiteUrl);
    
    environment.getReservationRules ().add (rr);
}




XMLNode* 
ReservationRuleLS::Save (const synthese::env::ReservationRule* publicPlace)
{
    // ...
    return 0;

}






}
}


