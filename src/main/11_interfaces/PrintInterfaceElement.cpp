
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/PrintInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		void PrintInterfaceElement::display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stream << _toBePrinted->getValue(parameters, rootObject, request);
		}

		PrintInterfaceElement::~PrintInterfaceElement()
		{
			delete _toBePrinted;
		}

		void PrintInterfaceElement::storeParameters(ValueElementList& vel )
		{
			if (vel.size() != 1)
				throw InterfacePageException("Malformed print command");
			_toBePrinted = vel.front();
		}
	}
}
