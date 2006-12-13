
#include "01_util/Conversion.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/ParameterValueInterfaceElement.h"

namespace synthese
{
	using namespace std;
	using namespace util;

	namespace interfaces
	{
		string ParameterValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const server::Request* request) const
		{
			return parameters[Conversion::ToInt(_rank->getValue(parameters))] ;
		}

		void ParameterValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() != 1)
				throw InterfacePageException("Malformed parameter interface element");
			_rank = vel.front();
		}

	}
}

