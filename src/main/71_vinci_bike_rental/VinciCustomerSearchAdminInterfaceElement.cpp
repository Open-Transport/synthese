
/** VinciCustomerSearchAdminInterfaceElement class implementation.
	@file VinciCustomerSearchAdminInterfaceElement.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "01_util/Html.h"

#include "12_security/User.h"

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

		void VinciCustomerSearchAdminInterfaceElement::display(ostream& stream, const AdminRequest* request) const
		{
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
				<< "Nom : " << Html::getTextInput(PARAM_SEARCH_NAME, _searchName)
				<< "Prénom : " << Html::getTextInput(PARAM_SEARCH_SURNAME, _searchSurname)
				<< Html::getSubmitButton("Rechercher")
				<< "</form>"
				;

			if (_activeSearch)
			{
				stream
					<< "<h1>Résultat de la recherche</h1>"
					<< addContractRequest->getHTMLFormHeader("add")
					<< "<table><tr><th>id</th><th>Nom</th><th>Prénom</th></tr>"
					;
				if (_contracts.size() == 0)
					stream << "<tr><td colspan=\"2\">Aucun contrat trouvé</td></tr>";
				else
				{
					for (vector<VinciContract*>::const_iterator it = _contracts.begin(); it != _contracts.end(); ++it)
					{
						contractRequest->setObjectId((*it)->getKey());
						stream
							<< "<tr>"
							<< "<td>" << contractRequest->getHTMLLink(Conversion::ToString((*it)->getKey())) << "</td>"
							<< "<td>" << contractRequest->getHTMLLink((*it)->getUser()->getName()) << "</td>"
							<< "<td>" << contractRequest->getHTMLLink((*it)->getUser()->getSurname()) << "</td>"
							<< "</tr>";
					}
				}
				stream << "<tr>"
					<< "<td>" << Html::getSubmitButton("Nouveau") << "</td>"
					<< "<td>" << Html::getTextInput(AddCustomerAction::PARAMETER_NAME, _searchName) << "</td>"
					<< "<td>" << Html::getTextInput(AddCustomerAction::PARAMETER_SURNAME, _searchSurname) << "</td>"
					<< "</tr>"
					<< "</table></form>"
					;
			}
		}

		void VinciCustomerSearchAdminInterfaceElement::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			server::Request::ParametersMap::const_iterator it = map.find(PARAM_SEARCH_SURNAME);
			if (it != map.end())
				_searchSurname = it->second;

			it = map.find(PARAM_SEARCH_NAME);
			if (it != map.end())
				_searchName = it->second;

			_contracts = VinciContractTableSync::search(_searchName, _searchSurname);
			_activeSearch = (_searchSurname != "" || _searchName != "");
		}

		VinciCustomerSearchAdminInterfaceElement::~VinciCustomerSearchAdminInterfaceElement()
		{
			for (vector<VinciContract*>::iterator it = _contracts.begin(); it != _contracts.end(); ++it)
				delete *it;
		}
	}
}

