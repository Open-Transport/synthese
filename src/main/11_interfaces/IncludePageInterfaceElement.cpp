
#include "IncludePageInterfaceElement.h"
#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		const std::string IncludePageInterfaceElement::_factory_key = Factory<LibraryInterfaceElement>::integrate<IncludePageInterfaceElement>(">");

		void IncludePageInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object, const Site* site ) const
		{
			const InterfacePage* page_to_include = Factory<InterfacePage>::create(_page_code);
			page_to_include->display(stream, _parameters.fillParameters( parameters ), object, site );
		}


		void IncludePageInterfaceElement::parse( const std::string& text )
		{
			ValueElementList vai( text );
			ValueInterfaceElement* page_code_element = vai.front();
			_page_code = page_code_element->getValue( ParametersVector() );
			_parameters = vai;
			delete page_code_element;
		}
	}
}
