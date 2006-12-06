
#include <vector>

#include "12_security/User.h"

#include "30_server/ServerModule.h"

#include "32_admin/AdminRequest.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
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
		const string VinciCustomerSearchAdminInterfaceElement::PARAM_SEARCH_NAME = "searchname";
		const string VinciCustomerSearchAdminInterfaceElement::PARAM_SEARCH_SURNAME = "searchsurname";

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
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerSearchAdminInterfaceElement>());

			AdminRequest* currentRequest = (AdminRequest*) request;

			stream
				<< searchRequest->getHTMLFormHeader("search")
				<< "<h1>Recherche de client</h1>"
				<< "Nom : <input name=\"" << PARAM_SEARCH_NAME << "\" /> Prénom : <input name=\"" << PARAM_SEARCH_SURNAME << "\" />";

			if (currentRequest->getStringParameter(PARAM_SEARCH_NAME, "") != "" || currentRequest->getStringParameter(PARAM_SEARCH_SURNAME, "") != "")
			{
				vector<VinciContract*> contracts = VinciContractTableSync::searchVinciContracts(
					ServerModule::getSQLiteThread(),
					currentRequest->getStringParameter(PARAM_SEARCH_NAME, ""), currentRequest->getStringParameter(PARAM_SEARCH_SURNAME, "")
					,0, 0);

				stream
					<< "<h1>Résultat de la recherche</h1>";

				if (contracts.size() == 0)
					stream << "Aucun contrat trouvé";
				else
				{
					stream << "<table>";
					for (vector<VinciContract*>::const_iterator it = contracts.begin(); it !=contracts.end(); ++it)
					{
						stream
							<< "<tr>"
							<< "<td>" << (*it)->getUser()->getName() << "</td>"
							<< "<td>" << (*it)->getUser()->getSurname() << "</td>"
							<< "</tr>";
						delete *it;
					}
					stream << "</table>";
				}
			}
			

			stream
				<< "<h1>Nouveau contrat</h1>";

				
		}
	}
}
