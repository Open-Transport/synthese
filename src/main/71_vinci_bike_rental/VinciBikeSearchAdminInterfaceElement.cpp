
#include "VinciBikeSearchAdminInterfaceElement.h"

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
			stream 
				<< "Recherche de vélo :<br />"
				<< "Numéro : <input name=\"searchnumber\" /><br />";

		}
	}
}
