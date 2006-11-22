
#include "VinciCustomerSearchAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace vinci
	{
		VinciCustomerSearchAdminInterfaceElement::VinciCustomerSearchAdminInterfaceElement()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		string VinciCustomerSearchAdminInterfaceElement::getTitle() const
		{
			return "Clients";
		}

		void VinciCustomerSearchAdminInterfaceElement::display(ostream& stream, const ParametersVector& parameters, const void* rootObject, const Request* request) const
		{
			stream 
				<< "Recherche de client :<br />"
				<< "Nom : <input name=\"searchname\" /><br />";
				
		}
	}
}
