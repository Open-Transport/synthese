
#include "VinciReportsAdminInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;

	namespace vinci
	{
		VinciReportsAdminInterfaceElement::VinciReportsAdminInterfaceElement()
			: AdminInterfaceElement("home", AdminInterfaceElement::EVER_DISPLAYED) {}

		string VinciReportsAdminInterfaceElement::getTitle() const
		{
			return "Etats journaliers";
		}

		void VinciReportsAdminInterfaceElement::display(ostream& stream, const ParametersVector& parameters, const void* rootObject, const Request* request) const
		{
			stream
				<< "<table>"
				<< "<tr><td>Date début</td><td><input /></td></tr>"
				<< "<tr><td>Date fin</td><td><input /></td></tr>"
				<< "<tr><td>Nombre de locations</td><td></td></tr>"
				<< "<tr><td>Nombre de validations</td><td></td></tr>"
				<< "<tr><td>Encaissements effectués</td><td></td></tr>"
				<< "<tr><td>tri par tarif</td>"
				;
		}
	}
}
