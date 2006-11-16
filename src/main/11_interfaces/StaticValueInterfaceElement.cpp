
#include "StaticValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		const std::string StaticValueInterfaceElement::_value_factory_key = Factory<ValueInterfaceElement>::integrate<StaticValueInterfaceElement>("text");
		const std::string StaticValueInterfaceElement::_library_factory_key = Factory<LibraryInterfaceElement>::integrate<StaticValueInterfaceElement>("text");

		const std::string& StaticValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const Site* site ) const
		{
			return _value;
		}

		void StaticValueInterfaceElement::parse( const std::string& text )
		{
			_value = text;
		}

	}
}
