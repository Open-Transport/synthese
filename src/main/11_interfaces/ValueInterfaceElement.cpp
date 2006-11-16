
#include "ValueInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void ValueInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object, const Site* site ) const
		{
			stream << getValue( parameters, site );
		}
	}
}
