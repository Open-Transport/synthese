
#include "ParameterValueInterfaceElement.h"
#include "01_util/Conversion.h"

namespace synthese
{
	using namespace std;
	using namespace util;

	namespace interfaces
	{
		const std::string ParameterValueInterfaceElement::_value_factory_key = Factory<ValueInterfaceElement>::integrate<ParameterValueInterfaceElement>("param");
		const std::string ParameterValueInterfaceElement::_library_factory_key = Factory<LibraryInterfaceElement>::integrate<ParameterValueInterfaceElement>("param");

		const string& ParameterValueInterfaceElement::getValue( const ParametersVector& parameters, const void* object, const Site* site ) const
		{
			return parameters[_rank] ;
		}

		void ParameterValueInterfaceElement::parse( const std::string& text )
		{
			_rank = Conversion::ToInt(text);
		}

	}
}
