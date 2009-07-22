
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
#include "UserUpdateAction.h"
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
			const ParametersMap& map,
			bool objectWillBeCreatedLater
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
			ParametersMap m;
			if(_user.get()) m.insert(Request::PARAMETER_OBJECT_ID, _user->getKey());
			return m;
		}

		
		void ResaCustomerAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_PROPERTIES))
			{

				// Requests
				AdminActionFunctionRequest<UserUpdateAction,ResaCustomerAdmin> updateRequest(
					_request
				);
				updateRequest.getAction()->setUser(_user);

				AdminFunctionRequest<ReservationRoutePlannerAdmin> routeplannerRequest(
					_request
				);
				routeplannerRequest.getPage()->setCustomer(_user);

				// Display
				stream << "<h1>Liens</h1>";
				stream << "<p>";
				stream << HTMLModule::getLinkButton(routeplannerRequest.getURL(), "Recherche d'itinéraire", "", ReservationRoutePlannerAdmin::ICON);
				stream << "</p>";
				
				stream << "<h1>Coordonnées</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm("upd"));
				t.getForm().setUpdateRight(tabHasWritePermissions());
				stream << t.open();
				stream << t.title("Connexion");
				stream << t.cell(
					"Login",
					t.getForm().getTextInput(UserUpdateAction::PARAMETER_LOGIN, _user->getLogin())
				);

				stream << t.title("Coordonnées");
				stream << t.cell("Prénom", t.getForm().getTextInput(UserUpdateAction::PARAMETER_SURNAME, _user->getSurname()));
				stream << t.cell("Nom", t.getForm().getTextInput(UserUpdateAction::PARAMETER_NAME, _user->getName()));
				stream << t.cell("Adresse", t.getForm().getTextAreaInput(UserUpdateAction::PARAMETER_ADDRESS, _user->getAddress(), 4, 50));
				stream << t.cell("Code postal", t.getForm().getTextInput(UserUpdateAction::PARAMETER_POSTAL_CODE, _user->getPostCode()));
				stream << t.cell("Ville", t.getForm().getTextInput(UserUpdateAction::PARAMETER_CITY, _user->getCityText()));
				stream << t.cell("Téléphone",t.getForm().getTextInput(UserUpdateAction::PARAMETER_PHONE, _user->getPhone()));
				stream << t.cell("E-mail",t.getForm().getTextInput(UserUpdateAction::PARAMETER_EMAIL, _user->getEMail()));

				stream << t.title("Droits");
				stream << t.cell("Accès site web",t.getForm().getOuiNonRadioInput(UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN, _user->getConnectionAllowed()));

				if(_user->getProfile()->getKey() == ResaModule::GetBasicResaCustomerProfile()->getKey() ||
					_user->getProfile()->getKey() == ResaModule::GetAutoResaResaCustomerProfile()->getKey()
				){
					map<uid, string> profiles;
					profiles[ResaModule::GetBasicResaCustomerProfile()->getKey()] = "Auto réservation interdite";
					profiles[ResaModule::GetAutoResaResaCustomerProfile()->getKey()] = "Auto réservation autorisée";

					stream << t.cell(
						"Auto-réservation",
						t.getForm().getRadioInputCollection(UserUpdateAction::PARAMETER_PROFILE_ID, profiles, _user->getProfile()->getKey(), true)
					);
				}
				else
				{
					stream << t.cell("Droits","Cet utilisateur n'est pas un client. Son niveau de droits est défini par ailleurs et ne peut être visualisé ni modifié ici.");
				}
				stream << t.close();
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
				stream << "<h1>Historique / Réservations</h1>";

				// Results
				_log.display(
					stream,
					FunctionRequest<AdminRequest>(_request),
					true,
					true
				);
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}

		bool ResaCustomerAdmin::isAuthorized(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{
			return _request.isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}
		


		std::string ResaCustomerAdmin::getTitle() const
		{
			return _user.get() ? _user->getFullName() : DEFAULT_TITLE;
		}


		
		void ResaCustomerAdmin::_buildTabs(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			_tabs.clear();
			bool writeRight(_request.isAuthorized<ResaRight>(WRITE, UNKNOWN_RIGHT_LEVEL));

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, writeRight, "user.png"));
			_tabs.push_back(Tab("Paramètres", TAB_PARAMETERS, writeRight, "cog.png"));
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
	}
}
