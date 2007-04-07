
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

#include "05_html/SearchFormHTMLTable.h"
#include "05_html/ActionResultHTMLTable.h"

#include "12_security/User.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminRequest.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/AddCustomerAction.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciCustomerSearchAdminInterfaceElement.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;

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

		void VinciCustomerSearchAdminInterfaceElement::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Search request
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerSearchAdminInterfaceElement>());

			// Add contract request
			ActionFunctionRequest<AddCustomerAction,AdminRequest> addContractRequest(request);
			addContractRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());

			// View contract request
			FunctionRequest<AdminRequest> contractRequest(request);
			contractRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<VinciCustomerAdminInterfaceElement>());

			stream << "<h1>Recherche de client</h1>";
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
			stream << st.cell("Prénom", st.getForm().getTextInput(PARAM_SEARCH_SURNAME, _searchSurname));
			stream << st.close();

			if (_activeSearch)
			{
				stream << "<h1>Résultat de la recherche</h1>";

				ActionResultHTMLTable::HeaderVector h;
				h.push_back(make_pair(string(), "ID"));
				h.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
				h.push_back(make_pair(PARAM_SEARCH_SURNAME, "Prénom"));
				ActionResultHTMLTable t(h, st.getForm(), string(), true, addContractRequest.getHTMLForm("add"));
				stream << t.open();
				if (_contracts.size() == 0)
				{
					stream << t.row();
					stream << t.col(3) << "Aucun contrat trouvé";
				}
				else
				{
					for (vector<VinciContract*>::const_iterator it = _contracts.begin(); it != _contracts.end(); ++it)
					{
						contractRequest.setObjectId((*it)->getKey());
						stream << t.row();
						stream << t.col() << HTMLModule::getHTMLLink(contractRequest.getURL(), Conversion::ToString((*it)->getKey()));
						stream << t.col() << HTMLModule::getHTMLLink(contractRequest.getURL(), (*it)->getUser()->getName());
						stream << t.col() << HTMLModule::getHTMLLink(contractRequest.getURL(), (*it)->getUser()->getSurname());
					}
				}
				stream << t.row();
				stream << t.col() << t.getActionForm().getSubmitButton("Nouveau");
				stream << t.col() << t.getActionForm().getTextInput(AddCustomerAction::PARAMETER_NAME, _searchName);
				stream << t.col() << t.getActionForm().getTextInput(AddCustomerAction::PARAMETER_SURNAME, _searchSurname);
				stream << t.close();
			}
		}

		void VinciCustomerSearchAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it = map.find(PARAM_SEARCH_SURNAME);
			if (it != map.end())
				_searchSurname = it->second;

			it = map.find(PARAM_SEARCH_NAME);
			if (it != map.end())
				_searchName = it->second;

			_activeSearch = (_searchSurname != "" || _searchName != "");

			if (_activeSearch)
				_contracts = VinciContractTableSync::search(_searchName, _searchSurname);
		}

		VinciCustomerSearchAdminInterfaceElement::~VinciCustomerSearchAdminInterfaceElement()
		{
			for (vector<VinciContract*>::iterator it = _contracts.begin(); it != _contracts.end(); ++it)
				delete *it;
		}
	}
}

