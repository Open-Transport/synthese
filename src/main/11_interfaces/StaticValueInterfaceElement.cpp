
#include "StaticValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		const std::string& StaticValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const server::Request* request) const
		{
			return _value;
		}

		void StaticValueInterfaceElement::parse( const std::string& text )
		{
			_value = text;
		}

	}
}
