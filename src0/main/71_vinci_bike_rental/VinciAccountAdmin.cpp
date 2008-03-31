
/** VinciAccountAdmin class implementation.
	@file VinciAccountAdmin.cpp
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

#include "VinciAccountAdmin.h"
#include "VinciAccountsAdminInterfaceElement.h"

#include "71_vinci_bike_rental/VinciStockAlert.h"
#include "71_vinci_bike_rental/VinciStockAlertTableSync.h"
#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/AdvancedSelectTableSync.h"
#include "71_vinci_bike_rental/VinciStockFullfillAction.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/AccountUnitPriceUpdateAction.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

#include "30_server/QueryString.h"
#include "30_server/ActionFunctionRequest.h"

#include "05_html/PropertiesHTMLTable.h"

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

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VinciAccountAdmin>::FACTORY_KEY("vinci_account");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VinciAccountAdmin>::ICON("cart.png");
		template<> const string AdminInterfaceElementTemplate<VinciAccountAdmin>::DEFAULT_TITLE("Produit inconnu");
	}

	namespace vinci
	{
		VinciAccountAdmin::VinciAccountAdmin()
			: AdminInterfaceElementTemplate<VinciAccountAdmin>()
		{ }

		void VinciAccountAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
				_account = AccountTableSync::Get(id);
			}
			catch (...)
			{
				throw AdminParametersException("Account not found");
			}
		}


		void VinciAccountAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			ActionFunctionRequest<VinciStockFullfillAction,AdminRequest> fRequest(request);
			fRequest.getFunction()->setPage<VinciAccountAdmin>();
			fRequest.setObjectId(_account->getKey());
			fRequest.getAction()->setAccount(_account);

			ActionFunctionRequest<AccountUnitPriceUpdateAction,AdminRequest> unitPriceRequest(request);
			unitPriceRequest.getFunction()->setPage<VinciAccountAdmin>();
			unitPriceRequest.setObjectId(_account->getKey());

			// Display
			stream << "<h1>Propriétés</h1>";

			PropertiesHTMLTable pt(unitPriceRequest.getHTMLForm("prop"));
			stream << pt.open();
			stream << pt.cell("Prix unitaire", pt.getForm().getTextInput(AccountUnitPriceUpdateAction::PARAMETER_VALUE, Conversion::ToString(_account->getUnitPrice())));
			stream << pt.close();


			stream << "<h1>Stocks</h1>";
			StocksSize ss(getStocksSize(_account->getKey(), UNKNOWN_VALUE));

			HTMLTable::ColsVector sv;
			sv.push_back("Site");
			sv.push_back("Nombre de pièces");
//			sv.push_back("Alerte");
			HTMLTable tv(sv, "adminresults");
			stream << tv.open();

			for (StocksSize::const_iterator it(ss.begin()); it != ss.end(); ++it)
			{
				shared_ptr<const VinciSite> si(VinciSiteTableSync::Get(it->first.second));
				stream << tv.row();
				stream << tv.col();
				stream << si->getName();
				stream << tv.col();
				stream << it->second.size;
/*				stream << tv.col();
				if ((*it)->getMinAlert() > 0 && it->second.size < (*it)->getMinAlert())
					stream << "ALERTE BAS";
				if ((*it)->getMaxAlert() > 0 && (*it)->getStockSize() > (*it)->getMaxAlert())
					stream << "ALERTE HAUT";
*/			}
			stream << tv.close();

			stream << "<h1>Approvisionnement</h1>";

			PropertiesHTMLTable nt(fRequest.getHTMLForm("newstock"));

			stream << nt.open();
			vector<pair<uid, string> > sites;
			vector<shared_ptr<VinciSite> > rsites(VinciSiteTableSync::search(string("%")));
			for (vector<shared_ptr<VinciSite> >::const_iterator it(rsites.begin()); it != rsites.end(); ++it)
				sites.push_back(make_pair((*it)->getKey(), (*it)->getName()));
			stream << nt.cell("Site", nt.getForm().getSelectInput(VinciStockFullfillAction::PARAMETER_SITE_ID, sites, static_cast<uid>(UNKNOWN_VALUE)));
			stream << nt.cell("Quantité", nt.getForm().getTextInput(VinciStockFullfillAction::PARAMETER_PIECES, string()));
			stream << nt.cell("Commentaire", nt.getForm().getTextAreaInput(VinciStockFullfillAction::PARAMETER_COMMENT, string(), 3, 50));
			stream << nt.close();

		}

		bool VinciAccountAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}

		AdminInterfaceElement::PageLinks VinciAccountAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == VinciAccountsAdminInterfaceElement::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY)
			{
				links.push_back(currentPage.getPageLink());
			}
			return links;
		}

		std::string VinciAccountAdmin::getTitle() const
		{
			return _account.get() ? _account->getName() : DEFAULT_TITLE;
		}

		std::string VinciAccountAdmin::getParameterName() const
		{
			return _account.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string VinciAccountAdmin::getParameterValue() const
		{
			return _account.get() ? Conversion::ToString(_account->getKey()) : string();
		}
	}
}
