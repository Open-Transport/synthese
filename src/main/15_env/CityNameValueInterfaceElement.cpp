
#include "CityNameValueInterfaceElement.h"
#include "15_env/City.h"
#include "15_env/Environment.h"
#include "11_interfaces/Site.h"
#include "11_interfaces/Interface.h"
#include "01_util/UId.h"
#include "01_util/Conversion.h"
#include "11_interfaces/ValueElementList.h"
#include <string>

namespace synthese
{
	using namespace env;
	using namespace util;
	using std::string;

	namespace interfaces
	{
		const std::string CityNameValueInterfaceElement::_value_factory_key = Factory<ValueInterfaceElement>::integrate<CityNameValueInterfaceElement>("city_name");
		const std::string CityNameValueInterfaceElement::_library_factory_key = Factory<LibraryInterfaceElement>::integrate<CityNameValueInterfaceElement>("city_name");


		const string& CityNameValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const Site* site) const
		{
			if (_uid == NULL || Conversion::ToLongLong(_uid->getValue(parameters)) == 0 )
			{
				const City* city = (City*) object;
				return city->getName();
			}
			else
			{
				const Environment* __Environnement = site->getEnvironment();
				uid cityUID = Conversion::ToLongLong(_uid->getValue(parameters));

				const City* curCommune = __Environnement->getCities().get(cityUID);
				return curCommune->getName();
			}
		}

		void CityNameValueInterfaceElement::parse( const std::string& text )
		{
			ValueElementList vel(text);
			_uid = vel.front();
		}
	}

}