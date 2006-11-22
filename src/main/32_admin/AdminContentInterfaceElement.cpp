
#include "11_interfaces/ValueElementList.h"

#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminContentInterfaceElement.h"

namespace synthese
{
	using namespace interfaces;

	namespace admin
	{
		void AdminContentInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}

		void AdminContentInterfaceElement::display(std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			const AdminInterfaceElement* aie = (const AdminInterfaceElement*) rootObject;
			aie->display(stream, parameters, NULL, request);
		}

		AdminContentInterfaceElement::~AdminContentInterfaceElement()
		{
		}
	}
}
