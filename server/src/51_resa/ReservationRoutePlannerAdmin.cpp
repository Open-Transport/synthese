
/** ReservationRoutePlannerAdmin class implementation.
	@file ReservationRoutePlannerAdmin.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "AlgorithmLogger.hpp"
#include "GeographyModule.h"
#include "PTServiceConfig.hpp"
#include "ResaModule.h"
#include "ResaRight.h"
#include "User.h"
#include "BookReservationAction.h"
#include "NamedPlace.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "Profile.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "RoadModule.h"
#include "PTTimeSlotRoutePlanner.h"
#include "PTRoutePlannerResult.h"
#include "SearchFormHTMLTable.h"
#include "PTConstants.h"
#include "Journey.h"
#include "ServicePointer.h"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "LineStop.h"
#include "Road.h"
#include "RoadChunk.h"
#include "PTModule.h"
#include "AccessParameters.h"
#include "RoadPlace.h"
#include "Language.hpp"

#include "CityListFunction.h"
#include "PlacesListFunction.h"

#include <geos/geom/Point.h>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;


namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace pt;
	using namespace html;
	using namespace pt_journey_planner;
	using namespace algorithm;
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
		const std::string ReservationRoutePlannerAdmin::PARAMETER_DATE("da");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_TIME("ti");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_DISABLED_PASSENGER("dp");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_WITHOUT_TRANSFER("wt");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_PLANNING_ORDER("po");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_APPROACH_SPEED("apsp");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_ENABLED_PEDESTRIAN("epe");
		const std::string ReservationRoutePlannerAdmin::PARAMETER_IGNORE_RESERVATION("ir");



		ReservationRoutePlannerAdmin::ReservationRoutePlannerAdmin(
		):	AdminInterfaceElementTemplate<ReservationRoutePlannerAdmin>(),
			_dateTime(second_clock::local_time()),
			_disabledPassenger(false),
			_withoutTransfer(false),
			_planningOrder(DEPARTURE_FIRST),
			_approachSpeed(1.111),//0.833 = 3km/h, 1.111 = 4km/h
			_enabledPedestrian(false),
			_ignoreReservation(false)
		{
			_effectiveApproachSpeed = _approachSpeed;
		}



		void ReservationRoutePlannerAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_planningOrder = static_cast<PlanningOrder>(map.getDefault<int>(PARAMETER_PLANNING_ORDER));
			_startCity = map.getDefault<string>(PARAMETER_START_CITY);
			_startPlace = map.getDefault<string>(PARAMETER_START_PLACE);
			_endCity = map.getDefault<string>(PARAMETER_END_CITY);
			_endPlace = map.getDefault<string>(PARAMETER_END_PLACE);
			if(!map.getDefault<string>(PARAMETER_DATE).empty())
			{
				_dateTime = ptime(
					from_string(map.get<string>(PARAMETER_DATE)),
					map.getDefault<string>(PARAMETER_TIME).empty() ?
					minutes(0) :
					duration_from_string(map.get<string>(PARAMETER_TIME))
				);
			}
			_disabledPassenger = map.getDefault<bool>(PARAMETER_DISABLED_PASSENGER, false);
			_withoutTransfer = map.getDefault<bool>(PARAMETER_WITHOUT_TRANSFER, false);
			_enabledPedestrian = map.getDefault<bool>(PARAMETER_ENABLED_PEDESTRIAN, false);
			_ignoreReservation = map.getDefault<bool>(PARAMETER_IGNORE_RESERVATION, false);

			if(map.getOptional<double>(PARAMETER_APPROACH_SPEED))
			{
				_approachSpeed = map.get<double>(PARAMETER_APPROACH_SPEED);
			}
			if(_enabledPedestrian)
			{
				_effectiveApproachSpeed = _approachSpeed;
			}
			else //If pedestrian disabled, then set pedestrian speed to 0 to avoid using roads
			{
				_effectiveApproachSpeed = 0;
			}

			setBaseReservationFromParametersMap(*_env, map);
		}



		ParametersMap ReservationRoutePlannerAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.merge(getBaseReservationParametersMap());
			m.insert(PARAMETER_START_CITY, _startCity);
			m.insert(PARAMETER_START_PLACE, _startPlace);
			m.insert(PARAMETER_END_CITY, _endCity);
			m.insert(PARAMETER_END_PLACE, _endPlace);
			m.insert(PARAMETER_DATE, _dateTime.date());
			m.insert(PARAMETER_TIME, _dateTime.time_of_day());
			m.insert(PARAMETER_DISABLED_PASSENGER, _disabledPassenger);
			m.insert(PARAMETER_WITHOUT_TRANSFER, _withoutTransfer);
			m.insert(PARAMETER_PLANNING_ORDER, static_cast<int>(_planningOrder));
			m.insert(PARAMETER_APPROACH_SPEED, _approachSpeed);
			m.insert(PARAMETER_IGNORE_RESERVATION, _ignoreReservation);
			return m;
		}



		void ReservationRoutePlannerAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const {

			const synthese::Language& language(
				_request.getUser()->getLanguage() ?
				*_request.getUser()->getLanguage() :
				synthese::Language::GetLanguageFromIso639_1Code("fr")
			);

			vector<pair<optional<string>, string> > dates;
			vector<pair<optional<string>, string> > hours;
			{
				date date(day_clock::local_day());
				for(size_t i=0; i<140; ++i)
				{
					dates.push_back(
						make_pair(
							to_iso_extended_string(date),
							language.getWeekDayName(date.day_of_week()) +" "+ to_simple_string(date)
						)	);
					date += days(1);
				}
				for(size_t i=0; i<24; ++i)
				{
					hours.push_back(make_pair(lexical_cast<string>(i) +":00", lexical_cast<string>(i) +":00"));
				}
			}

			AdminActionFunctionRequest<BookReservationAction,ReservationRoutePlannerAdmin> resaRequest(
				_request,
				*this
			);
			resaRequest.setActionWillCreateObject();
			if(ResaModule::GetJourneyPlannerWebsite())
			{
				resaRequest.getAction()->setSite(Env::GetOfficialEnv().getSPtr(ResaModule::GetJourneyPlannerWebsite()));
			}
			resaRequest.getAction()->setIgnoreReservationRules(_ignoreReservation);
			resaRequest.getAction()->setApproachSpeed(_effectiveApproachSpeed);

			// Confirmation
			if (_confirmedTransaction.get())
			{
				stream << "<h1>Réservation confirmée</h1>";

				ResaModule::DisplayReservations(stream, *_confirmedTransaction, language);
			}

			// Search form
			stream << "<h1>Recherche</h1>";

			boost::shared_ptr<Place> startPlace;
			boost::shared_ptr<Place> endPlace;

			if (!_startCity.empty() && !_endCity.empty())
			{
				if(ResaModule::GetJourneyPlannerWebsite())
				{
					startPlace = ResaModule::GetJourneyPlannerWebsite()->fetchPlace(_startCity, _startPlace);
					endPlace = ResaModule::GetJourneyPlannerWebsite()->fetchPlace(_endCity, _endPlace);
				}
				else
				{
					startPlace = RoadModule::FetchPlace(_startCity, _startPlace);
					endPlace = RoadModule::FetchPlace(_endCity, _endPlace);
				}
			}

			AdminFunctionRequest<ReservationRoutePlannerAdmin> searchRequest(_request, *this);
			ParametersMap tpm(searchRequest.getFunction()->getTemplateParameters());
			tpm.remove(Request::PARAMETER_OBJECT_ID);
			searchRequest.getFunction()->setTemplateParameters(tpm);

			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Commune départ", st.getForm().getTextInputAutoCompleteFromService(
						PARAMETER_START_CITY,
						startPlace ?
						lexical_cast<string>(dynamic_cast<City*>(startPlace.get()) ? dynamic_cast<City*>(startPlace.get())->getKey() : dynamic_cast<NamedPlace*>(startPlace.get())->getCity()->getKey()) :
						_startCity,
						startPlace ?
						(dynamic_cast<City*>(startPlace.get()) ? dynamic_cast<City*>(startPlace.get())->getName() : dynamic_cast<NamedPlace*>(startPlace.get())->getCity()->getName()) :
						_startCity,
						pt_website::CityListFunction::FACTORY_KEY,
						pt_website::CityListFunction::DATA_CITIES,
						pt_website::CityListFunction::DATA_CITY,
						string(),string(),
						false, false, false
				)	);
			stream << st.cell("Arrêt départ", st.getForm().getTextInputAutoCompleteFromService(
						PARAMETER_START_PLACE,
						startPlace ?
						(dynamic_cast<City*>(startPlace.get()) ? string() : lexical_cast<string>(dynamic_cast<NamedPlace*>(startPlace.get())->getKey())) :
						_startPlace,
						startPlace ?
						(dynamic_cast<City*>(startPlace.get()) ? string() : dynamic_cast<NamedPlace*>(startPlace.get())->getName()) :
						_startPlace,
						pt_website::PlacesListFunction::FACTORY_KEY,
						pt_website::PlacesListFunction::DATA_PLACES,
						pt_website::PlacesListFunction::DATA_PLACE,
						"ct",PARAMETER_START_CITY,
						false, false, false
				)	);
			stream << st.cell("Commune arrivée", st.getForm().getTextInputAutoCompleteFromService(
						PARAMETER_END_CITY,
						endPlace ?
						lexical_cast<string>(dynamic_cast<City*>(endPlace.get()) ? dynamic_cast<City*>(endPlace.get())->getKey() : dynamic_cast<NamedPlace*>(endPlace.get())->getCity()->getKey()) :
						_endCity,
						endPlace ?
						(dynamic_cast<City*>(endPlace.get()) ? dynamic_cast<City*>(endPlace.get())->getName() : dynamic_cast<NamedPlace*>(endPlace.get())->getCity()->getName()) :
						_endCity,
						pt_website::CityListFunction::FACTORY_KEY,
						pt_website::CityListFunction::DATA_CITIES,
						pt_website::CityListFunction::DATA_CITY,
						string(),string(),
						false, false, false
				)	);
			stream << st.cell("Arrêt arrivée", st.getForm().getTextInputAutoCompleteFromService(
						PARAMETER_END_PLACE,
						endPlace ?
						(dynamic_cast<City*>(endPlace.get()) ? string() : lexical_cast<string>(dynamic_cast<NamedPlace*>(endPlace.get())->getKey())) :
						_endPlace,
						endPlace ?
						(dynamic_cast<City*>(endPlace.get()) ? string() : dynamic_cast<NamedPlace*>(endPlace.get())->getName()) :
						_endPlace,
						pt_website::PlacesListFunction::FACTORY_KEY,
						pt_website::PlacesListFunction::DATA_PLACES,
						pt_website::PlacesListFunction::DATA_PLACE,
						"ct",PARAMETER_END_CITY,
						false, false, false
				)	);
			stream << st.cell(
				"Date",
				st.getForm().getSelectInput(
					PARAMETER_DATE,
					dates,
					optional<string>(to_iso_extended_string(_dateTime.date()))
			)	);
			stream << st.cell(
				"Heure",
				st.getForm().getSelectInput(
					PARAMETER_TIME,
					hours,
					optional<string>(lexical_cast<string>(_dateTime.time_of_day().hours())+":00")
			)	);
			stream << st.cell("PMR", st.getForm().getOuiNonRadioInput(PARAMETER_DISABLED_PASSENGER, _disabledPassenger));
			stream << st.cell("Sans correspondance", st.getForm().getOuiNonRadioInput(PARAMETER_WITHOUT_TRANSFER, _withoutTransfer));
			stream << st.cell("Avec trajet piéton", st.getForm().getOuiNonRadioInput(PARAMETER_ENABLED_PEDESTRIAN, _enabledPedestrian));
			stream << st.cell("Ignorer délai résa", st.getForm().getOuiNonRadioInput(PARAMETER_IGNORE_RESERVATION, _ignoreReservation));
			stream << st.close();
			stream << st.getForm().setFocus(PARAMETER_START_CITY);

			// No calculation without cities
			if(	_startCity.empty() ||
				_endCity.empty() ||
				!startPlace.get() ||
				!endPlace.get()
			){
				return;
			}

			TimeSlotRoutePlanner::Result dummy;
			PTRoutePlannerResult jv(startPlace.get(), endPlace.get(), false, dummy);
			ptime now(second_clock::local_time());

			if(!_confirmedTransaction.get())
			{
				ptime maxDepartureTime(_dateTime);
				if(_planningOrder == DEPARTURE_FIRST)
				{
					maxDepartureTime += days(1);
				}
				else
				{
					maxDepartureTime -= days(1);
				}
				ptime maxArrivalTime(maxDepartureTime);
				if(_planningOrder == DEPARTURE_FIRST)
				{
					maxArrivalTime += days(1);
					if(	startPlace->getPoint().get() &&
						!startPlace->getPoint()->isEmpty() &&
						endPlace->getPoint().get() &&
						!endPlace->getPoint()->isEmpty()
					){
							maxArrivalTime += minutes(2 * static_cast<int>(startPlace->getPoint()->distance(endPlace->getPoint().get()) / 1000));
					}
				}
				else
				{
					maxArrivalTime -= days(1);
					if(	startPlace->getPoint().get() &&
						!startPlace->getPoint()->isEmpty() &&
						endPlace->getPoint().get() &&
						!endPlace->getPoint()->isEmpty()
					){
						maxArrivalTime -= minutes(2 * static_cast<int>(startPlace->getPoint()->distance(endPlace->getPoint().get()) / 1000));
					}
				}


				// Route planning
				AccessParameters ap(
					_disabledPassenger ? USER_HANDICAPPED : USER_PEDESTRIAN,
					false, false, 1000, posix_time::minutes(23), 1.111)
				;
				if(ResaModule::GetJourneyPlannerWebsite())
				{
					ap = ResaModule::GetJourneyPlannerWebsite()->getAccessParameters(
						_disabledPassenger ? USER_HANDICAPPED : USER_PEDESTRIAN,
						AccessParameters::AllowedPathClasses(),
						AccessParameters::AllowedNetworks()
					);
				}
				ap.setApproachSpeed(_effectiveApproachSpeed);

				ap.setMaxtransportConnectionsCount(
					_withoutTransfer ? (size_t)(0) : optional<size_t>()
				);
				resaRequest.getAction()->setAccessParameters(ap);
				stringstream trace;
				AlgorithmLogger logger;
				PTTimeSlotRoutePlanner r(
					startPlace.get(),
					endPlace.get(),
					_planningOrder == DEPARTURE_FIRST ? _dateTime : maxArrivalTime,
					_planningOrder == DEPARTURE_FIRST ? maxDepartureTime : _dateTime,
					_planningOrder == DEPARTURE_FIRST ? _dateTime : maxDepartureTime,
					_planningOrder == DEPARTURE_FIRST ? maxArrivalTime : _dateTime,
					5,
					ap,
					_planningOrder,
					_ignoreReservation,
					logger
				);
				jv = r.run();
			}

			stream << "<h1>Liens</h1><p>";
			ptime date(_dateTime);
			date -= days(1);
			searchRequest.getPage()->_dateTime = date;
			stream << HTMLModule::getLinkButton(searchRequest.getURL(), "Jour précédent", string(), "/admin/img/rewind_blue.png") << " ";

			if(!jv.getJourneys().empty())
			{
				PTRoutePlannerResult::Journeys::const_iterator it(jv.getJourneys().begin());
				date = it->getFirstArrivalTime();
				date -= minutes(1);
				searchRequest.getPage()->_dateTime = date;
				searchRequest.getPage()->_planningOrder = ARRIVAL_FIRST;

				stream << HTMLModule::getLinkButton(searchRequest.getURL(), "Solutions précédentes", string(), "/admin/img/resultset_previous.png") << " ";
			}

			searchRequest.getPage()->_planningOrder = DEPARTURE_FIRST;
			searchRequest.getPage()->_dateTime = _dateTime;
			searchRequest.getPage()->_startCity = _endCity;
			searchRequest.getPage()->_startPlace = _endPlace;
			searchRequest.getPage()->_endCity = _startCity;
			searchRequest.getPage()->_endPlace = _startPlace;
			stream << HTMLModule::getLinkButton(searchRequest.getURL(), "Trajet retour", string(), "/admin/img/arrow_undo.png") << " ";
			searchRequest.getPage()->_startCity = _startCity;
			searchRequest.getPage()->_startPlace = _startPlace;
			searchRequest.getPage()->_endCity = _endCity;
			searchRequest.getPage()->_endPlace = _endPlace;

			if(!jv.getJourneys().empty())
			{
				PTRoutePlannerResult::Journeys::const_iterator it(jv.getJourneys().end() - 1);
				date = it->getFirstDepartureTime();
				date += minutes(1);
				searchRequest.getPage()->_dateTime = date;
				stream << HTMLModule::getLinkButton(searchRequest.getURL(), "Solutions suivantes", string(), "/admin/img/resultset_next.png") << " ";

			}

			date = _dateTime;
			date += days(1);
			searchRequest.getPage()->_dateTime = date;
			stream << HTMLModule::getLinkButton(searchRequest.getURL(), "Jour suivant", string(), "/admin/img/forward_blue.png");

			stream << "</p>";

			if(_confirmedTransaction.get()) return;

			stream << "<h1>Résultats</h1>";

			// Reservation
			bool withReservation(false);
			for (PTRoutePlannerResult::Journeys::const_iterator it(jv.getJourneys().begin()); it != jv.getJourneys().end(); ++it)
			{
				if(	it->getReservationCompliance(_ignoreReservation) &&
					(_ignoreReservation || it->getReservationDeadLine() > now)
				){
					withReservation = true;
					resaRequest.getAction()->setJourney(*it);
					resaRequest.getAction()->getJourneyPlanner().setOriginCityText(_startCity);
					resaRequest.getAction()->getJourneyPlanner().setOriginPlaceText(_startPlace);
					resaRequest.getAction()->getJourneyPlanner().setDestinationCityText(_endCity);
					resaRequest.getAction()->getJourneyPlanner().setDestinationPlaceText(_endPlace);
					break;
				}
			}
			HTMLForm rf(resaRequest.getHTMLForm("resa"));
			if (withReservation)
			{
				stream << rf.open();
			}

			jv.displayHTMLTable(
				stream,
				rf,
				Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME,
				_ignoreReservation
			);

			if(jv.getJourneys().empty())
			{
				return;
			}

			stream << "<h1>Réservation</h1>";

			if (!withReservation)
			{
				stream << "<p>Aucune solution proposée n'est ouverte à la réservation.</p>";
			}
			else
			{
				stream <<
					rf.setFocus(
						Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME,
						0
					)
				;

				displayReservationForm(
					stream,
					rf,
					_request
				);
				stream << rf.close();
			}
		}



		bool ReservationRoutePlannerAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL);
		}



		AdminInterfaceElement::PageLinks ReservationRoutePlannerAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(	dynamic_cast<const ResaModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewCopiedPage());
			}
			return links;
		}



		bool ReservationRoutePlannerAdmin::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			return true;
		}
}	}
