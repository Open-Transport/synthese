
#include "LineLabelInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void LineLabelInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
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
