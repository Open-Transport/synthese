#include "AlarmLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"
#include "01_util/XmlToolkit.h"

#include "15_env/Environment.h"
#include "15_env/Alarm.h"

#include "04_time/DateTime.h"



using namespace synthese::util::XmlToolkit;

using synthese::env::Alarm;
using synthese::time::DateTime;


namespace synthese
{
namespace envlsxml
{

const std::string AlarmLS::ALARM_TAG ("alarm");
const std::string AlarmLS::ALARM_ID_ATTR ("id");
const std::string AlarmLS::ALARM_MESSAGE_ATTR ("message");
const std::string AlarmLS::ALARM_PERIODSTART_ATTR ("periodStart");
const std::string AlarmLS::ALARM_PERIODEND_ATTR ("periodEnd");

const std::string AlarmLS::ALARM_LEVEL_ATTR ("level");
const std::string AlarmLS::ALARM_LEVEL_ATTR_INFO ("info");
const std::string AlarmLS::ALARM_LEVEL_ATTR_WARNING ("warning");





void 
AlarmLS::Load (XMLNode& node,
	      synthese::env::Environment& environment)
{
    // assert (ALARM_TAG == node.getName ());
    uid id (GetLongLongAttr (node, ALARM_ID_ATTR));

    if (environment.getAlarms ().contains (id)) return;

    std::string message (GetStringAttr (node, ALARM_MESSAGE_ATTR));
    DateTime periodStart (DateTime::FromString (GetStringAttr (node, ALARM_PERIODSTART_ATTR)));
    DateTime periodEnd (DateTime::FromString (GetStringAttr (node, ALARM_PERIODEND_ATTR)));

    std::string levelStr (GetStringAttr (node, ALARM_LEVEL_ATTR));
    Alarm::AlarmLevel level (Alarm::ALARM_LEVEL_INFO);
    if (levelStr == ALARM_LEVEL_ATTR_INFO) level = Alarm::ALARM_LEVEL_INFO;
    else if (levelStr == ALARM_LEVEL_ATTR_WARNING) level = Alarm::ALARM_LEVEL_WARNING;
    
    environment.getAlarms ().add (new synthese::env::Alarm (id, 
							    message, 
							    periodStart,
							    periodEnd,
							    level));
    
}




XMLNode* 
AlarmLS::Save (const synthese::env::Alarm* alarm)
{
    // ...
    return 0;
}






}
}


