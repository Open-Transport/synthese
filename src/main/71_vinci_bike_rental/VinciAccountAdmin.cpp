
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

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"

#include "32_admin/AdminParametersException.h"

#include "30_server/QueryString.h"

#include "05_html/HTMLTable.h"

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
				_pageLink.name = _account->getName();
				_pageLink.parameterName = QueryString::PARAMETER_OBJECT_ID;
				_pageLink.parameterValue = Conversion::ToString(id);
			}
			catch (...)
			{
				throw AdminParametersException("Account not found");
			}
		}


		void VinciAccountAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			stream << "<h1>Stocks</h1>";
			StocksSize ss(getStocksSize(_account->getKey(), UNKNOWN_VALUE));

			HTMLTable::ColsVector sv;
			sv.push_back("Site");
			sv.push_back("Nombre de pièces");
//			sv.push_back("Alerte");
			HTMLTable tv(sv);
			stream << tv.open();

			for (StocksSize::const_iterator it(ss.begin()); it != ss.end(); ++it)
			{
				shared_ptr<const VinciSite> si(VinciSiteTableSync::Get(it->first.second));
				stream << tv.row();
				stream << tv.col();
				stream << si->getName();
				stream << tv.col();
				stream << it->second.size;
				stream << tv.col();
/*				if ((*it)->getMinAlert() > 0 && it->second.size < (*it)->getMinAlert())
					stream << "ALERTE BAS";
				if ((*it)->getMaxAlert() > 0 && (*it)->getStockSize() > (*it)->getMaxAlert())
					stream << "ALERTE HAUT";
*/			}
			stream << tv.close();
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
	}
}
