
#include "30_server/ServerModule.h"

#include "32_admin/AdminRequest.h"

#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciBikeAdminInterfaceElement.h"

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;

	namespace vinci
	{
		VinciBikeAdminInterfaceElement::VinciBikeAdminInterfaceElement()
			: AdminInterfaceElement("vincibikes", AdminInterfaceElement::DISPLAYED_IF_CURRENT) {}


		std::string VinciBikeAdminInterfaceElement::getTitle() const
		{
			return "Vélo";
		}

		void VinciBikeAdminInterfaceElement::display( std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			// Current request
			AdminRequest* currentRequest = (AdminRequest*) request;

			// Update bike request
			AdminRequest* updateBikeRequest = Factory<Request>::create<AdminRequest>();
			updateBikeRequest->copy(request);
			updateBikeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeAdminInterfaceElement>());

			// Display of data board
			VinciBike* bike = VinciBikeTableSync::get(ServerModule::getSQLiteThread(), request->getObjectId());
			stream
				<< "<h1>Données</h1>"
				<< updateBikeRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><td>Numéro :</td><td><input value=\"" << bike->getNumber() << "\" /></td></tr>"
				<< "<tr><td>Cadre :</td><td><input value=\"" << bike->getMarkedNumber() << "\" /></td></tr>"
				<< "</table></form>"
				;

			// Display of history
			stream
				<< "<h1>Historique</h1>"
				;

			// Cleaning
			delete updateBikeRequest;
		}
	}
}
