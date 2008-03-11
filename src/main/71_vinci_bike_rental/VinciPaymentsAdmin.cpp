
/** VinciPaymentsAdmin class implementation.
	@file VinciPaymentsAdmin.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "VinciPaymentsAdmin.h"
#include "VinciBikeRentalModule.h"

#include "30_server/QueryString.h"
#include "30_server/ActionFunctionRequest.h"

#include "05_html/SearchFormHTMLTable.h"

#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Account.h"
#include "57_accounting/AccountAddAction.h"
#include "57_accounting/AccountRenameAction.h"
#include "57_accounting/AccountUnitPriceUpdateAction.h"
#include "57_accounting/AccountLockAction.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminRequest.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace vinci;
	using namespace html;
	using namespace accounts;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VinciPaymentsAdmin>::FACTORY_KEY("VinciPaymentsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VinciPaymentsAdmin>::ICON("money.png");
		template<> const string AdminInterfaceElementTemplate<VinciPaymentsAdmin>::DEFAULT_TITLE("Modes de paiement");
	}

	namespace vinci
	{
		const string VinciPaymentsAdmin::PARAMETER_NAME("na");
		const string VinciPaymentsAdmin::PARAMETER_CODE("co");
		const string VinciPaymentsAdmin::PARAMETER_LOCKED("lo");

		VinciPaymentsAdmin::VinciPaymentsAdmin()
			: AdminInterfaceElementTemplate<VinciPaymentsAdmin>()
		{ }
		
		void VinciPaymentsAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_searchName = map.getString(PARAMETER_NAME, false, FACTORY_KEY);
			_searchCode = map.getString(PARAMETER_CODE, false, FACTORY_KEY);
			_requestParameters = ResultHTMLTable::getParameters(map.getMap(), string(), 30);
		}
		
		void VinciPaymentsAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<VinciPaymentsAdmin>();

			ActionFunctionRequest<AccountAddAction,AdminRequest> addRequest(request);
			addRequest.getFunction()->setPage<VinciPaymentsAdmin>();
			addRequest.getAction()->setCurrency(VinciBikeRentalModule::getEuroCurrency());
			addRequest.getAction()->setAutoIncrementClass(true);
			addRequest.getAction()->setRightUser(VinciBikeRentalModule::getVinciUser());

			ActionFunctionRequest<AccountRenameAction,AdminRequest> renameRequest(request);
			renameRequest.getFunction()->setPage<VinciPaymentsAdmin>();

			ActionFunctionRequest<AccountLockAction,AdminRequest> lockRequest(request);
			lockRequest.getFunction()->setPage<VinciPaymentsAdmin>();

			// Search of the results
			string emptyString;
			vector<shared_ptr<Account> > changeSearchResult = AccountTableSync::search(
				VinciBikeRentalModule::getVinciUser()->getKey()
				, VinciBikeRentalModule::VINCI_CHANGE_CODE
				, UNKNOWN_VALUE
				, emptyString
				, _searchName
				, _requestParameters.orderField == PARAMETER_NAME
				, _requestParameters.raisingOrder
				, _requestParameters.first
				, _requestParameters.maxSize
				);

			// HTML search form
			stream << "<h1>Recherche</h1>";
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAMETER_NAME, _searchName));
			stream << st.cell("Code", st.getForm().getTextInput(PARAMETER_CODE, _searchCode));
			stream << st.close();

			// Change accounts list
			stream << "<h1>Modes de paiement</h1>";

			ResultHTMLTable::ResultParameters pc(ResultHTMLTable::getParameters(_requestParameters, changeSearchResult));

			ResultHTMLTable::HeaderVector hc;
			hc.push_back(make_pair(PARAMETER_NAME, "Nom"));
			hc.push_back(make_pair(PARAMETER_CODE, "Code mode"));
			hc.push_back(make_pair(PARAMETER_LOCKED, "Liste"));

			ResultHTMLTable tc(hc, st.getForm(), _requestParameters, pc);
			stream << tc.open();

			// Loop on each result
			for (vector<shared_ptr<Account> >::const_iterator it = changeSearchResult.begin(); it != changeSearchResult.end(); ++it)
			{
				lockRequest.setObjectId((*it)->getKey());
				renameRequest.setObjectId((*it)->getKey());

				stream << tc.row();
				stream << tc.col();
				if ((*it)->getLocked())
					stream << (*it)->getName();
				else
				{
					HTMLForm upr(renameRequest.getHTMLForm("re"+Conversion::ToString((*it)->getKey())));
					stream << upr.open();
					stream << upr.getTextInput(AccountRenameAction::PARAMETER_VALUE, (*it)->getName());
					stream << upr.getSubmitButton("Renommer");
					stream << upr.close();
				}
				stream << tc.col() << (*it)->getRightClassNumber().substr(1);
				stream << tc.col() << ((*it)->getLocked() ? "NON" : "OUI : " + HTMLModule::getLinkButton(lockRequest.getURL(), "Supprimer", "Êtes-vous sûr de vouloir supprimer le compte ? (cette opération est définitive)", "money_delete.png"));
			}

			// New account
			addRequest.getAction()->setClass(VinciBikeRentalModule::VINCI_CHANGE_CUSTOM_CODE);
			HTMLForm afc(addRequest.getHTMLForm("addc"));

			stream << tc.row();
			stream << tc.col() << afc.open();
			stream << afc.getTextInput(AccountAddAction::PARAMETER_NAME, emptyString, string("(nom du mode de paiement)"));
			stream << " " << afc.getSubmitButton("Ajouter");
			stream << afc.close();

			stream << tc.close();
		}

		bool VinciPaymentsAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks VinciPaymentsAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == VinciBikeRentalModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks VinciPaymentsAdmin::getSubPages(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}
	}
}
