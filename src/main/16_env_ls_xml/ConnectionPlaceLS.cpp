#include "ConnectionPlaceLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/ConnectionPlace.h"


using synthese::env::ConnectionPlace;
using synthese::env::City;
using namespace synthese::util::XmlToolkit;


namespace synthese
{
namespace envlsxml
{

const std::string ConnectionPlaceLS::CONNECTIONPLACE_TAG ("connectionPlace");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_ID_ATTR ("id");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_NAME_ATTR ("name");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_CITYID_ATTR ("cityId");

const std::string ConnectionPlaceLS::CONNECTIONPLACE_CONNECTIONTYPE_ATTR ("connectionType");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_CONNECTIONTYPE_ATTR_FORBIDDEN ("forbidden");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_CONNECTIONTYPE_ATTR_ROADROAD ("road-road");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_CONNECTIONTYPE_ATTR_ROADLINE ("road-line");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_CONNECTIONTYPE_ATTR_LINELINE ("line-line");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_CONNECTIONTYPE_ATTR_RECOMMENDEDSHORT ("recommendedShort");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_CONNECTIONTYPE_ATTR_RECOMMENDED ("recommended");

const std::string ConnectionPlaceLS::CONNECTIONPLACE_ISCITYMAINCONNECTION_ATTR ("isCityMainConnection");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_DEFAULTTRANSFERDELAY_ATTR ("defaultTransferDelay");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_ALARMID_ATTR ("alarmId");

const std::string ConnectionPlaceLS::TRANSFERDELAY_TAG ("transferDelay");
const std::string ConnectionPlaceLS::TRANSFERDELAY_FROMVERTEXID_ATTR ("fromVertexId");
const std::string ConnectionPlaceLS::TRANSFERDELAY_TOVERTEXID_ATTR ("toVertexId");
const std::string ConnectionPlaceLS::TRANSFERDELAY_VALUE_ATTR ("value");





void
ConnectionPlaceLS::Load (XMLNode& node,
			 synthese::env::Environment& environment)
{
    assert (CONNECTIONPLACE_TAG == node.getName ());
    
    uid id (GetLongLongAttr (node, CONNECTIONPLACE_ID_ATTR));

    if (environment.getConnectionPlaces ().contains (id)) return;

    std::string name (GetStringAttr (node, CONNECTIONPLACE_NAME_ATTR));
    uid cityId (GetLongLongAttr (node, CONNECTIONPLACE_CITYID_ATTR));
    
    std::string typeStr (GetStringAttr (node, CONNECTIONPLACE_CONNECTIONTYPE_ATTR));

    ConnectionPlace::ConnectionType type = ConnectionPlace::CONNECTION_TYPE_FORBIDDEN;
    if (typeStr == CONNECTIONPLACE_CONNECTIONTYPE_ATTR_ROADROAD) 
	type = ConnectionPlace::CONNECTION_TYPE_ROADROAD;
    else if (typeStr == CONNECTIONPLACE_CONNECTIONTYPE_ATTR_ROADLINE) 
	type = ConnectionPlace::CONNECTION_TYPE_ROADLINE;
    else if (typeStr == CONNECTIONPLACE_CONNECTIONTYPE_ATTR_LINELINE) 
	type = ConnectionPlace::CONNECTION_TYPE_LINELINE;
    else if (typeStr == CONNECTIONPLACE_CONNECTIONTYPE_ATTR_RECOMMENDEDSHORT) 
	type = ConnectionPlace::CONNECTION_TYPE_RECOMMENDED_SHORT;
    else if (typeStr == CONNECTIONPLACE_CONNECTIONTYPE_ATTR_RECOMMENDED) 
	type = ConnectionPlace::CONNECTION_TYPE_RECOMMENDED;


    int defaultTransferDelay (GetIntAttr (node, 
					  CONNECTIONPLACE_DEFAULTTRANSFERDELAY_ATTR, 
					  ConnectionPlace::FORBIDDEN_TRANSFER_DELAY));

    City* city = environment.getCities ().get (cityId);

    ConnectionPlace* cp = new ConnectionPlace (id, name, city,
					       type, defaultTransferDelay);
    
    bool isCityMainConnection (GetBoolAttr (node, CONNECTIONPLACE_ISCITYMAINCONNECTION_ATTR, false));
    if (isCityMainConnection)
    {
	city->addIncludedPlace (cp);
    }
    
    if (HasAttr (node, CONNECTIONPLACE_ALARMID_ATTR))
    {
	uid alarmId (GetLongLongAttr (node, CONNECTIONPLACE_ALARMID_ATTR));
	cp->setAlarm (environment.getAlarms ().get (alarmId));
    }
    
    // Transfer delays
    for (int i=0; i<GetChildNodeCount (node, TRANSFERDELAY_TAG); ++i)
    {
	XMLNode td (GetChildNode (node, TRANSFERDELAY_TAG, i));
	cp->addTransferDelay (GetLongLongAttr (td, TRANSFERDELAY_FROMVERTEXID_ATTR),
			      GetLongLongAttr (td, TRANSFERDELAY_TOVERTEXID_ATTR),
			      GetIntAttr (td, TRANSFERDELAY_VALUE_ATTR));
    }

    city->getConnectionPlacesMatcher ().add (cp->getName (), cp);
    environment.getConnectionPlaces ().add (cp);
}




XMLNode* 
ConnectionPlaceLS::Save (const synthese::env::ConnectionPlace* connectionPlace)
{
    // ...
    return 0;

}






}
}

