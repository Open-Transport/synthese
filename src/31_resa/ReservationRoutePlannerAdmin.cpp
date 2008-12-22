
/** ReservationRoutePlannerAdmin class implementation.
	@file ReservationRoutePlannerAdmin.cpp
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

#include "ReservationRoutePlannerAdmin.h"

#include "31_resa/ResaModule.h"
#include "31_resa/ResaRight.h"
#include "31_resa/BookReservationAction.h"
#include "31_resa/ResaCustomerHtmlOptionListFunction.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/Reservation.h"
#include "31_resa/ReservationTableSync.h"

#include "30_server/QueryString.h"
#include "30_server/ActionFunctionRequest.h"

#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

#include "33_route_planner/RoutePlanner.h"

#include "05_html/SearchFormHTMLTable.h"

#include "15_env/Journey.h"
#include "15_env/ReservationRule.h"
#include "15_env/ServiceUse.h"
#include "15_env/Line.h"
#include "15_env/CommercialLine.h"
#include "15_env/LineStop.h"
#include "15_env/Road.h"
#include "15_env/RoadChunk.h"
#include "15_env/EnvModule.h"
#include "15_env/AccessParameters.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace env;
	using namespace time;
	using namespace html;
	using namespace routeplanner;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ReservationRoutePlannerAdmin>::FACTORY_KEY("ReservationRoutePlannerAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ReservationRoutePlannerAdmin>::ICON("arrow_switch.png");
		template<> const string AdminInterfaceElementTemplate<ReservationRoutePlannerAdmin>::DEFAULT_TITLE("Recherche d'itinéraires");
	}

	namespace resa
	{
		const std::string ReservationRoutePlannerAdmin::PARAMETER_START_CITY("sc");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_START_PLACE("sp");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_END_CITY("ec");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_END_PLACE("ep");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_DATE_TIME("dt");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_RESULTS_NUMBER("rn");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_DISABLED_PASSENGER("dp");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_DRT_ONLY("do");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_CUSTOMER_ID("cu");

		ReservationRoutePlannerAdmin::ReservationRoutePlannerAdmin()
			: AdminInterfaceElementTemplate<ReservationRoutePlannerAdmin>()
			, _resultsNumber(5)
			, _disabledPassenger(false)
			, _drtOnly(false)
			, _dateTime(TIME_CURRENT)
		{ }
		
		void ReservationRoutePlannerAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_startCity = map.getString(PARAMETER_START_CITY, false, FACTORY_KEY);
			_startPlace = map.getString(PARAMETER_START_PLACE, false, FACTORY_KEY);
			_endCity = map.getString(PARAMETER_END_CITY, false, FACTORY_KEY);
			_endPlace = map.getString(PARAMETER_END_PLACE, false, FACTORY_KEY);
			_dateTime = map.getDateTime(PARAMETER_DATE_TIME, false, FACTORY_KEY);
			if (_dateTime.isUnknown())
				_dateTime = DateTime(TIME_CURRENT);
			int i(map.getInt(PARAMETER_RESULTS_NUMBER, false, FACTORY_KEY));
			if (i > 0)
				_resultsNumber = i;
			_disabledPassenger = map.getBool(PARAMETER_DISABLED_PASSENGER, false, false, FACTORY_KEY);
			_drtOnly = map.getBool(PARAMETER_DRT_ONLY, false, false, FACTORY_KEY);

			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY));
			if (id != UNKNOWN_VALUE)
			{
				try
				{
					_confirmedTransaction = ReservationTransactionTableSync::GetEditable(id, _env);
					//ReservationTableSync::Search(_confirmedTransaction.get());
				}
				catch (...)
				{
					throw AdminParametersException("Reservation load error");
				}
			}
			
			id = map.getUid(PARAMETER_CUSTOMER_ID, false, FACTORY_KEY);
			if (id != UNKNOWN_VALUE)
			{
				try
				{
					_customer = UserTableSync::Get(id, _env);
				}
				catch (...)
				{
					throw AdminParametersException("No such User");
				}
			}
		}
		
		void ReservationRoutePlannerAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage<ReservationRoutePlannerAdmin>();

			ActionFunctionRequest<BookReservationAction,AdminRequest> resaRequest(request);
			resaRequest.getFunction()->setPage<ReservationRoutePlannerAdmin>();

			FunctionRequest<ResaCustomerHtmlOptionListFunction> customerSearchRequest(request);
			customerSearchRequest.getFunction()->setNumber(20);

			stream << HTMLModule::GetHTMLJavascriptOpen("resa.js");

			// Confirmation
			if (_confirmedTransaction.get())
			{
				stream << "<h1>Réservation confirmée</h1>";

				ResaModule::DisplayReservations(stream, _confirmedTransaction.get());
			}

			// Search form
			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune départ", st.getForm().getTextInput(PARAMETER_START_CITY, _startCity));
			stream << st.cell("Arrêt départ", st.getForm().getTextInput(PARAMETER_START_PLACE, _startPlace));
			stream << st.cell("Commune arrivée", st.getForm().getTextInput(PARAMETER_END_CITY, _endCity));
			stream << st.cell("Arrêt arrivée", st.getForm().getTextInput(PARAMETER_END_PLACE, _endPlace));
			stream << st.cell("Date/Heure", st.getForm().getCalendarInput(PARAMETER_DATE_TIME, _dateTime));
			stream << st.cell("Nombre réponses", st.getForm().getSelectNumberInput(PARAMETER_RESULTS_NUMBER, 1, 99, _resultsNumber, 1, "(illimité)"));
			stream << st.cell("PMR", st.getForm().getOuiNonRadioInput(PARAMETER_DISABLED_PASSENGER, _disabledPassenger));
			stream << st.cell("TAD seulement", st.getForm().getOuiNonRadioInput(PARAMETER_DRT_ONLY, _drtOnly));
			stream << st.close();
			stream << st.getForm().setFocus(PARAMETER_START_CITY);

			// No calculation without cities
			if (_startCity.empty() || _endCity.empty())
				return;

			stream << "<h1>Résultats</h1>";

			DateTime endDate(_dateTime);
			endDate++;

			DateTime now(TIME_CURRENT);

			// Route planning
			AccessParameters ap(false, NULL, _disabledPassenger, false, _drtOnly ? logic::tribool(true) : logic::indeterminate);
			const Place* startPlace(EnvModule::FetchPlace(_startCity, _startPlace));
			const Place* endPlace(EnvModule::FetchPlace(_endCity, _endPlace));
			stringstream trace;
			RoutePlanner r(
				startPlace
				, endPlace
				, ap
				, PlanningOrder()
				, _dateTime
				, endDate
				, _resultsNumber
			);
			const RoutePlanner::Result& jv(r.computeJourneySheetDepartureArrival());

			if (jv.journeys.empty())
			{
				stream << "Aucun résultat trouvé de " << startPlace->getFullName() << " à " << endPlace->getFullName();
				return;
			}

			HTMLTable::ColsVector v;
			v.push_back("Départ<br />" + startPlace->getFullName());
			v.push_back("Ligne");
			v.push_back("Arrivée");
			v.push_back("Correspondance");
			v.push_back("Départ");
			v.push_back("Ligne");
			v.push_back("Arrivée<br />" + endPlace->getFullName());
			HTMLTable t(v,"adminresults");

			// Reservation
			bool withReservation(false);
			for (JourneyBoardJourneys::const_iterator it(jv.journeys.begin()); it != jv.journeys.end(); ++it)
				if ((*it)->getReservationCompliance() && (*it)->getReservationDeadLine() > now)
				{
					withReservation = true;
					resaRequest.getAction()->setJourney(**it);
					break;
				}
			HTMLForm rf(resaRequest.getHTMLForm("resa"));
			if (withReservation)
				stream << rf.open();

			// Solutions display loop
			int solution(1);
			stream << t.open();
			for (JourneyBoardJourneys::const_iterator it(jv.journeys.begin()); it != jv.journeys.end(); ++it)
			{
				stream << t.row();
				stream << t.col(7, string(), true);
				if ((*it)->getReservationCompliance() && (*it)->getReservationDeadLine() > now)
				{
					withReservation = true;
					stream << rf.getRadioInput(BookReservationAction::PARAMETER_DATE_TIME, (*it)->getDepartureTime(), (solution==1) ? (*it)->getDepartureTime() : DateTime(UNKNOWN_VALUE), " Solution "+Conversion::ToString(solution));
				}
				else
					stream << "Solution " << solution;
				++solution;

				// Departure time
				Journey::ServiceUses::const_iterator its((*it)->getServiceUses().begin());

				if ((*it)->getContinuousServiceRange() > 1)
				{
					DateTime endRange(its->getDepartureDateTime());
					endRange += (*it)->getContinuousServiceRange();
					stream << " - Service continu jusqu'à " << endRange.toString();
				}
				if ((*it)->getReservationCompliance() == true)
				{
					stream << " - " << HTMLModule::getHTMLImage("resa_compulsory.png", "Réservation obligatoire") << " Réservation obligatoire avant le " << (*it)->getReservationDeadLine().toString();
				}
				if ((*it)->getReservationCompliance() == boost::logic::indeterminate)
				{
					stream << " - " << HTMLModule::getHTMLImage("resa_optional.png", "Réservation facultative") << " Réservation facultative avant le " << (*it)->getReservationDeadLine().toString();
				}

				stream << t.row();
				stream << t.col() << its->getDepartureDateTime().toString();

				// Line
				const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
				const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
				stream << t.col(1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
				stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getName());

				// Transfers
				if (its == (*it)->getServiceUses().end() -1)
				{
					stream << t.col(4) << "(trajet direct)";
				}
				else
				{
					while(true)
					{
						// Arrival
						stream << t.col() << its->getArrivalDateTime().toString();

						// Place
						stream << t.col() << its->getArrivalEdge()->getPlace()->getFullName();

						// Next service use
						++its;

						// Departure
						stream << t.col() << its->getDepartureDateTime().toString();

						// Line
						const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
						const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
						stream << t.col(1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
						stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getName());

						// Exit if last service use
						if (its == (*it)->getServiceUses().end() -1)
							break;

						// Empty final arrival col
						stream << t.col();

						// New row and empty origin departure cols;
						stream << t.row();
						stream << t.col();
						stream << t.col();
					}
				}

				// Final arrival
				stream << t.col() << its->getArrivalDateTime().toString();
			}
			stream << t.close();

			stream << "<h1>Réservation</h1>";

			if (!withReservation)
				stream << "<p>Aucune solution proposée n'est ouverte à la réservation.</p>";
			else
			{
				stream << rf.setFocus(BookReservationAction::PARAMETER_DATE_TIME, 0);

				HTMLTable rt(2,"propertysheet");
				stream << rt.open();

				if (_customer.get())
				{
					stream << rt.row();
					stream << rt.col() << "Client";
					stream << rt.col() << _customer->getFullName() << " (" << _customer->getPhone() << ")";
					rf.addHiddenField(BookReservationAction::PARAMETER_CUSTOMER_ID, Conversion::ToString(_customer->getKey()));
				}
				else
				{
					stream << rt.row();
					stream << rt.col() << "Recherche client";
					stream << rt.col() << "Nom : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_NAME, string());
					stream << "	Prénom : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_SURNAME, string());

					stream << HTMLModule::GetHTMLJavascriptOpen();
					stream
						<< "document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_NAME) << "').onkeyup = "
						<< "function(){ programCustomerUpdate("
						<< "'" << rf.getName() << "'"
						<< ",'" << BookReservationAction::PARAMETER_CREATE_CUSTOMER << "'"
						<< ",'ie_bug_curstomer_div'"
						<< ",'" << BookReservationAction::PARAMETER_CUSTOMER_ID << "'"
						<< ",'" << customerSearchRequest.getURL() 
						<< "&" << ResaCustomerHtmlOptionListFunction::PARAMETER_NAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_NAME) << "').value"
						<< "+'&" << ResaCustomerHtmlOptionListFunction::PARAMETER_SURNAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_SURNAME) << "').value"
						<< "); };";
					stream << "document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_SURNAME) << "').onkeyup = "
						<< "function(){ programCustomerUpdate(" 
						<< "'" << rf.getName() << "'"
						<< ",'" << BookReservationAction::PARAMETER_CREATE_CUSTOMER << "'"
						<< ",'ie_bug_curstomer_div'"
						<< ",'" << BookReservationAction::PARAMETER_CUSTOMER_ID << "'"
						<< ",'" << customerSearchRequest.getURL() 
						<< "&" << ResaCustomerHtmlOptionListFunction::PARAMETER_NAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_NAME) << "').value"
						<< "+'&" << ResaCustomerHtmlOptionListFunction::PARAMETER_SURNAME <<"='+document.getElementById('" << rf.getFieldId(BookReservationAction::PARAMETER_CUSTOMER_SURNAME) << "').value"
						<< "); };";
					stream << HTMLModule::GetHTMLJavascriptClose();

					stream << rt.row();
					stream << rt.col() << "Client";
					stream << rt.col() << rf.getRadioInput(BookReservationAction::PARAMETER_CREATE_CUSTOMER, true, true, "Nouveau client");
					stream << " : Téléphone : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_PHONE, string());
					stream << "	E-mail : " << rf.getTextInput(BookReservationAction::PARAMETER_CUSTOMER_EMAIL, string());
					stream << "<br />" << rf.getRadioInput(BookReservationAction::PARAMETER_CREATE_CUSTOMER, false, true, "Client existant", true);
					stream << "<span id=\"ie_bug_curstomer_div\"></span>";
				}
				
				
				stream << rt.row();
				stream << rt.col() << "Nombre de places";
				stream << rt.col() << rf.getTextInput(BookReservationAction::PARAMETER_SEATS_NUMBER, "1");

				stream << rt.row();
				stream << rt.col(2, string(), true) << rf.getSubmitButton("Réserver");

				stream << rt.close() << rf.close();
			}
		}

		bool ReservationRoutePlannerAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}
		
		AdminInterfaceElement::PageLinks ReservationRoutePlannerAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == ResaModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks ReservationRoutePlannerAdmin::getSubPages(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}

		bool ReservationRoutePlannerAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}
	}
}
