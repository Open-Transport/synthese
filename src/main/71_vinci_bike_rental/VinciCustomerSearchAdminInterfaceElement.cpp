
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

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/AddCustomerAction.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciCustomerSearchAdminInterfaceElement.h"

#include "05_html/SearchFormHTMLTable.h"
#include "05_html/ActionResultHTMLTable.h"

#include "12_security/User.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/HomeAdmin.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace vinci;

	namespace util
	{
	template <> const string FactorableTemplate<AdminInterfaceElement,VinciCustomerSearchAdminInterfaceElement>::FACTORY_KEY("vincicustomers");
	}

	namespace admin
	{
	template <> const string AdminInterfaceElementTemplate<VinciCustomerSearchAdminInterfaceElement>::ICON("user_gray.png");
	template <>const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<VinciCustomerSearchAdminInterfaceElement>::DISPLAY_MODE(AdminInterfaceElement::EVER_DISPLAYED);
	template <> string AdminInterfaceElementTemplate<VinciCustomerSearchAdminInterfaceElement>::getSuperior()
		{
			return HomeAdmin::FACTORY_KEY;
		}
	}

	namespace vinci
	{
		const string VinciCustomerSearchAdminInterfaceElement::PARAM_SEARCH_NAME = "searchname";
		const string VinciCustomerSearchAdminInterfaceElement::PARAM_SEARCH_SURNAME = "searchsurname";

		VinciCustomerSearchAdminInterfaceElement::VinciCustomerSearchAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciCustomerSearchAdminInterfaceElement>() {}

		string VinciCustomerSearchAdminInterfaceElement::getTitle() const
		{
			return "Clients";
		}

		void VinciCustomerSearchAdminInterfaceElement::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Search request
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<VinciCustomerSearchAdminInterfaceElement>();

			// Add contract request
			ActionFunctionRequest<AddCustomerAction,AdminRequest> addContractRequest(request);
			addContractRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();

			// View contract request
			FunctionRequest<AdminRequest> contractRequest(request);
			contractRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();

			stream << "<h1>Recherche de client</h1>";
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
			stream << st.cell("Prénom", st.getForm().getTextInput(PARAM_SEARCH_SURNAME, _searchSurname));
			stream << st.close();

			stream << "<h1>Résultat de la recherche</h1>";

			ActionResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "ID"));
			h.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(PARAM_SEARCH_SURNAME, "Prénom"));
			ActionResultHTMLTable t(h, st.getForm(), _requestParameters, _resultParameters, addContractRequest.getHTMLForm("add"));
			stream << t.open();
			if (_contracts.size() == 0)
			{
				stream << t.row();
				stream << t.col(3) << "Aucun contrat trouvé";
			}
			else
			{
				for (vector<shared_ptr<VinciContract> >::const_iterator it = _contracts.begin(); it != _contracts.end(); ++it)
				{
					contractRequest.setObjectId((*it)->getKey());
					stream << t.row();
					stream << t.col() << HTMLModule::getHTMLLink(contractRequest.getURL(), Conversion::ToString((*it)->getKey()));
					stream << t.col() << HTMLModule::getHTMLLink(contractRequest.getURL(), (*it)->getUser()->getName());
					stream << t.col() << HTMLModule::getHTMLLink(contractRequest.getURL(), (*it)->getUser()->getSurname());
				}
			}
			if (_activeSearch)
			{
				stream << t.row();
				stream << t.col() << t.getActionForm().getSubmitButton("Nouveau");
				stream << t.col() << t.getActionForm().getTextInput(AddCustomerAction::PARAMETER_NAME, _searchName);
				stream << t.col() << t.getActionForm().getTextInput(AddCustomerAction::PARAMETER_SURNAME, _searchSurname);
			}
			stream << t.close();
		}

		void VinciCustomerSearchAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			_searchSurname = map.getString(PARAM_SEARCH_SURNAME, false, FACTORY_KEY);

			_searchName = map.getString(PARAM_SEARCH_NAME, false, FACTORY_KEY);

			_requestParameters = ActionResultHTMLTable::getParameters(map.getMap(), PARAM_SEARCH_NAME, 30);

			_activeSearch = !_searchName.empty() || !_searchSurname.empty();

			_contracts = VinciContractTableSync::search(
				_searchName
				, _searchSurname
				, _requestParameters.first
				, _requestParameters.maxSize
				, _requestParameters.orderField == PARAM_SEARCH_NAME
				, _requestParameters.orderField == PARAM_SEARCH_SURNAME
				, _requestParameters.raisingOrder
			);

			_resultParameters = ActionResultHTMLTable::getParameters(_requestParameters, _contracts);
		}

		bool VinciCustomerSearchAdminInterfaceElement::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}

		server::ParametersMap VinciCustomerSearchAdminInterfaceElement::getParametersMap() const
		{
			ParametersMap map(ActionResultHTMLTable::getParametersMap(_requestParameters));
			map.insert(PARAM_SEARCH_NAME, _searchName);
			map.insert(PARAM_SEARCH_SURNAME, _searchSurname);
			return map;
		}

		void VinciCustomerSearchAdminInterfaceElement::setSearchName( const std::string& name )
		{
			_searchName = name;
		}
	}
}

