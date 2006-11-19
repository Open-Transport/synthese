
#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void ValueInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object, const server::Request* request) const
		{
			stream << getValue( parameters, request);
		}
	}
}
