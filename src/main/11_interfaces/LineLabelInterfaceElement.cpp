
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/StaticValueInterfaceElement.h"
#include "11_interfaces/LineLabelInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void LineLabelInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
		}

		void LineLabelInterfaceElement::storeParameters( ValueElementList& vel )
		{
			if (vel.size() != 1)
				throw InterfacePageException("Malformed line label declaration");
			ValueInterfaceElement* vie = vel.front();
            if (dynamic_cast<StaticValueInterfaceElement*>(vie) == NULL)
				throw InterfacePageException("Line label must be statically defined");
			ParametersVector pv;
			_label = vie->getValue(pv);
		}
		const std::string& synthese::interfaces::LineLabelInterfaceElement::getLabel() const
		{
			return _label;
		}
	}
}

