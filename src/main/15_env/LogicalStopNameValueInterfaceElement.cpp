
#include <string>

#include "01_util/UId.h"
#include "01_util/Conversion.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"

#include "15_env/Environment.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/LogicalStopNameValueInterfaceElement.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using std::string;

	namespace env
	{
		string LogicalStopNameValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const server::Request* request) const
		{
/*			if (_uid == NULL || Conversion::ToLongLong(_uid->getValue(parameters)) == 0 )
			{
*/				const ConnectionPlace* place = (ConnectionPlace*) object;
				return place->getName();
/*			}
			else
			{
				const Environment* __Environnement = request->getSite()->getEnvironment();
				uid placeUID = Conversion::ToLongLong(_uid->getValue(parameters));

				const ConnectionPlace* place = __Environnement->getConnectionPlaces().get(placeUID);
				return place->getName();
			}
*/		}

		void LogicalStopNameValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_uid = (vel.size() >= 1) ? vel.front() : NULL;
		}
	}

}
