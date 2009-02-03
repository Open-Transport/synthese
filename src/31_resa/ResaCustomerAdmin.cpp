
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

#include "QueryString.h"
#include "ActionFunctionRequest.h"
#include "Request.h"

#include "DBLogEntry.h"
#include "DBLogEntryTableSync.h"

#include "AdminParametersException.h"
#include "AdminRequest.h"

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
		const string ResaCustomerAdmin::PARAMETER_DISPLAY_CANCELLED("dc");
		const string ResaCustomerAdmin::PARAMETER_DISPLAY_EVENTS("de");
		const string ResaCustomerAdmin::PARAMETER_EVENT_DATE("ed");
		const string ResaCustomerAdmin::PARAMETER_TRAVEL_DATE("td");

		ResaCustomerAdmin::ResaCustomerAdmin()
			: AdminInterfaceElementTemplate<ResaCustomerAdmin>()
			, _eventDate(TIME_CURRENT)
			, _displayCancelled(false)
			, _displayEvents(false)
		{
			_eventDate -= 14;
		}
		
		void ResaCustomerAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions
		){
			_displayCancelled = map.getBool(PARAMETER_DISPLAY_CANCELLED, false, false, FACTORY_KEY);
			_displayEvents = map.getBool(PARAMETER_DISPLAY_EVENTS, false, false, FACTORY_KEY);
			Date da = map.getDate(PARAMETER_EVENT_DATE, false, FACTORY_KEY);
			if (!da.isUnknown())
				_eventDate = da;

			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_EVENT_DATE, 50, false);
			
			if(!doDisplayPreparationActions) return;
			
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			try
			{
				_user = UserTableSync::Get(id, _env);
			}
			catch (...)
			{
				throw AdminParametersException("Bad user id");
			}
		}
		
		
		
		server::ParametersMap ResaCustomerAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_DISPLAY_CANCELLED, _displayCancelled);
			m.insert(PARAMETER_DISPLAY_EVENTS, _displayEvents);
			m.insert(PARAMETER_EVENT_DATE, _eventDate);
			return m;
		}

		
		void ResaCustomerAdmin::display(
			ostream& stream
			, VariablesMap& variables
		) const	{

			// Requests
			ActionFunctionRequest<UserUpdateAction,AdminRequest> updateRequest(_request);
			updateRequest.getFunction()->setPage<ResaCustomerAdmin>();
			updateRequest.setObjectId(_request->getObjectId());

			FunctionRequest<AdminRequest> routeplannerRequest(_request);
			routeplannerRequest.getFunction()->setPage<ReservationRoutePlannerAdmin>();
			routeplannerRequest.getFunction()->setParameter(ReservationRoutePlannerAdmin::PARAMETER_CUSTOMER_ID, Conversion::ToString(_user->getKey()));

			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setPage<ResaCustomerAdmin>();
			searchRequest.setObjectId(_request->getObjectId());

			ActionFunctionRequest<CancelReservationAction,AdminRequest> cancelRequest(_request);
			cancelRequest.getFunction()->setPage<ResaCustomerAdmin>();
			cancelRequest.setObjectId(_request->getObjectId());

			bool writingRight(_request->isAuthorized<ResaRight>(WRITE,WRITE));

			// Search
			Env env;
			DBLogEntryTableSync::Search(
				env,
				ResaDBLog::FACTORY_KEY
				, DateTime(_eventDate, Hour(TIME_MIN))
				, DateTime(TIME_MAX)
				, UNKNOWN_VALUE
				, DBLogEntry::DB_LOG_UNKNOWN
				, _user->getKey()
				, ""
				, _requestParameters.first
				, _requestParameters.maxSize
				, _requestParameters.orderField == PARAMETER_EVENT_DATE
				, false
				, false
				, _requestParameters.raisingOrder 
			);

			// Display
			stream << "<h1>Liens</h1>";
			stream << "<p>";
			stream << HTMLModule::getLinkButton(routeplannerRequest.getURL(), "Recherche d'itinéraire", "", ReservationRoutePlannerAdmin::ICON);
			stream << "</p>";
			
			stream << "<h1>Coordonnées</h1>";

			PropertiesHTMLTable t(updateRequest.getHTMLForm("upd"));
			t.getForm().setUpdateRight(writingRight);
			stream << t.open();
			stream << t.title("Connexion");
			stream << t.cell("Login", t.getForm().getTextInput(UserUpdateAction::PARAMETER_LOGIN, _user->getLogin()));

			stream << t.title("Coordonnées");
			stream << t.cell("Prénom", t.getForm().getTextInput(UserUpdateAction::PARAMETER_SURNAME, _user->getSurname()));
			stream << t.cell("Nom", t.getForm().getTextInput(UserUpdateAction::PARAMETER_NAME, _user->getName()));
			stream << t.cell("Adresse", t.getForm().getTextAreaInput(UserUpdateAction::PARAMETER_ADDRESS, _user->getAddress(), 4, 50));
			stream << t.cell("Code postal", t.getForm().getTextInput(UserUpdateAction::PARAMETER_POSTAL_CODE, _user->getPostCode()));
			stream << t.cell("Ville", t.getForm().getTextInput(UserUpdateAction::PARAMETER_CITY, _user->getCityText()));
			stream << t.cell("Téléphone",t.getForm().getTextInput(UserUpdateAction::PARAMETER_PHONE, _user->getPhone()));
			stream << t.cell("E-mail",t.getForm().getTextInput(UserUpdateAction::PARAMETER_EMAIL, _user->getEMail()));

			stream << t.title("Droits");
	//		stream << t.cell("Connexion autorisée",t.getForm().getOuiNonRadioInput(UserUpdateAction::PARAMETER_AUTHORIZED_LOGIN, _user->getConnectionAllowed()));
			stream << t.cell("Auto réservation autorisée","");
			stream << t.close();

//			stream << "<h1>Trajets favoris</h1>";

			stream << "<h1>Historique / Réservations</h1>";

			ResaModule::DisplayResaDBLog(
				stream
				, env
				, PARAMETER_EVENT_DATE
				, searchRequest
				, cancelRequest
				, _requestParameters
				, false
			);
		}

		bool ResaCustomerAdmin::isAuthorized() const
		{
			return _request->isAuthorized<ResaRight>(READ, READ);
		}
		
		AdminInterfaceElement::PageLinks ResaCustomerAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == ResaCustomersAdmin::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY)
				links.push_back(currentPage.getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks ResaCustomerAdmin::getSubPages(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}


		std::string ResaCustomerAdmin::getTitle() const
		{
			return _user.get() ? _user->getFullName() : DEFAULT_TITLE;
		}

		std::string ResaCustomerAdmin::getParameterName() const
		{
			return _user.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string ResaCustomerAdmin::getParameterValue() const
		{
			return _user.get() ? Conversion::ToString(_user->getKey()) : string();
		}
	}
}
