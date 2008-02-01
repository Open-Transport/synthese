
/** VinciAccountsAdminInterfaceElement class implementation.
	@file VinciAccountsAdminInterfaceElement.cpp
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

#include "VinciAccountsAdminInterfaceElement.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciRight.h"
#include "71_vinci_bike_rental/VinciAccountAdmin.h"
#include "71_vinci_bike_rental/VinciCreateStockAction.h"

#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Account.h"
#include "57_accounting/AccountAddAction.h"
#include "57_accounting/AccountRenameAction.h"
#include "57_accounting/AccountUnitPriceUpdateAction.h"
#include "57_accounting/AccountLockAction.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/HomeAdmin.h"
#include "32_admin/AdminRequest.h"

#include "30_server/ActionFunctionRequest.h"

#include "05_html/SearchFormHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace vinci;
	using namespace accounts;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VinciAccountsAdminInterfaceElement>::FACTORY_KEY("vinciaccounts");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VinciAccountsAdminInterfaceElement>::ICON("cart.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<VinciAccountsAdminInterfaceElement>::DISPLAY_MODE(AdminInterfaceElement::EVER_DISPLAYED);

		template<> string AdminInterfaceElementTemplate<VinciAccountsAdminInterfaceElement>::getSuperior()
		{
			return HomeAdmin::FACTORY_KEY;
		}
	}

	namespace vinci
	{
		const string VinciAccountsAdminInterfaceElement::PARAMETER_NAME("na");
		const string VinciAccountsAdminInterfaceElement::PARAMETER_CODE("co");
		const string VinciAccountsAdminInterfaceElement::PARAMETER_UNIT_PRICE("up");
		const string VinciAccountsAdminInterfaceElement::PARAMETER_LOCKED("lo");
		const string VinciAccountsAdminInterfaceElement::PARAMETER_STOCK("st");

		VinciAccountsAdminInterfaceElement::VinciAccountsAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciAccountsAdminInterfaceElement>()
		{ }

		void VinciAccountsAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			_searchName = map.getString(PARAMETER_NAME, false, FACTORY_KEY);
			_searchCode = map.getString(PARAMETER_CODE, false, FACTORY_KEY);
			_requestParameters = ResultHTMLTable::getParameters(map.getMap(), string(), 30);
		}

		string VinciAccountsAdminInterfaceElement::getTitle() const
		{
			return "Produits";
		}

		void VinciAccountsAdminInterfaceElement::display(ostream& stream, VariablesMap & variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<VinciAccountsAdminInterfaceElement>();

			ActionFunctionRequest<AccountAddAction,AdminRequest> addRequest(request);
			addRequest.getFunction()->setPage<VinciAccountsAdminInterfaceElement>();
			addRequest.getAction()->setCurrency(VinciBikeRentalModule::getEuroCurrency());
			addRequest.getAction()->setAutoIncrementClass(true);
			addRequest.getAction()->setRightUser(VinciBikeRentalModule::getVinciUser());

			ActionFunctionRequest<VinciCreateStockAction,AdminRequest> createStockRequest(request);
			createStockRequest.getFunction()->setPage<VinciAccountAdmin>();
			createStockRequest.getFunction()->setActionFailedPage<VinciAccountsAdminInterfaceElement>();

			ActionFunctionRequest<AccountUnitPriceUpdateAction,AdminRequest> unitPriceRequest(request);
			unitPriceRequest.getFunction()->setPage<VinciAccountsAdminInterfaceElement>();

			ActionFunctionRequest<AccountRenameAction,AdminRequest> renameRequest(request);
			renameRequest.getFunction()->setPage<VinciAccountsAdminInterfaceElement>();

			FunctionRequest<AdminRequest> openStockRequest(request);
			openStockRequest.getFunction()->setPage<VinciAccountAdmin>();

			ActionFunctionRequest<AccountLockAction,AdminRequest> lockRequest(request);
			lockRequest.getFunction()->setPage<VinciAccountsAdminInterfaceElement>();

			// Search of the results
			string emptyString;
			vector<shared_ptr<Account> > salesSearchResult = AccountTableSync::search(
				VinciBikeRentalModule::getVinciUser()->getKey()
				, VinciBikeRentalModule::VINCI_SERVICES_CODE + _searchCode
				, UNKNOWN_VALUE
				, emptyString
				, _searchName
				, _requestParameters.orderField == PARAMETER_NAME
				, _requestParameters.raisingOrder
				, _requestParameters.first
				, _requestParameters.maxSize
			);
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
			stream << st.close();

			// Results list
			stream << "<h1>Produits vendus</h1>";

			ResultHTMLTable::ResultParameters p(ResultHTMLTable::getParameters(_requestParameters, salesSearchResult));

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_NAME, "Nom"));
			h.push_back(make_pair(PARAMETER_CODE, "Code article"));
			h.push_back(make_pair(PARAMETER_UNIT_PRICE, "Prix unitaire"));
			h.push_back(make_pair(PARAMETER_STOCK, "Stock"));
			h.push_back(make_pair(PARAMETER_LOCKED, "Liste"));

			ResultHTMLTable t(h, st.getForm(), _requestParameters, p);
			stream << t.open();

			// Loop on each result
			for (vector<shared_ptr<Account> >::const_iterator it = salesSearchResult.begin(); it != salesSearchResult.end(); ++it)
			{
				createStockRequest.setObjectId((*it)->getKey());
				openStockRequest.setObjectId((*it)->getStockAccountId());
				unitPriceRequest.setObjectId((*it)->getKey());
				renameRequest.setObjectId((*it)->getKey());
				lockRequest.setObjectId((*it)->getKey());

				stream << t.row();
				stream << t.col();
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

				stream << t.col() << (*it)->getRightClassNumber().substr(1);
				
				stream << t.col();
				if ((*it)->getLocked())
				{
					stream << ((*it)->getUnitPrice() ? Conversion::ToString((*it)->getUnitPrice()) : string("NON"));
				}
				else
				{
					HTMLForm upm(unitPriceRequest.getHTMLForm("up"+Conversion::ToString((*it)->getKey())));
					stream << upm.open();
					stream << upm.getTextInput(AccountUnitPriceUpdateAction::PARAMETER_VALUE, Conversion::ToString((*it)->getUnitPrice()));
					stream << upm.getSubmitButton("Modifier");
					stream << upm.close();
				}

				stream << t.col() << (((*it)->getStockAccountId() > 0) 
					? "OUI : " + HTMLModule::getLinkButton(openStockRequest.getURL(), "Ouvrir", string(), "cart_go.png")
					: "NON : " +((*it)->getLocked() ? "" : HTMLModule::getLinkButton(createStockRequest.getURL(), "Activer", string(), "cart_add.png")));
				stream << t.col() << ((*it)->getLocked() ? "NON" : "OUI : " + HTMLModule::getLinkButton(lockRequest.getURL(), "Supprimer", "E^tes-vous sûr de vouloir supprimer le compte ? (cette opération est définitive)", "cart_delete.png"));
			}

			// New account
			addRequest.getAction()->setClass(VinciBikeRentalModule::VINCI_SERVICES_SALES_CODE);
			HTMLForm af(addRequest.getHTMLForm("addf"));

			stream << t.row();
			stream << t.col() << af.open();
			stream << af.getTextInput(AccountAddAction::PARAMETER_NAME, emptyString, string("(nom du produit)"));
			stream << " " << af.getSubmitButton("Ajouter");
			stream << af.close();
			
			stream << t.close();

			// Change accounts list
			stream << "<h1>Modes de paiement</h1>";

			ResultHTMLTable::ResultParameters pc(ResultHTMLTable::getParameters(_requestParameters, changeSearchResult));

			ResultHTMLTable::HeaderVector hc;
			hc.push_back(make_pair(PARAMETER_NAME, "Nom"));
			hc.push_back(make_pair(PARAMETER_CODE, "Code mode"));
			hc.push_back(make_pair(PARAMETER_LOCKED, "Liste"));

			ResultHTMLTable tc(hc, st.getForm(), _requestParameters, p);
			stream << tc.open();

			// Loop on each result
			for (vector<shared_ptr<Account> >::const_iterator it = changeSearchResult.begin(); it != changeSearchResult.end(); ++it)
			{
				stream << tc.row();
				stream << t.col();
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
				stream << t.col() << ((*it)->getLocked() ? "NON" : "OUI : " + HTMLModule::getLinkButton(lockRequest.getURL(), "Supprimer", "E^tes-vous sûr de vouloir supprimer le compte ? (cette opération est définitive)", "cart_delete.png"));
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

		bool VinciAccountsAdminInterfaceElement::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<VinciRight>(READ);
		}
	}
}
