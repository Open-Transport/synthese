
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

#include "31_resa/ResaCustomersAdmin.h"
#include "31_resa/ResaModule.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/Reservation.h"
#include "31_resa/ReservationTableSync.h"
#include "31_resa/ResaDBLog.h"
#include "31_resa/CancelReservationAction.h"
#include "31_resa/ReservationRoutePlannerAdmin.h"
#include "31_resa/ResaRight.h"

#include "05_html/HTMLTable.h"
#include "05_html/PropertiesHTMLTable.h"

#include "30_server/QueryString.h"
#include "30_server/ActionFunctionRequest.h"
#include "30_server/Request.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/UserUpdateAction.h"
#include "12_security/SecurityModule.h"

#include "13_dblog/DBLogEntry.h"
#include "13_dblog/DBLogEntryTableSync.h"

#include "04_time/DateTime.h"

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
		
		void ResaCustomerAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			try
			{
				_user = UserTableSync::Get(id);
			}
			catch (...)
			{
				throw AdminParametersException("Bad user id");
			}

			_displayCancelled = map.getBool(PARAMETER_DISPLAY_CANCELLED, false, false, FACTORY_KEY);
			_displayEvents = map.getBool(PARAMETER_DISPLAY_EVENTS, false, false, FACTORY_KEY);
			Date da = map.getDate(PARAMETER_EVENT_DATE, false, FACTORY_KEY);
			if (!da.isUnknown())
				_eventDate = da;

			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_EVENT_DATE, 50, false);
		}
		
		void ResaCustomerAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Rights
			bool writingRight(request->isAuthorized<ResaRight>(WRITE,WRITE));

			// Requests
			ActionFunctionRequest<UserUpdateAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<ResaCustomerAdmin>();
			updateRequest.setObjectId(request->getObjectId());

			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<ResaCustomerAdmin>();
			searchRequest.setObjectId(request->getObjectId());

			ActionFunctionRequest<CancelReservationAction,AdminRequest> cancelRequest(request);
			cancelRequest.getFunction()->setPage<ResaCustomerAdmin>();
			cancelRequest.setObjectId(request->getObjectId());

			FunctionRequest<AdminRequest> routeplannerRequest(request);
			routeplannerRequest.getFunction()->setPage<ReservationRoutePlannerAdmin>();
			routeplannerRequest.getFunction()->setParameter(ReservationRoutePlannerAdmin::PARAMETER_CUSTOMER_ID, Conversion::ToString(_user->getKey()));


			// Search
			vector<shared_ptr<DBLogEntry> > resats(DBLogEntryTableSync::search(
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
			));
			ResultHTMLTable::ResultParameters resultParameters;
			resultParameters.setFromResult(_requestParameters, resats);


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

			DateTime now(TIME_CURRENT);
			ResultHTMLTable::HeaderVector ht;
			ht.push_back(make_pair(PARAMETER_EVENT_DATE, "Date"));
			ht.push_back(make_pair(PARAMETER_TRAVEL_DATE, "Objet"));
			ht.push_back(make_pair(string(), "Description"));
			ht.push_back(make_pair(string(), "Opérateur"));
			ht.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable rt(ht, searchRequest.getHTMLForm(), _requestParameters, resultParameters);
			stream << rt.open();

			for (vector<shared_ptr<DBLogEntry> >::const_iterator itr(resats.begin()); itr != resats.end(); ++itr)
			{
				DBLogEntry::Content content((*itr)->getContent());
				ResaDBLog::_EntryType entryType(static_cast<ResaDBLog::_EntryType>(Conversion::ToInt(content[ResaDBLog::COL_TYPE])));
				shared_ptr<ReservationTransaction> tr;
				ReservationStatus status(NO_RESERVATION);
				const User* entryUser((*itr)->getUser());
				
				if (Conversion::ToLongLong(content[ResaDBLog::COL_RESA]) > 0)
				{
					tr = ReservationTransactionTableSync::GetUpdateable(Conversion::ToLongLong(content[ResaDBLog::COL_RESA]));
					ReservationTableSync::search(tr.get());
					status = tr->getStatus();
					cancelRequest.getAction()->setTransaction(tr);
				}

				if (entryType == ResaDBLog::CALL_ENTRY)
				{
					DateTime d(DateTime::FromSQLTimestamp(content[ResaDBLog::COL_DATE2]));

					stream << rt.row();
					stream << rt.col(1,string(),true) << (*itr)->getDate().toString();
					stream << rt.col(1,string(),true) << HTMLModule::getHTMLImage("phone.png","Appel");
					stream << rt.col(1,string(),true) << "APPEL";
					if (!d.isUnknown())
						stream << " jusqu'à " << d.toString() << " (" << (d.getSecondsDifference((*itr)->getDate())) << " s)";
					stream << rt.col(1,string(),true) << entryUser->getFullName();
					stream << rt.col(1,string(),true);
				}
				else
				{
					stream << rt.row();

					stream << rt.col() << (*itr)->getDate().toString();

					stream << rt.col();
					switch (entryType)
					{
					case ResaDBLog::RESERVATION_ENTRY:
						stream << HTMLModule::getHTMLImage("resa_compulsory.png", "Réservation");
						stream << HTMLModule::getHTMLImage(ResaModule::GetStatusIcon(status), tr->getFullStatusText());
						break;

					case ResaDBLog::CANCELLATION_ENTRY:
						stream << HTMLModule::getHTMLImage("bullet_delete.png", "Annulation de réservation");
						break;

					case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
						stream << HTMLModule::getHTMLImage("error.png", "Annulation de réservation hors délai");
						break;

					case ResaDBLog::NO_SHOW:
						stream << HTMLModule::getHTMLImage("exclamation.png", "Absence");
						break;
					}

					stream << rt.col();
					switch (entryType)
					{
					case ResaDBLog::RESERVATION_ENTRY:
						ResaModule::DisplayReservations(stream, tr.get());
						break;

					case ResaDBLog::CANCELLATION_ENTRY:
						stream << "ANNULATION de : ";
						ResaModule::DisplayReservations(stream, tr.get());
						break;

					case ResaDBLog::DELAYED_CANCELLATION_ENTRY:
						stream << "ANNULATION HORS DELAI de : ";
						ResaModule::DisplayReservations(stream, tr.get());
						break;

					case ResaDBLog::NO_SHOW:
						stream << "ABSENCE sur : ";
						ResaModule::DisplayReservations(stream, tr.get());
						break;
					}

					stream << rt.col() << entryUser->getFullName();


					stream << rt.col();
					if (writingRight)
					{
						switch(status)
						{
						case ReservationStatus::OPTION:
							stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler", "Etes-vous sûr de vouloir annuler la réservation ?", "bullet_delete.png");
							break;

						case ReservationStatus::TO_BE_DONE:
							stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler hors délai", "Etes-vous sûr de vouloir annuler la réservation (hors délai) ?", "error.png");
							break;

						case ReservationStatus::AT_WORK:
							stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Noter absence", "Etes-vous sûr de noter l'absence du client à l'arrêt ?", "exclamation.png");
							break;
						}
					}
				}
			}

			stream << rt.close();
		}

		bool ResaCustomerAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<ResaRight>(READ, READ);
		}
		
		AdminInterfaceElement::PageLinks ResaCustomerAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == ResaCustomersAdmin::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY)
				links.push_back(currentPage.getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks ResaCustomerAdmin::getSubPages(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
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
