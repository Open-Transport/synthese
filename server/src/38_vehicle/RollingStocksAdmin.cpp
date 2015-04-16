
//////////////////////////////////////////////////////////////////////////
/// RollingStocksAdmin class implementation.
///	@file RollingStocksAdmin.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "RollingStocksAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "VehicleModule.hpp"
#include "RollingStock.hpp"
#include "RollingStockAdmin.hpp"
#include "ResultHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "HTMLModule.h"
#include "RemoveObjectAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "RollingStockUpdateAction.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace vehicle;
	using namespace html;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, RollingStocksAdmin>::FACTORY_KEY("RollingStocksAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<RollingStocksAdmin>::ICON("database.png");
		template<> const string AdminInterfaceElementTemplate<RollingStocksAdmin>::DEFAULT_TITLE("Modes de transport");
	}

	namespace vehicle
	{
		RollingStocksAdmin::RollingStocksAdmin()
			: AdminInterfaceElementTemplate<RollingStocksAdmin>()
		{ }



		void RollingStocksAdmin::setFromParametersMap(
			const ParametersMap& map
		){
		}



		ParametersMap RollingStocksAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}



		bool RollingStocksAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true; // TODO create a vehicle right
			// return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void RollingStocksAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminFunctionRequest<RollingStockAdmin> openRequest(request);

			AdminActionFunctionRequest<RemoveObjectAction, RollingStocksAdmin> deleteRequest(request, *this);

			AdminActionFunctionRequest<RollingStockUpdateAction, RollingStockAdmin> addRequest(request);
			addRequest.setActionWillCreateObject();
			addRequest.setActionFailedPage<RollingStocksAdmin>();

			HTMLForm f(addRequest.getHTMLForm("add"));

			HTMLTable::ColsVector c;
			c.push_back("ID");
			c.push_back("Nom");
			c.push_back("Action");
			c.push_back("Action");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

			stream << f.open();
			stream << t.open();

			BOOST_FOREACH(const RollingStock::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<RollingStock>())
			{
				stream << t.row();

				stream << t.col();
				stream << it.first;

				stream << t.col();
				stream << it.second->getName();

				stream << t.col();
				openRequest.getPage()->setRollingStock(it.second);
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + RollingStockAdmin::ICON);

				stream << t.col();
				deleteRequest.getAction()->setObjectId(it.first);
				stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le mode ?", "/admin/img/delete.png");
			}

			stream << t.row();
			stream << t.col();
			stream << t.col();
			stream << f.getTextInput(RollingStockUpdateAction::PARAMETER_NAME, string(), "(nom)");
			stream << t.col(2);
			stream << f.getSubmitButton("Ajouter");

			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks RollingStocksAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const VehicleModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks RollingStocksAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			const RollingStockAdmin* rsa(
				dynamic_cast<const RollingStockAdmin*>(&currentPage)
			);

			if(rsa)
			{
				boost::shared_ptr<RollingStockAdmin> p(getNewPage<RollingStockAdmin>());
				p->setRollingStock(rsa->getRollingStock());
				links.push_back(p);
			}

			return links;
		}
}	}
