
#include <string>

#include "01_util/UId.h"
#include "01_util/Conversion.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"

#include "15_env/City.h"
#include "15_env/Environment.h"
#include "15_env/CityNameValueInterfaceElement.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using std::string;

	namespace env
	{
		string CityNameValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const server::Request* request) const
		{
/*			if (_uid == NULL || Conversion::ToLongLong(_uid->getValue(parameters)) == 0 )
			{
*/				const City* city = (City*) object;
				return city->getName();
/*			}
			else
			{
				const Environment* __Environnement = request->getSite()->getEnvironment();
				uid cityUID = Conversion::ToLongLong(_uid->getValue(parameters));

				const City* curCommune = __Environnement->getCities().get(cityUID);
				return curCommune->getName();
			}
*/		}

		void CityNameValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_uid = (vel.size() > 0) ? vel.front() : NULL;
		}
	}

}

