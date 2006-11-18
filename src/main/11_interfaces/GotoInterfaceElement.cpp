
#include "GotoInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void GotoInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const Site* site /*= NULL*/ ) const
		{
		}

		void GotoInterfaceElement::parse( const std::string& text )
		{
			_label = text;
		}

		const std::string& GotoInterfaceElement::getLabel() const
		{
			return _label;
		}
	}
}
