
/** VinciCustomerSearchAdminInterfaceElement class implementation.
	@file VinciCustomerSearchAdminInterfaceElement.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Rapha�l Murat - Vinci Park <rmurat@vincipark.com>

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
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

#include "05_html/SearchFormHTMLTable.h"
#include "05_html/ActionResultHTMLTable.h"

#include "12_security/User.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/ModuleAdmin.h"

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
	template <> const string FactorableTemplate<AdminInterfaceElement,VinciCustomerSearchAdminInterfaceElement>::FACTORY_KEY("1vincicustomers");
	}

	namespace admin
	{
		template <> const string AdminInterfaceElementTemplate<VinciCustomerSearchAdminInterfaceElement>::ICON("vcard.png");
		template <> const string AdminInterfaceElementTemplate<VinciCustomerSearchAdminInterfaceElement>::DEFAULT_TITLE("Clients");
	}

	namespace vinci
	{
		const string VinciCustomerSearchAdminInterfaceElement::PARAM_SEARCH_NAME = "sn";
		const string VinciCustomerSearchAdminInterfaceElement::PARAM_SEARCH_SURNAME = "ss";
		const string VinciCustomerSearchAdminInterfaceElement::PARAM_SEARCH_FILTER = "sf";

		VinciCustomerSearchAdminInterfaceElement::VinciCustomerSearchAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciCustomerSearchAdminInterfaceElement>() {}



		void VinciCustomerSearchAdminInterfaceElement::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// Search
			vector<shared_ptr<VinciContract> >	_contracts(VinciContractTableSync::search(
				_searchName
				, _searchSurname
				, UNKNOWN_VALUE
				, _searchFilter == FILTER_ONLATE
				, _searchFilter == FILTER_DUE
				, _searchFilter == FILTER_OUTDATED_GUARANTEE
				, _searchFilter == FILTER_CONTRACTED_GUARANTEE
				, _requestParameters.first
				, _requestParameters.maxSize
				, _requestParameters.orderField == PARAM_SEARCH_NAME
				, _requestParameters.orderField == PARAM_SEARCH_SURNAME
				, _requestParameters.orderField == PARAM_SEARCH_FILTER
				, _requestParameters.raisingOrder
			));
			ResultHTMLTable::ResultParameters	_resultParameters;
			_resultParameters.setFromResult(_requestParameters, _contracts);



			// Search request
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<VinciCustomerSearchAdminInterfaceElement>();

			// Add contract request
			ActionFunctionRequest<AddCustomerAction,AdminRequest> addContractRequest(request);
			addContractRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();

			// View contract request
			FunctionRequest<AdminRequest> contractRequest(request);
			contractRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();

			// Filter list
			

			stream << "<h1>Recherche de client</h1>";
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
			stream << st.cell("Pr�nom", st.getForm().getTextInput(PARAM_SEARCH_SURNAME, _searchSurname));
			stream << st.cell("Filtre", st.getForm().getSelectInput(PARAM_SEARCH_FILTER, _GetFilterNames(), _searchFilter));
			stream << st.close();

			stream << "<h1>R�sultat de la recherche</h1>";

			ActionResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "ID"));
			h.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(PARAM_SEARCH_SURNAME, "Pr�nom"));
			h.push_back(make_pair(string(), "Retard"));
			h.push_back(make_pair(string(), "Solde"));
			h.push_back(make_pair(string(), "Caution retard"));
			ActionResultHTMLTable t(h, st.getForm(), _requestParameters, _resultParameters, addContractRequest.getHTMLForm("add"));
			stream << t.open();
			if (_contracts.size() == 0)
			{
				stream << t.row();
				stream << t.col(3) << "Aucun contrat trouv�";
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
					stream << t.col() << (*it)->getLate().toString();
					stream << t.col() << (*it)->getDue();
					stream << t.col() << (*it)->getOutDatedGuarantee().toString();
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

			_searchFilter = static_cast<_Filter>(map.getInt(PARAM_SEARCH_FILTER, false, FACTORY_KEY));

			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_NAME, 30);

			_activeSearch = !_searchName.empty() || !_searchSurname.empty();
		}

		bool VinciCustomerSearchAdminInterfaceElement::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}

		server::ParametersMap VinciCustomerSearchAdminInterfaceElement::getParametersMap() const
		{
			ParametersMap map(_requestParameters.getParametersMap());
			map.insert(PARAM_SEARCH_NAME, _searchName);
			map.insert(PARAM_SEARCH_SURNAME, _searchSurname);
			return map;
		}

		void VinciCustomerSearchAdminInterfaceElement::setSearchName( const std::string& name )
		{
			_searchName = name;
		}

		AdminInterfaceElement::PageLinks VinciCustomerSearchAdminInterfaceElement::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage 		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == VinciBikeRentalModule::FACTORY_KEY)
			{
				links.push_back(getPageLink());
			}
			return links;
		}

		std::vector<std::pair<VinciCustomerSearchAdminInterfaceElement::_Filter, std::string> > VinciCustomerSearchAdminInterfaceElement::_GetFilterNames()
		{
			std::vector<std::pair<VinciCustomerSearchAdminInterfaceElement::_Filter, std::string> > filters;
			filters.push_back(make_pair(FILTER_NONE, "Aucun filtre"));
			filters.push_back(make_pair(FILTER_ONLATE, "Clients en retard"));
			filters.push_back(make_pair(FILTER_DUE, "Clients d�biteurs"));
			filters.push_back(make_pair(FILTER_OUTDATED_GUARANTEE, "Cautions p�rim�es"));
			filters.push_back(make_pair(FILTER_ALL_PROBLEMS, "Tous probl�mes"));
			filters.push_back(make_pair(FILTER_CONTRACTED_GUARANTEE, "Cautions sp�ciales"));
			return filters;
		}
	}
}

