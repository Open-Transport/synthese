
#include "32_admin/AdminRequest.h"

#include "71_vinci_bike_rental/VinciCustomerSearchAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

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
			AdminRequest* searchRequest = (AdminRequest*) Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			AdminRequest* currentRequest = (AdminRequest*) request;

			stream
				<< searchRequest->getHTMLFormHeader("search")
				<< "<h1>Recherche de client<h1>"
				<< "Nom : <input name=\"searchname\" /> Prénom : <input name=\"searchsurname\" />";

			stream
				<< "<h1>Résultat de la recherche</h1>";


			

			stream
				<< "<h1>Nouveau contrat</h1>";

				
		}
	}
}
