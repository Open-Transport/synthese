
//////////////////////////////////////////////////////////////////////////
/// FaresAdmin class implementation.
///	@file FaresAdmin.cpp
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

#include "FaresAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "FareModule.hpp"
#include "User.h"
#include "Fare.hpp"
#include "FareAdmin.hpp"
#include "ResultHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "HTMLModule.h"
#include "RemoveObjectAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "FareUpdateAction.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace fare;
	using namespace html;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, FaresAdmin>::FACTORY_KEY("FaresAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<FaresAdmin>::ICON("database.png");
		template<> const string AdminInterfaceElementTemplate<FaresAdmin>::DEFAULT_TITLE("Tarifications");
	}

	namespace fare
	{



		FaresAdmin::FaresAdmin()
			: AdminInterfaceElementTemplate<FaresAdmin>()
		{ }



		void FaresAdmin::setFromParametersMap(
			const ParametersMap& map
		){
		}



		ParametersMap FaresAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}



		bool FaresAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true; // TODO create fare right
		}



		void FaresAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminFunctionRequest<FareAdmin> openRequest(request);

			AdminActionFunctionRequest<RemoveObjectAction, FaresAdmin> deleteRequest(request);

			AdminActionFunctionRequest<FareUpdateAction, FareAdmin> addRequest(request);
			addRequest.setActionWillCreateObject();
			addRequest.setActionFailedPage<FaresAdmin>();

			HTMLForm f(addRequest.getHTMLForm("add"));

			HTMLTable::ColsVector c;
			c.push_back("ID");
			c.push_back("Nom");
			c.push_back("Type");
			c.push_back("Action");
			c.push_back("Action");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

			stream << f.open();
			stream << t.open();

			BOOST_FOREACH(const Fare::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<Fare>())
			{
				stream << t.row();

				stream << t.col();
				stream << it.first;

				stream << t.col();
				stream << it.second->getName();

				stream << t.col();
				stream << (it.second->getType() ? FareType::GetTypeName(it.second->getType()->getTypeNumber()) : string("non défini"));

				stream << t.col();
				openRequest.getPage()->setFare(it.second);
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + FareAdmin::ICON);

				stream << t.col();
				deleteRequest.getAction()->setObjectId(it.first);
				stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer ce tarif ?", "/admin/img/delete.png");
			}

			stream << t.row();
			stream << t.col();
			stream << t.col();
			stream << f.getTextInput(FareUpdateAction::PARAMETER_NAME, string(), "(nom)");
			stream << t.col() << f.getSelectInput(FareUpdateAction::PARAMETER_TYPE, FareType::GetTypesList(), optional<FareType::FareTypeNumber>());
			stream << t.col(2);
			stream << f.getSubmitButton("Ajouter");

			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks FaresAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const FareModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks FaresAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			const FareAdmin* fa(
				dynamic_cast<const FareAdmin*>(&currentPage)
			);

			if(fa)
			{
				boost::shared_ptr<FareAdmin> p(getNewPage<FareAdmin>());
				p->setFare(fa->getFare());
				links.push_back(p);
			}

			return links;
		}
}	}
