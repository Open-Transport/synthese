
//////////////////////////////////////////////////////////////////////////
/// ContactCentersAdmin class implementation.
///	@file ContactCentersAdmin.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "ContactCentersAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "ReservationContact.h"
#include "ContactCenterAdmin.hpp"
#include "ResultHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "HTMLModule.h"
#include "RemoveObjectAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "ContactCenterUpdateAction.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ContactCentersAdmin>::FACTORY_KEY = "ContactCenters";
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ContactCentersAdmin>::ICON = "help.png";
		template<> const string AdminInterfaceElementTemplate<ContactCentersAdmin>::DEFAULT_TITLE = "Centres de contact";
	}

	namespace pt
	{
		ContactCentersAdmin::ContactCentersAdmin()
			: AdminInterfaceElementTemplate<ContactCentersAdmin>()
		{ }



		void ContactCentersAdmin::setFromParametersMap(
			const ParametersMap& map
		){
		}



		ParametersMap ContactCentersAdmin::getParametersMap() const
		{
			ParametersMap m;
			return m;
		}



		bool ContactCentersAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void ContactCentersAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminFunctionRequest<ContactCenterAdmin> openRequest(request);

			AdminActionFunctionRequest<RemoveObjectAction, ContactCentersAdmin> deleteRequest(request, *this);

			AdminActionFunctionRequest<ContactCenterUpdateAction, ContactCenterAdmin> addRequest(request);
			addRequest.setActionWillCreateObject();
			addRequest.setActionFailedPage<ContactCentersAdmin>();

			HTMLForm f(addRequest.getHTMLForm("add"));

			HTMLTable::ColsVector c;
			c.push_back(string());
			c.push_back("Nom");
			c.push_back("Numéro téléphone");
			c.push_back(string());
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

			stream << "<h1>Centres de contact</h1>";
			stream << f.open();
			stream << t.open();

			BOOST_FOREACH(
				const ReservationContact::Registry::value_type& it,
				Env::GetOfficialEnv().getRegistry<ReservationContact>()
			){
				// New row
				stream << t.row();

				// Open button
				stream << t.col();
				openRequest.getPage()->setContactCenter(it.second);
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + ContactCenterAdmin::ICON);

				// Name
				stream << t.col();
				stream << it.second->getName();

				// Phone number
				stream << t.col();
				stream << it.second->getPhoneExchangeNumber();

				// Delete button
				stream << t.col();
				deleteRequest.getAction()->setObjectId(it.first);
				stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer ce tarif ?", "/admin/img/delete.png");
			}

			// New row for object creation
			stream << t.row();
			stream << t.col();
			stream << t.col();
			stream << f.getTextInput(ContactCenterUpdateAction::PARAMETER_NAME, string(), "(nom)");
			stream << t.col() << f.getTextInput(ContactCenterUpdateAction::PARAMETER_PHONE_NUMBER, string(), "(numéro)");
			stream << t.col() << f.getSubmitButton("Ajouter");

			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks ContactCentersAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks ContactCentersAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			const ContactCenterAdmin* fa(
				dynamic_cast<const ContactCenterAdmin*>(&currentPage)
			);

			if(fa)
			{
				boost::shared_ptr<ContactCenterAdmin> p(getNewPage<ContactCenterAdmin>());
				p->setContactCenter(fa->getContactCenter());
				links.push_back(p);
			}

			return links;
		}
}	}
