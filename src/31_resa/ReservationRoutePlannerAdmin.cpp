
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
#include "GeographyModule.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "BookReservationAction.h"
#include "ResaCustomerHtmlOptionListFunction.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "NamedPlace.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"

#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"

#include "RoutePlanner.h"

#include "SearchFormHTMLTable.h"
#include "PTConstants.h"
#include "Journey.h"
#include "ServiceUse.h"
#include "Line.h"
#include "CommercialLine.h"
#include "LineStop.h"
#include "Road.h"
#include "RoadChunk.h"
#include "EnvModule.h"
#include "AccessParameters.h"
#include "RoadPlace.h"
#include "User.h"
#include "UserTableSync.h"

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
	using namespace graph;
	using namespace road;
	using namespace geography;

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
		
		void ReservationRoutePlannerAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool doDisplayPreparationActions,
				bool objectWillBeCreatedLater
		){
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

			uid id(map.getUid(Request::PARAMETER_OBJECT_ID, false, FACTORY_KEY));
			if (id != UNKNOWN_VALUE)
			{
				try
				{
					_confirmedTransaction = ReservationTransactionTableSync::GetEditable(id, _getEnv());
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
					_customer = UserTableSync::Get(id, _getEnv());
				}
				catch (...)
				{
					throw AdminParametersException("No such User");
				}
			}
		}
		
		
		
		server::ParametersMap ReservationRoutePlannerAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAMETER_START_CITY, _startCity);
			m.insert(PARAMETER_START_PLACE, _startPlace);
			m.insert(PARAMETER_END_CITY, _endCity);
			m.insert(PARAMETER_END_PLACE, _endPlace);
			m.insert(PARAMETER_DATE_TIME, _dateTime);
			m.insert(PARAMETER_RESULTS_NUMBER, _resultsNumber);
			m.insert(PARAMETER_DISABLED_PASSENGER, _disabledPassenger);
			m.insert(PARAMETER_DRT_ONLY, _drtOnly);
			if(_customer.get())
			{
				m.insert(PARAMETER_CUSTOMER_ID, _customer->getKey());
			}
			return m;
		}

		
		
		void ReservationRoutePlannerAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			AdminFunctionRequest<ReservationRoutePlannerAdmin> searchRequest(_request);

			AdminActionFunctionRequest<BookReservationAction,ReservationRoutePlannerAdmin> resaRequest(
				_request
			);
			resaRequest.setActionWillCreateObject();

			FunctionRequest<ResaCustomerHtmlOptionListFunction> customerSearchRequest(&_request);
			customerSearchRequest.getFunction()->setNumber(20);

			stream << HTMLModule::GetHTMLJavascriptOpen("resa.js");

			// Confirmation
			if (_confirmedTransaction.get())
			{
				stream << "<h1>Réservation confirmée</h1>";

				ResaModule::DisplayReservations(stream, *_confirmedTransaction);
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

			if(_confirmedTransaction.get())
				return;

			stream << "<h1>Résultats</h1>";

			DateTime endDate(_dateTime);
			endDate++;

			DateTime now(TIME_CURRENT);

			// Route planning
			AccessParameters ap(
				_disabledPassenger ? USER_HANDICAPPED : USER_PEDESTRIAN,
				_drtOnly
			);
			const Place* startPlace(GeographyModule::FetchPlace(_startCity, _startPlace));
			const Place* endPlace(GeographyModule::FetchPlace(_endCity, _endPlace));
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
				stream << "Aucun résultat trouvé de " << (
						dynamic_cast<const NamedPlace*>(startPlace) ?
						dynamic_cast<const NamedPlace*>(startPlace)->getFullName() :
						dynamic_cast<const City*>(startPlace)->getName()
					) << " à " << (
						dynamic_cast<const NamedPlace*>(endPlace) ?
						dynamic_cast<const NamedPlace*>(endPlace)->getFullName() :
						dynamic_cast<const City*>(endPlace)->getName()
					);
				return;
			}

			HTMLTable::ColsVector v;
			v.push_back("Départ<br />" + (
					dynamic_cast<const NamedPlace*>(startPlace) ?
					dynamic_cast<const NamedPlace*>(startPlace)->getFullName() :
					dynamic_cast<const City*>(startPlace)->getName()
			)	);
			v.push_back("Ligne");
			v.push_back("Arrivée");
			v.push_back("Correspondance");
			v.push_back("Départ");
			v.push_back("Ligne");
			v.push_back("Arrivée<br />" + (
					dynamic_cast<const NamedPlace*>(endPlace) ?
					dynamic_cast<const NamedPlace*>(endPlace)->getFullName() :
					dynamic_cast<const City*>(endPlace)->getName()
			)	);
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
				stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName());

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
						stream <<
							t.col() <<
							static_cast<const PublicTransportStopZoneConnectionPlace*>(
								its->getArrivalEdge()->getHub()
							)->getFullName()
						;

						// Next service use
						++its;

						// Departure
						stream << t.col() << its->getDepartureDateTime().toString();

						// Line
						const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
						const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
						stream << t.col(1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
						stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName());

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

		bool ReservationRoutePlannerAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			return _request.isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}
		
		AdminInterfaceElement::PageLinks ReservationRoutePlannerAdmin::getSubPagesOfModule(
				const std::string& moduleKey,
				boost::shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(moduleKey == ResaModule::FACTORY_KEY)
			{
				if(dynamic_cast<const ReservationRoutePlannerAdmin*>(currentPage.get()))
				{
					AddToLinks(links, currentPage);
				}
				else
				{
					AddToLinks(links, getNewPage());
				}
			}
			return links;
		}
		
		bool ReservationRoutePlannerAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage ) const
		{
			return true;
		}
		
		void ReservationRoutePlannerAdmin::setCustomer(boost::shared_ptr<const User> value)
		{
			_customer = value;
		}
	}
}
