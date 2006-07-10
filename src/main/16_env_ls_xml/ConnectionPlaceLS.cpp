#include "ConnectionPlaceLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlParser.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/ConnectionPlace.h"


namespace su = synthese::util;

namespace synthese
{
namespace envlsxml
{

const std::string ConnectionPlaceLS::CONNECTIONPLACE_TAG ("connectionPlace");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_ID_ATTR ("id");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_NAME_ATTR ("name");
const std::string ConnectionPlaceLS::CONNECTIONPLACE_CITYID_ATTR ("cityId");


synthese::env::ConnectionPlace* 
ConnectionPlaceLS::Load (XMLNode& node,
	      const synthese::env::Environment& environment)
{
    assert (CONNECTIONPLACE_TAG == node.getName ());

    uid id (su::Conversion::ToLongLong (
		node.getAttribute (CONNECTIONPLACE_ID_ATTR.c_str())));
    std::string name (node.getAttribute (CONNECTIONPLACE_NAME_ATTR.c_str()));

    int cityId (su::Conversion::ToInt (
		    node.getAttribute (CONNECTIONPLACE_CITYID_ATTR.c_str())));

    return new synthese::env::ConnectionPlace (id, name,
					   environment.getCities ().get (cityId));
}




XMLNode* 
ConnectionPlaceLS::Save (const synthese::env::ConnectionPlace* connectionPlace)
{
    // ...
    return 0;

}






}
}

