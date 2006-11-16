
#include "LineLabelInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		const std::string LineLabelInterfaceElement::_factory_key = Factory<LibraryInterfaceElement>::integrate<LineLabelInterfaceElement>("label");

		void LineLabelInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const Site* site /*= NULL*/ ) const
		{
		}

		void LineLabelInterfaceElement::parse( const std::string& text )
		{
		}

		const std::string& synthese::interfaces::LineLabelInterfaceElement::getLabel() const
		{
			return _label;
		}
	}
}
