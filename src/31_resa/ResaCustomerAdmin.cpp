
/** ResaCustomerAdmin class implementation.
	@file ResaCustomerAdmin.cpp
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

#include "ResaCustomerAdmin.h"
#include "Profile.h"
#include "ResaCustomersAdmin.h"
#include "ResaModule.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "ResaDBLog.h"
#include "CancelReservationAction.h"
#include "ReservationRoutePlannerAdmin.h"
#include "ResaRight.h"

#include "PropertiesHTMLTable.h"

#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "Request.h"

#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"

#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"

#include "User.h"
#include "UserTableSync.h"
#include "ReservationUserUpdateAction.h"
#include "SendPasswordAction.h"
#include "SecurityModule.h"

#include "DateTime.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace html;
	using namespace security;
	using namespace time;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaCustomerAdmin>::FACTORY_KEY("ResaCustomerAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaCustomerAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<ResaCustomerAdmin>::DEFAULT_TITLE("Client inconnu");
	}

	namespace resa
	{
		const string ResaCustomerAdmin::TAB_PROPERTIES("properties");
		const string ResaCustomerAdmin::TAB_PARAMETERS("parameters");
		const string ResaCustomerAdmin::TAB_LOG("log");

		ResaCustomerAdmin::ResaCustomerAdmin(
		): AdminInterfaceElementTemplate<ResaCustomerAdmin>(),
			_log("log")
		{
		}
		
		void ResaCustomerAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_user = UserTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv()
				);
			}
			catch (...)
			{
				throw AdminParametersException("Bad user id");
			}

			_log.set(
				map,
				ResaDBLog::FACTORY_KEY,
				_user->getKey()
			);
		}
		
		
		
		server::ParametersMap ResaCustomerAdmin::getParametersMap() const
		{
			ParametersMap m(_log.getParametersMap());
			if(_user.get()) m.insert(Request::PARAMETER_OBJECT_ID, _user->getKey());
			return m;
		}

		
		void ResaCustomerAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{

				// Requests
				AdminActionFunctionRequest<ReservationUserUpdateAction,ResaCustomerAdmin> updateRequest(
					_request
				);
				updateRequest.getAction()->setUser(_user);

				AdminActionFunctionRequest<SendPasswordAction,ResaCustomerAdmin> sendPasswordRequest(
					_request
				);
				sendPasswordRequest.getAction()->setUser(_user);

				AdminFunctionRequest<ReservationRoutePlannerAdmin> routeplannerRequest(
					_request
				);
				routeplannerRequest.getPage()->setCustomer(_user);

				// Display
				stream << "<h1>Liens</h1>";
				stream << "<p>";
				stream << HTMLModule::getLinkButton(routeplannerRequest.getURL(), "Recherche d'itin�raire", "", ReservationRoutePlannerAdmin::ICON);
				stream << "</p>";
				
				stream << "<h1>Coordonn�es</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm("upd"));
				t.getForm().setUpdateRight(tabHasWritePermissions());
				stream << t.open();
				stream << t.title("Connexion");
				stream << t.cell(
					"Login",
					t.getForm().getTextInput(ReservationUserUpdateAction::PARAMETER_LOGIN, _user->getLogin())
				);

				stream << t.title("Coordonn�es");
				stream << t.cell("Pr�nom", t.getForm().getTextInput(ReservationUserUpdateAction::PARAMETER_SURNAME, _user->getSurname()));
				stream << t.cell("Nom", t.getForm().getTextInput(ReservationUserUpdateAction::PARAMETER_NAME, _user->getName()));
				stream << t.cell("Adresse", t.getForm().getTextAreaInput(ReservationUserUpdateAction::PARAMETER_ADDRESS, _user->getAddress(), 4, 50));
				stream << t.cell("Code postal", t.getForm().getTextInput(ReservationUserUpdateAction::PARAMETER_POSTAL_CODE, _user->getPostCode()));
				stream << t.cell("Ville", t.getForm().getTextInput(ReservationUserUpdateAction::PARAMETER_CITY, _user->getCityText()));
				stream << t.cell("T�l�phone",t.getForm().getTextInput(ReservationUserUpdateAction::PARAMETER_PHONE, _user->getPhone()));
				stream << t.cell("E-mail",t.getForm().getTextInput(ReservationUserUpdateAction::PARAMETER_EMAIL, _user->getEMail()));

				stream << t.title("Droits");
				stream << t.cell("Acc�s site web",t.getForm().getOuiNonRadioInput(ReservationUserUpdateAction::PARAMETER_AUTHORIZED_LOGIN, _user->getConnectionAllowed()));

				if(_user->getProfile()->getKey() == ResaModule::GetBasicResaCustomerProfile()->getKey() ||
					_user->getProfile()->getKey() == ResaModule::GetAutoResaResaCustomerProfile()->getKey()
				){
					stream << t.cell(
						"Auto-r�servation autoris�e",
						t.getForm().getOuiNonRadioInput(ReservationUserUpdateAction::PARAMETER_AUTORESA_ACTIVATED, _user->getProfile()->getKey() == ResaModule::GetAutoResaResaCustomerProfile()->getKey())
					);
				}
				else
				{
					stream << t.cell("Droits","Cet utilisateur n'est pas un client. Son niveau de droits est d�fini par ailleurs et ne peut �tre visualis� ni modifi� ici.");
				}
				stream << t.close();

				stream << "<p>" << HTMLModule::getLinkButton(sendPasswordRequest.getURL(), "Envoi de nouveau mot de passe par e-mail", string(), ResaDBLog::GetIconURL(ResaDBLog::EMAIL)) << "</p>";
			}


			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_PARAMETERS))
			{
				stream << "<h1>Trajets favoris</h1>";
			}

			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_LOG))
			{
				stream << "<h1>Historique / R�servations</h1>";

				// Results
				_log.display(
					stream,
					AdminRequest(_request, true),
					true,
					true
				);
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}

		bool ResaCustomerAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}
		


		std::string ResaCustomerAdmin::getTitle() const
		{
			return _user.get() ? _user->getFullName() : DEFAULT_TITLE;
		}


		
		void ResaCustomerAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();
			bool writeRight(profile.isAuthorized<ResaRight>(WRITE, UNKNOWN_RIGHT_LEVEL));

			_tabs.push_back(Tab("Propri�t�s", TAB_PROPERTIES, writeRight, "user.png"));
			_tabs.push_back(Tab("Param�tres", TAB_PARAMETERS, writeRight, "cog.png"));
			_tabs.push_back(Tab("Journal", TAB_LOG, writeRight, "book.png"));

			_tabBuilded = true;
		}



		void ResaCustomerAdmin::setUser(boost::shared_ptr<User> value)
		{
			_user = const_pointer_cast<const User>(value);
		}
	
	
		void ResaCustomerAdmin::setUser(boost::shared_ptr<const User> value)
		{
			_user = value;
		}
		
		
		bool ResaCustomerAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _user == static_cast<const ResaCustomerAdmin&>(other)._user;
		}

		boost::shared_ptr<const security::User> ResaCustomerAdmin::getUser() const
		{
			return _user;
		}



		AdminInterfaceElement::PageLinks ResaCustomerAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			PageLinks result;
			if(moduleKey == ResaModule::FACTORY_KEY)
			{
				util::RegistryKeyType callId(ResaModule::GetCurrentCallId(request.getSession()));

				if(callId > 0)
				{
					shared_ptr<const DBLogEntry> entry(DBLogEntryTableSync::Get(callId, *_env));

					if(entry->getObjectId() > 0)
					{
						try
						{
							shared_ptr<const User> user(UserTableSync::Get(entry->getObjectId(), *_env));

							shared_ptr<ResaCustomerAdmin> p(new ResaCustomerAdmin);
							p->setUser(user);

							result.push_back(p);
						}
						catch(...)
						{

						}

					}
	
				}
			}
			return result;
		}
	}
}
