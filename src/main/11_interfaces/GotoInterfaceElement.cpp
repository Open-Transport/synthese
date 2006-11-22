
#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/GotoInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void GotoInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
		}

		const std::string GotoInterfaceElement::getLabel(const ParametersVector& parameters) const
		{
			return _label->getValue(parameters);
		}

		void GotoInterfaceElement::storeParameters( ValueElementList& vel )
		{
			_label = vel.front();
		}
	}
}
