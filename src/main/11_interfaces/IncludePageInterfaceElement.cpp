
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/IncludePageInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void IncludePageInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object, const server::Request* request) const
		{
			const InterfacePage* page_to_include = Factory<InterfacePage>::create(_page_code->getValue(parameters));
			page_to_include->display(stream, _parameters.fillParameters( parameters ), object, request);
		}

		void IncludePageInterfaceElement::storeParameters(ValueElementList& vel )
		{
			if (vel.isEmpty())
				throw InterfacePageException("Included page not specified");

			_page_code = vel.front();
			vel = _parameters;
		}
	}
}

