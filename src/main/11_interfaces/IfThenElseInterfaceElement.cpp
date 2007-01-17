
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/StaticValueInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/IfThenElseInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		std::string IfThenElseInterfaceElement::getValue(const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			std::string result = _criteria->getValue(parameters, object, request);
			return ( result.size() == 0 || result == "0" )
				? _to_return_if_false->getValue(parameters, object, request)
				: _to_return_if_true->getValue(parameters, object, request);
		}
		
		IfThenElseInterfaceElement::~IfThenElseInterfaceElement()
		{
			delete _criteria;
			delete _to_return_if_false;
			delete _to_return_if_true;
		}

		void IfThenElseInterfaceElement::storeParameters(ValueElementList& vel )
		{
			if (vel.size() < 2)
				throw InterfacePageException("Conditional element without value to return");

			_criteria = vel.front();
			_to_return_if_true = vel.front();
			_to_return_if_false = vel.isEmpty() ? new StaticValueInterfaceElement("") : vel.front();
		}
	}
}

