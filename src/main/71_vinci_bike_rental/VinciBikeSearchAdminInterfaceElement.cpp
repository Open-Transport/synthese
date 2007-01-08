
#include <vector>

#include "30_server/ServerModule.h"

#include "32_admin/AdminRequest.h"

#include "71_vinci_bike_rental/VinciAddBike.h"
#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciBikeAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciBikeSearchAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace vinci
	{
		VinciBikeSearchAdminInterfaceElement::VinciBikeSearchAdminInterfaceElement()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		string VinciBikeSearchAdminInterfaceElement::getTitle() const
		{
			return "Parc de vélos";
		}

		void VinciBikeSearchAdminInterfaceElement::display(ostream& stream, const ParametersVector& parameters, const void* rootObject, const Request* request) const
		{
			// Current request
			AdminRequest* currentRequest = (AdminRequest*) request;

			// AddStatus
			AdminRequest* addStatusRequest = Factory<Request>::create<AdminRequest>();
			addStatusRequest->copy(request);
			addStatusRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeSearchAdminInterfaceElement>());

			// AddBike
			AdminRequest* addBikeRequest = Factory<Request>::create<AdminRequest>();
			addBikeRequest->copy(request);
			addBikeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeAdminInterfaceElement>());
			addBikeRequest->setAction(Factory<Action>::create<VinciAddBike>());

			// Open a bike
			AdminRequest* viewBikeRequest = Factory<Request>::create<AdminRequest>();
			viewBikeRequest->copy(request);
			viewBikeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeAdminInterfaceElement>());

			stream
				<< addStatusRequest->getHTMLFormHeader("search")
				<< "<h1>Recherche de vélo</h1>"
				<< "Numéro : <input name=\"searchnumber\" /> "
				<< "Cadre : <input name=\"searchcadre\" /> "
				<< "<input type=\"submit\" value=\"Rechercher\" />"
				<< "</form>"
				<< "<h1>Résultat de la recherche</h1>"
				;

			vector<VinciBike*> bikes = VinciBikeTableSync::search(currentRequest->getStringParameter("searchnumber", ""), currentRequest->getStringParameter("searchcadre", ""));
			stream
				<< addBikeRequest->getHTMLFormHeader("create")
				<< "<table><tr><th>Numéro</th><th>Cadre</th></tr>"
				;
			for (vector<VinciBike*>::iterator it = bikes.begin(); it != bikes.end(); ++it)
			{
				viewBikeRequest->setObjectId((*it)->getKey());
				stream
					<< "<tr>"
					<< "<td>" << viewBikeRequest->getHTMLLink((*it)->getNumber()) << "</td>"
					<< "<td>" << viewBikeRequest->getHTMLLink((*it)->getMarkedNumber()) << "</td>"
					<< "</tr>";
				delete *it;
			}
			if (currentRequest->getStringParameter("searchnumber", "") != "")
			{
				stream
					<< "<tr>"
					<< "<td><input name=\"" << VinciAddBike::PARAMETER_NUMBER << "\" value=\"" << currentRequest->getStringParameter("searchnumber", "") << "\" /></td>"
					<< "<td><input name=\"" << VinciAddBike::PARAMETER_MARKED_NUMBER << "\" value=\"" << currentRequest->getStringParameter("searchcadre", "") << "\" /></td>"
					<< "<td><input type=\"submit\" value=\"Créer\" /></td>"
					<< "</tr>"
					;
			}
			stream << "</table></form>";

			delete addStatusRequest;
		}
	}
}

