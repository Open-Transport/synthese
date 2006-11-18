
#include "LogicalStopNameValueInterfaceElement.h"
#include "15_env/Environment.h"
#include "15_env/ConnectionPlace.h"
#include "01_util/UId.h"
#include "01_util/Conversion.h"
#include "11_interfaces/Site.h"
#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"
#include <string>

namespace synthese
{
	using namespace env;
	using namespace util;
	using std::string;

	namespace interfaces
	{
		const string& LogicalStopNameValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const Site* site) const
		{
			if (_uid == NULL || Conversion::ToLongLong(_uid->getValue(parameters)) == 0 )
			{
				const ConnectionPlace* place = (ConnectionPlace*) object;
				return place->getName();
			}
			else
			{
				const Environment* __Environnement = site->getEnvironment();
				uid placeUID = Conversion::ToLongLong(_uid->getValue(parameters));

				const ConnectionPlace* place = __Environnement->getConnectionPlaces().get(placeUID);
				return place->getName();
			}
		}

		void LogicalStopNameValueInterfaceElement::parse( const std::string& text )
		{
			ValueElementList vel(text);
			_uid = vel.front();
		}
	}

}