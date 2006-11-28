
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
			return "Parc de v�los";
		}

		void VinciBikeSearchAdminInterfaceElement::display(ostream& stream, const ParametersVector& parameters, const void* rootObject, const Request* request) const
		{
			stream 
				<< "Recherche de v�lo :<br />"
				<< "Num�ro : <input name=\"searchnumber\" /><br />";

		}
	}
}
