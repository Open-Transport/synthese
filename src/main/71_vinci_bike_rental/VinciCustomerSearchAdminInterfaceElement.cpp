
#include <vector>

#include "12_security/User.h"

#include "30_server/ServerModule.h"

#include "32_admin/AdminRequest.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/AddCustomerAction.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"
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
			// Current request
			AdminRequest* currentRequest = (AdminRequest*) request;

			// Search request
			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerSearchAdminInterfaceElement>());

			// Add contract request
			AdminRequest* addContractRequest = Factory<Request>::create<AdminRequest>();
			addContractRequest->copy(request);
			addContractRequest->setAction(Factory<Action>::create<AddCustomerAction>());
			addContractRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());

			// View contract request
			AdminRequest* contractRequest = Factory<Request>::create<AdminRequest>();
			contractRequest->copy(request);
			contractRequest->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());

			stream
				<< searchRequest->getHTMLFormHeader("search")
				<< "<h1>Recherche de client</h1>"
				<< "Nom : <input name=\"" << PARAM_SEARCH_NAME << "\" value=\"" << currentRequest->getStringParameter(PARAM_SEARCH_NAME, "") << "\" />"
				<< "Prénom : <input name=\"" << PARAM_SEARCH_SURNAME << "\" value=\"" << currentRequest->getStringParameter(PARAM_SEARCH_SURNAME, "") << "\" />"
				<< "<input type=\"submit\" value=\"Rechercher\" />"
				<< "</form>"
				;

			if (currentRequest->getStringParameter(PARAM_SEARCH_NAME, "") != "" || currentRequest->getStringParameter(PARAM_SEARCH_SURNAME, "") != "")
			{
				vector<VinciContract*> contracts = VinciContractTableSync::searchVinciContracts(
					ServerModule::getSQLiteThread(),
					currentRequest->getStringParameter(PARAM_SEARCH_NAME, ""), currentRequest->getStringParameter(PARAM_SEARCH_SURNAME, "")
					);

				stream
					<< "<h1>Résultat de la recherche</h1>"
					<< addContractRequest->getHTMLFormHeader("add")
					<< "<table><tr><th>id</th><th>Nom</th><th>Prénom</th></tr>"
					;
				if (contracts.size() == 0)
					stream << "<tr><td colspan=\"2\">Aucun contrat trouvé</td></tr>";
				else
				{
					for (vector<VinciContract*>::const_iterator it = contracts.begin(); it !=contracts.end(); ++it)
					{
						contractRequest->setObjectId((*it)->getKey());

						stream
							<< "<tr>"
							<< "<td>" << contractRequest->getHTMLLink(Conversion::ToString((*it)->getKey())) << "</td>"
							<< "<td>" << contractRequest->getHTMLLink((*it)->getUser()->getName()) << "</td>"
							<< "<td>" << contractRequest->getHTMLLink((*it)->getUser()->getSurname()) << "</td>"
							<< "</tr>";
						delete *it;
					}
				}
				stream << "<tr>"
					<< "<td><input type=\"submit\" value=\"Nouveau\" /></td>"
					<< "<td><input name=\"" << AddCustomerAction::PARAMETER_NAME << "\" value=\"" << currentRequest->getStringParameter(PARAM_SEARCH_NAME, "") << "\" /></td>"
					<< "<td><input name=\"" << AddCustomerAction::PARAMETER_SURNAME << "\" value=\"" << currentRequest->getStringParameter(PARAM_SEARCH_SURNAME, "") << "\" /></td>"
					<< "</tr>"
					<< "</table></form>"
					;
			}
		}
	}
}

