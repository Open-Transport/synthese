
#include "StaticValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		std::string StaticValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const server::Request* request) const
		{
			return _value;
		}

		void StaticValueInterfaceElement::storeParameters(ValueElementList& vel )
		{
		}

		StaticValueInterfaceElement::StaticValueInterfaceElement( const std::string& value )
			: _value(value)
		{
			
		}
	}
}

