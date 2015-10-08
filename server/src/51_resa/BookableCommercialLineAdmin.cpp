
/** BookableCommercialLineAdmin class implementation.
	@file BookableCommercialLineAdmin.cpp
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

#include "BookableCommercialLineAdmin.h"

#include "FreeDRTAreaTableSync.hpp"
#include "FreeDRTBookingAdmin.hpp"
#include "FreeDRTTimeSlotTableSync.hpp"
#include "StopPoint.hpp"
#include "UserTableSync.h"
#include "ResaModule.h"
#include "ServiceReservations.h"
#include "ReservationTransactionTableSync.h"
#include "ReservationTableSync.h"
#include "ReservationTransaction.h"
#include "Reservation.h"
#include "CancelReservationAction.h"
#include "ResaCustomerAdmin.h"
#include "ResaRight.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStop.h"
#include "LineStopTableSync.h"
#include "SearchFormHTMLTable.h"
#include "CommercialLine.h"
#include "AdvancedSelectTableSync.h"
#include "CommercialLineTableSync.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "RequestException.h"
#include "StaticActionFunctionRequest.h"
#include "Profile.h"
#include "AdminParametersException.h"
#include "ModuleAdmin.h"
#include "AdminInterfaceElement.h"
#include "DRTArea.hpp"
#include "ReservationAdmin.hpp"
#include "ReservationUpdateAction.hpp"
#include "VehicleTableSync.hpp"

#include <map>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

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
	using namespace html;
	using namespace security;
	using namespace graph;
	using namespace geography;
	using namespace pt;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, BookableCommercialLineAdmin>::FACTORY_KEY = "BookableCommercialLine";
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLineAdmin>::ICON = "chart_line.png";
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLineAdmin>::DEFAULT_TITLE = "Ligne inconnue";
	}

	namespace resa
	{
		const string BookableCommercialLineAdmin::PARAMETER_DATE = "da";
		const string BookableCommercialLineAdmin::PARAMETER_DISPLAY_CANCELLED = "dc";
		const string BookableCommercialLineAdmin::PARAMETER_SERVICE = "se";

		const string BookableCommercialLineAdmin::TAB_RESAS = "resas";
		const string BookableCommercialLineAdmin::TAB_FREE_DRT = "free_drt";



		BookableCommercialLineAdmin::BookableCommercialLineAdmin()
			: AdminInterfaceElementTemplate<BookableCommercialLineAdmin>(),
			_displayCancelled(false),
			_date(not_a_date_time),
			_hideOldServices(false)
		{ }



		void BookableCommercialLineAdmin::setFromParametersMap(
			const ParametersMap& map
		){

			// Date
			try
			{
				if(!map.getDefault<string>(PARAMETER_DATE).empty())
				{
					_date = from_string(map.get<string>(PARAMETER_DATE));
				}
				else
				{
					_date = day_clock::local_day();
					if(second_clock::local_time().time_of_day() < time_duration(3,0,0))
					{
						_date -= days(1);
					}
					_hideOldServices = true;
				}
			}
			catch (...)
			{
				throw RequestException("Bad value for date");
			}
			_displayCancelled = map.getDefault<bool>(PARAMETER_DISPLAY_CANCELLED, false);

			// Line
			RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			try
			{
				_line = CommercialLineTableSync::Get(id, _getEnv());
			}
			catch (...)
			{
				throw RequestException("Bad value for line ID");
			}

			// Routes reading
			JourneyPatternTableSync::SearchResult routes(
				JourneyPatternTableSync::Search(_getEnv(), _line->getKey())
			);
			BOOST_FOREACH(const boost::shared_ptr<JourneyPattern>& line, routes)
			{
				LineStopTableSync::Search(
					_getEnv(),
					line->getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true, true,
					UP_LINKS_LOAD_LEVEL
				);
			}

			_serviceNumber = map.getOptional<string>(PARAMETER_SERVICE);
		}



		util::ParametersMap BookableCommercialLineAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(!_hideOldServices && !_date.is_not_a_date())
			{
				m.insert(PARAMETER_DATE, to_iso_extended_string(_date));
			}
			m.insert(PARAMETER_DISPLAY_CANCELLED, _displayCancelled);
			if(_line.get()) m.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
			if(_serviceNumber) m.insert(PARAMETER_SERVICE, *_serviceNumber);
			return m;
		}



		void BookableCommercialLineAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB RESAS
			if (openTabContent(stream, TAB_RESAS))
			{
				// Rights
				bool globalReadRight(
					_request.getUser()->getProfile()->isAuthorized<ResaRight>(security::READ,UNKNOWN_RIGHT_LEVEL)
				);
				if (!globalReadRight && _line.get())
				{
					globalReadRight = _request.getUser()->getProfile()->isAuthorized<ResaRight>(security::READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_line->getKey()));
				}
				bool globalDeleteRight(
					_request.getUser()->getProfile()->isAuthorized<ResaRight>(security::DELETE_RIGHT,UNKNOWN_RIGHT_LEVEL)
				);
				if (!globalDeleteRight && _line.get())
				{
					globalDeleteRight = _request.getUser()->getProfile()->isAuthorized<ResaRight>(security::DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_line->getKey()));
				}
				bool globalCancelRight(
					_request.getUser()->getProfile()->isAuthorized<ResaRight>(security::CANCEL, UNKNOWN_RIGHT_LEVEL)
				);
				if (!globalCancelRight && _line.get())
				{
					globalCancelRight = _request.getUser()->getProfile()->isAuthorized<ResaRight>(security::CANCEL, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_line->getKey()));
				}

				// Requests
				AdminFunctionRequest<BookableCommercialLineAdmin> searchRequest(_request, *this);

				AdminActionFunctionRequest<CancelReservationAction,BookableCommercialLineAdmin> cancelRequest(_request, *this);

				AdminFunctionRequest<ResaCustomerAdmin> customerRequest(_request);

				AdminFunctionRequest<BookableCommercialLineAdmin> printRequest(_request, *this);

				AdminFunctionRequest<ReservationAdmin> openReservationRequest(_request);

				AdminActionFunctionRequest<ReservationUpdateAction, BookableCommercialLineAdmin> vehicleUpdateRquest(_request, *this);

				// Local variables
				ptime now(second_clock::local_time());

				// Temporary variables
				int seatsNumber(0);
	//			bool round_trip = false;
	//			bool next_overflow = false;
	//			bool course = false;

				// Reservations reading
				ReservationTableSync::SearchResult sqlreservations(
					ReservationTableSync::Search(
						_getEnv(),
						_line->getKey(),
						_date,
						_date + days(1),
						_serviceNumber,
						_displayCancelled ? logic::indeterminate : logic::tribool(false)
				)	);
				// Services reading
				vector<boost::shared_ptr<ReservableService> > sortedServices;
				{
					// Declaration
					map<string, boost::shared_ptr<ReservableService> > servicesByNumber;

					// Scheduled services
					ScheduledServiceTableSync::SearchResult services(
						ScheduledServiceTableSync::Search(
							Env::GetOfficialEnv(),
							optional<RegistryKeyType>(),
							_line->getKey(),
							optional<RegistryKeyType>(),
							_serviceNumber,
							_hideOldServices,
							0,
							optional<size_t>(),
							true, true, DOWN_LINKS_LOAD_LEVEL
					)	);
					BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& service, services)
					{
						if(	!service->isActive(_date) ||
							servicesByNumber.find(service->getServiceNumber()) != servicesByNumber.end()
						){
							continue;
						}

						servicesByNumber.insert(make_pair(service->getServiceNumber(), service));
						sortedServices.push_back(service);
					}

					// Free DRT services
					FreeDRTAreaTableSync::SearchResult areas(
						FreeDRTAreaTableSync::Search(
							Env::GetOfficialEnv(),
							_line->getKey()
					)	);
					BOOST_FOREACH(const boost::shared_ptr<FreeDRTArea>& area, areas)
					{
						FreeDRTTimeSlotTableSync::SearchResult freeDRTs(
							FreeDRTTimeSlotTableSync::Search(
								Env::GetOfficialEnv(),
								area->getKey()
						)	);
						BOOST_FOREACH(const boost::shared_ptr<FreeDRTTimeSlot>& service, freeDRTs)
						{
							if(	!service->isActive(_date) ||
								servicesByNumber.find(service->getServiceNumber()) != servicesByNumber.end()
							){
								continue;
							}

							servicesByNumber.insert(make_pair(service->getServiceNumber(), service));
							sortedServices.push_back(service);
						}
					}
				}
				if(_serviceNumber && sortedServices.empty()) return;

				// Sort reservations by service number
				map<string, ServiceReservations> reservations;
				BOOST_FOREACH(const boost::shared_ptr<const Reservation>& resa, sqlreservations)
				{
					reservations[resa->get<ServiceCode>()].addReservation(resa.get());
				}

				// Search form
				if(!_serviceNumber)
				{
					stream << "<h1>Recherche</h1>";

					SearchFormHTMLTable st(searchRequest.getHTMLForm());
					stream << st.open();
					stream << st.cell("Date", st.getForm().getCalendarInput(PARAMETER_DATE, _date));
					stream << st.cell("Afficher annulations", st.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_CANCELLED, _displayCancelled));
					stream << st.close();

					stream << "<h1>Liens</h1>";

					date date(_date);
					date -= days(1);
					searchRequest.getPage()->_hideOldServices = false;
					searchRequest.getPage()->_date = date;

					stream <<
						"<p>" << HTMLModule::getLinkButton(
							searchRequest.getURL(),
							"Jour précédent",
							string(),
							"/admin/img/resultset_previous.png"
						) << " ";

					searchRequest.getPage()->_date = _date;
					searchRequest.getPage()->_hideOldServices = !_hideOldServices;

					stream << HTMLModule::getLinkButton(
							searchRequest.getURL(),
							_hideOldServices ? "Journée entière" : "Prochains services",
							string(),
							_hideOldServices ? "/admin/img/stop_blue.png" : "/admin/img/stop_green.png"
						) << " ";

					date = _date;
					date += days(1);
					searchRequest.getPage()->_hideOldServices = false;
					searchRequest.getPage()->_date = date;

					stream << HTMLModule::getLinkButton(
							searchRequest.getURL(),
							"Jour suivant",
							string(),
							"/admin/img/resultset_next.png"
						) << "</p>"
					;

					stream << "<h1>Résultats</h1>";
				}
				else
				{
					int serviceSeatsNumber(reservations[dynamic_cast<Service*>(sortedServices[0].get())->getServiceNumber()].getSeatsNumber());
					string plural((serviceSeatsNumber > 1) ? "s" : "");
					stream << "<h1>";
					if (serviceSeatsNumber > 0)
					{
						stream << serviceSeatsNumber << " place" << plural << " réservée" << plural;
					}
					else
					{
						stream << "Aucune réservation";
					}
					stream << "</h1>";

					UseRule::ReservationAvailabilityType pedestrianAbility(sortedServices[0]->getReservationAbility(_date, USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
					UseRule::ReservationAvailabilityType handicappedAbility(sortedServices[0]->getReservationAbility(_date, USER_HANDICAPPED - USER_CLASS_CODE_OFFSET));
					UseRule::ReservationAvailabilityType bikeAbility(sortedServices[0]->getReservationAbility(_date, USER_BIKE - USER_CLASS_CODE_OFFSET));
					if(	pedestrianAbility == UseRule::RESERVATION_COMPULSORY_POSSIBLE ||
						pedestrianAbility == UseRule::RESERVATION_OPTIONAL_POSSIBLE ||
						handicappedAbility == UseRule::RESERVATION_COMPULSORY_POSSIBLE ||
						handicappedAbility == UseRule::RESERVATION_OPTIONAL_POSSIBLE ||
						bikeAbility == UseRule::RESERVATION_COMPULSORY_POSSIBLE ||
						bikeAbility == UseRule::RESERVATION_OPTIONAL_POSSIBLE
					){
						stream <<
							"<p class=\"info\">" <<
							"ATTENTION Cette liste de réservations est provisoire tant que le service est ouvert à la réservation." <<
							"</p>"
						;
						ptime pedestrianDeadLine(sortedServices[0]->getReservationDeadLine(_date, USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
						ptime handicappedDeadLine(sortedServices[0]->getReservationDeadLine(_date, USER_HANDICAPPED - USER_CLASS_CODE_OFFSET));
						ptime bikeDeadLine(sortedServices[0]->getReservationDeadLine(_date, USER_BIKE - USER_CLASS_CODE_OFFSET));
						ptime deadLine(pedestrianDeadLine);
						if(deadLine.is_not_a_date_time() || (!handicappedDeadLine.is_not_a_date_time() && handicappedDeadLine > deadLine))
						{
							deadLine = handicappedDeadLine;
						}
						if(deadLine.is_not_a_date_time() || (!bikeDeadLine.is_not_a_date_time() && bikeDeadLine > deadLine))
						{
							deadLine = bikeDeadLine;
						}
						stream <<
							"<p class=\"info\">" <<
							"Le service sera fermé à la réservation à partir du " <<
							deadLine.date() <<
							" à " << deadLine.time_of_day() <<
							"</p>"
						;
					}
				}

				HTMLTable::ColsVector c;
				c.push_back("Statut");
				c.push_back("Heure départ");
				c.push_back("Arrêt départ");
				c.push_back("Arrêt arrivée");
				c.push_back("Heure arrivée");
				c.push_back("Places");
				c.push_back("Client");
				if ((globalDeleteRight || globalCancelRight) && !_serviceNumber)
					c.push_back("Actions");
				HTMLTable t(c,"adminresults");
				stream << t.open();

				int serviceIdNumber = 0;

				// Display of services
				BOOST_FOREACH(const boost::shared_ptr<ReservableService>& service, sortedServices)
				{
					serviceIdNumber++;
					const string& serviceNumber(
						dynamic_cast<Service*>(service.get())->getServiceNumber()
					);
					const ServiceReservations::ReservationsList& serviceReservations(
						reservations[serviceNumber].getReservations()
					);
					int serviceSeatsNumber(reservations[serviceNumber].getSeatsNumber());
					string plural((serviceSeatsNumber > 1) ? "s" : "");
					seatsNumber += serviceSeatsNumber;

					// Display
					if(!_serviceNumber)
					{
						stream << t.row(string(), "service" + lexical_cast<string>(serviceIdNumber));

						stream << t.col(1, string(), true);

						{
							UseRule::ReservationAvailabilityType status(
								service->getReservationAbility(_date, USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
								);
							switch(status)
							{
							case UseRule::RESERVATION_COMPULSORY_TOO_EARLY:
								stream << HTMLModule::getHTMLImage("/admin/img/pedestrian_access.png","Piétons") << HTMLModule::getHTMLImage("/admin/img/stop.png", "Pas encore ouvert");
								break;

							case UseRule::RESERVATION_COMPULSORY_POSSIBLE:
								stream << HTMLModule::getHTMLImage("/admin/img/pedestrian_access.png","Piétons") << HTMLModule::getHTMLImage("/admin/img/stop_blue.png", "Ouvert à la réservation");
								break;

							case UseRule::RESERVATION_COMPULSORY_TOO_LATE:
								stream << HTMLModule::getHTMLImage("/admin/img/pedestrian_access.png","Piétons") << HTMLModule::getHTMLImage("/admin/img/tick.png", "Fermé à la réservation");
								break;
							default:
								break;
							}
						}

						{
							UseRule::ReservationAvailabilityType status(
								service->getReservationAbility(_date, USER_HANDICAPPED - USER_CLASS_CODE_OFFSET)
								);
							switch(status)
							{
							case UseRule::RESERVATION_COMPULSORY_TOO_EARLY:
								stream << HTMLModule::getHTMLImage("/admin/img/handicapped_access.png","Handicapés") << HTMLModule::getHTMLImage("/admin/img/stop.png", "Pas encore ouvert");
								break;

							case UseRule::RESERVATION_COMPULSORY_POSSIBLE:
								stream << HTMLModule::getHTMLImage("/admin/img/handicapped_access.png","Handicapés") << HTMLModule::getHTMLImage("/admin/img/stop_blue.png", "Ouvert à la réservation");
								break;

							case UseRule::RESERVATION_COMPULSORY_TOO_LATE:
								stream << HTMLModule::getHTMLImage("/admin/img/handicapped_access.png","Handicapés") << HTMLModule::getHTMLImage("/admin/img/tick.png", "Fermé à la réservation");
								break;
								
							default:
								break;
							}
						}

						{
							UseRule::ReservationAvailabilityType status(
								service->getReservationAbility(_date, USER_BIKE - USER_CLASS_CODE_OFFSET)
								);
							switch(status)
							{
							case UseRule::RESERVATION_COMPULSORY_TOO_EARLY:
								stream << HTMLModule::getHTMLImage("/admin/img/bike_access.png","Vélos") << HTMLModule::getHTMLImage("/admin/img/stop.png", "Pas encore ouvert");
								break;

							case UseRule::RESERVATION_COMPULSORY_POSSIBLE:
								stream << HTMLModule::getHTMLImage("/admin/img/bike_access.png","Vélos") << HTMLModule::getHTMLImage("/admin/img/stop_blue.png", "Ouvert à la réservation");
								break;

							case UseRule::RESERVATION_COMPULSORY_TOO_LATE:
								stream << HTMLModule::getHTMLImage("/admin/img/bike_access.png","Vélos") << HTMLModule::getHTMLImage("/admin/img/tick.png", "Fermé à la réservation");
								break;

							default:
								break;
							}
						}

						// Service title
						stream << t.col(6, string(), true);

						if(dynamic_cast<ScheduledService*>(service.get()))
						{
							stream << "Service " << serviceNumber << " - départ de ";
							const LineStop* edge(
								*static_cast<JourneyPattern*>(dynamic_cast<Service*>(service.get())->getPath())->getLineStops().begin()
							);
							if(dynamic_cast<const StopPoint*>(&*edge->get<LineNode>()))
							{
								stream << dynamic_cast<const StopPoint*>(&*edge->get<LineNode>())->getConnectionPlace()->getFullName();
							}
							else if(dynamic_cast<const DRTArea*>(&*edge->get<LineNode>()))
							{
								stream << dynamic_cast<const DRTArea*>(&*edge->get<LineNode>())->getName();
							}
							stream << " à " <<
								Service::GetTimeOfDay(dynamic_cast<ScheduledService*>(service.get())->getDepartureSchedule(false, 0))
							;
						}
						else if(dynamic_cast<FreeDRTTimeSlot*>(service.get()))
						{
							stream << "Tranche horaire de " << dynamic_cast<FreeDRTTimeSlot*>(service.get())->getFirstDeparture() <<
								" à " << dynamic_cast<FreeDRTTimeSlot*>(service.get())->getLastArrival();
						}
						if (serviceSeatsNumber > 0)
						{
							stream << " - " << serviceSeatsNumber << " place" << plural << " réservée" << plural;
						}

						printRequest.getPage()->setServiceNumber(dynamic_cast<Service*>(service.get())->getServiceNumber());

						stream << t.col(1, string(), true) << HTMLModule::getHTMLLink(printRequest.getURL(), HTMLModule::getHTMLImage("/admin/img/printer.png", "Imprimer"));
					}

					if (serviceReservations.empty())
					{
						stream << t.row();
						stream << t.col(8) << "Aucune réservation";
					}
					else
					{
						BOOST_FOREACH(const Reservation* reservation, serviceReservations)
						{
							ReservationStatus status(reservation->getStatus());
							openReservationRequest.getPage()->setReservation(_env->getSPtr(reservation));
							vehicleUpdateRquest.getAction()->setReservation(
								_env->getEditableSPtr(
									const_cast<Reservation*>(reservation)
							)	);

							customerRequest.getPage()->setUser(
								UserTableSync::Get(
									reservation->get<Transaction>()->get<Customer>() ? reservation->get<Transaction>()->get<Customer>()->getKey() : util::RegistryKeyType(0),
									_getEnv()
							)	);

							cancelRequest.getAction()->setTransaction(
								ReservationTransactionTableSync::GetEditable(
									reservation->get<Transaction>()->getKey(),
									_getEnv()
							)	);

							stream << t.row();
							stream <<
								t.col(1, string(), false, string(), reservation->get<Transaction>()->get<Comment>().empty() ? 1 : 2) <<
								HTMLModule::getHTMLImage("/admin/img/" + ResaModule::GetStatusIcon(status), reservation->getFullStatusText());
							stream <<
								t.col() <<
								(	reservation->get<DepartureTime>().date() != _date ?
									to_simple_string(reservation->get<DepartureTime>()) :
									to_simple_string(reservation->get<DepartureTime>().time_of_day())
								)
							;
							stream << t.col() << reservation->get<DeparturePlaceName>();
							stream << t.col() << reservation->get<ArrivalPlaceName>();
							stream <<
								t.col() <<
								(	reservation->get<ArrivalTime>().date() != _date ?
									to_simple_string(reservation->get<ArrivalTime>())	:
									to_simple_string(reservation->get<ArrivalTime>().time_of_day())
								)
							;
							stream << t.col() << reservation->get<Transaction>()->get<Seats>();

							// Customer name
							stream << t.col();
							if (globalReadRight && !_serviceNumber)
							{
								stream  << HTMLModule::getHTMLLink(
									customerRequest.getURL(),
									reservation->get<Transaction>()->get<CustomerName>()
								);
							}
							else
							{
								stream << reservation->get<Transaction>()->get<CustomerName>();
							}

							if(!_serviceNumber)
							{
								// Cancel link
								if (globalDeleteRight)
								{
									stream << t.col(1, string(), false, string(), reservation->get<Transaction>()->get<Comment>().empty() ? 1 : 2);
									switch(status)
									{
									case OPTION:
									case ACKNOWLEDGED_OPTION:
										stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler", "Etes-vous sûr de vouloir annuler la réservation ?", "/admin/img/" + ResaModule::GetStatusIcon(CANCELLED));
										break;

									case TO_BE_DONE:
									case ACKNOWLEDGED:
										stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler hors délai", "Etes-vous sûr de vouloir annuler la réservation (hors délai) ?", "/admin/img/" + ResaModule::GetStatusIcon(CANCELLED_AFTER_DELAY));
										break;

									case SHOULD_BE_AT_WORK:
										stream << HTMLModule::getLinkButton(cancelRequest.getURL() + "&absence=1#service" + lexical_cast<string>(serviceIdNumber), "Noter absence", "Etes-vous sûr de noter l'absence du client à l'arrêt ?", "/admin/img/" + ResaModule::GetStatusIcon(NO_SHOW));
										break;
									default:
										break;
									}
								}
								else if (globalCancelRight)
								{
									switch(status)
									{
									case AT_WORK:
									case DONE:
									case SHOULD_BE_DONE:
									case SHOULD_BE_AT_WORK:
										stream << HTMLModule::getLinkButton(cancelRequest.getURL() + "&absence=1#service" + lexical_cast<string>(serviceIdNumber), "Noter absence", "Etes-vous sûr de noter l'absence du client à l'arrêt ?", "/admin/img/" + ResaModule::GetStatusIcon(NO_SHOW));
										break;
									default:
									break;
									}
								}

								stream << " " << HTMLModule::getLinkButton(openReservationRequest.getURL(), "Ouvrir");

								HTMLForm uv(vehicleUpdateRquest.getHTMLForm("uv"+ lexical_cast<string>(reservation->getKey())));
								stream << uv.open();
								stream << uv.getSelectInput(
									ReservationUpdateAction::PARAMETER_VEHICLE_ID,
									VehicleTableSync::GetVehiclesList(*_env, string("(non affecté)")),
									optional<RegistryKeyType>(reservation->get<Vehicle>() ? reservation->get<Vehicle>()->getKey() : RegistryKeyType(0))
								);
								stream << uv.getSubmitButton("Enregistrer");
								stream << uv.close();
							}

							// Comment
							if(!reservation->get<Transaction>()->get<Comment>().empty())
							{
								stream << t.row();
								stream << t.col(6) << reservation->get<Transaction>()->get<Comment>();
							}
						}
					}
				} // End services loop

				stream << t.close();

				if(!_serviceNumber)
				{
					string plural(seatsNumber > 1 ? "s" : "");
					stream << "<h1>Total</h1>";
					stream <<
						"<p class=\"info\">" <<
						seatsNumber << " place" << plural << " réservée" << plural << " au total." <<
						"</p>"
					;
				}

				if(_serviceNumber)
				{
					stream << HTMLModule::GetHTMLJavascriptOpen() <<
						"window.print();" << HTMLModule::GetHTMLJavascriptClose();
				}
			}


			////////////////////////////////////////////////////////////////////
			// TAB FREE DRT
			if (openTabContent(stream, TAB_FREE_DRT))
			{
				// Request
				AdminFunctionRequest<FreeDRTBookingAdmin> openRequest(_request);

				// Areas
				FreeDRTAreaTableSync::SearchResult areas(
					FreeDRTAreaTableSync::Search(
						Env::GetOfficialEnv(),
						_line->getKey()
				)	);

				// The table
				HTMLTable::ColsVector c;
				c.push_back(string());
				c.push_back("Zone");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const boost::shared_ptr<FreeDRTArea>& area, areas)
				{
					// New row
					stream << t.row();

					// Open button
					openRequest.getPage()->setArea(const_pointer_cast<const FreeDRTArea>(area));
					stream <<
						t.col() <<
						HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + FreeDRTBookingAdmin::ICON)
					;

					// Name
					stream << t.col() << area->getName();
				}
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		bool BookableCommercialLineAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (!_line.get())
				return false;

			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_line->getKey()));
		}



		std::string BookableCommercialLineAdmin::getTitle() const
		{
			if(_line.get())
			{
				stringstream s;
				if(_serviceNumber)
				{
					ScheduledServiceTableSync::SearchResult services(
						ScheduledServiceTableSync::Search(
							_getEnv(),
							optional<RegistryKeyType>(),
							_line->getKey(),
							optional<RegistryKeyType>(),
							_serviceNumber,
							_hideOldServices,
							0, 1,
							true, true, UP_LINKS_LOAD_LEVEL
					)	);
					if(services.empty()) return string();

					string departureName;
					const Edge* startEdge = static_cast<const JourneyPattern*>(services[0]->getPath())->getEdge(0);

					if(dynamic_cast<const NamedPlace*>(startEdge->getHub()))
					{
						departureName = dynamic_cast<const NamedPlace*>(startEdge->getHub())->getFullName();
					}
					// Handle DRTArea as departure
					else
					{
						departureName = startEdge->getFromVertex()->getName();
					}

					ptime date(_date, services[0]->getDepartureSchedule(false, 0));
					s <<
						"Ligne " << _line->getShortName() <<
						" - service " << services[0]->getServiceNumber() <<
						" - départ de " << departureName <<
						" le " << date.date() <<
						" à " << date.time_of_day()
					;
				}
				else
				{
					s <<
						"<span class=\"linesmall " << _line->getStyle() << "\">" <<
						_line->getShortName() <<
						"</span>"
					;
				}
				return s.str();
			}
			return DEFAULT_TITLE;
		}



		std::string BookableCommercialLineAdmin::getIcon() const
		{
			return _serviceNumber ? "car.png" : ICON;
		}



		AdminInterfaceElement::PageLinks BookableCommercialLineAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			const BookableCommercialLineAdmin* ba(
				dynamic_cast<const BookableCommercialLineAdmin*>(&currentPage)
			);

			PageLinks links;

			if(	!_serviceNumber &&
				ba &&
				ba->_line == _line &&
				ba->_serviceNumber
			){
				boost::shared_ptr<BookableCommercialLineAdmin> p(
					getNewPage<BookableCommercialLineAdmin>()
				);
				p->setCommercialLine(_line);
				p->_serviceNumber = ba->_serviceNumber;

				links.push_back(p);
			}

			// Free DRT Time slot booking
			if( currentPage.getCurrentTreeBranch().find(*this) &&
				dynamic_cast<const FreeDRTBookingAdmin*>(&currentPage)
			){
				links.push_back(currentPage.getNewBaseCopiedPage());
			}

			return links;
		}



		bool BookableCommercialLineAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			const BookableCommercialLineAdmin& bother(static_cast<const BookableCommercialLineAdmin&>(other));
			return
				_line->getKey() == bother._line->getKey() &&
				_serviceNumber == bother._serviceNumber
			;
		}



		bool BookableCommercialLineAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage, const server::Request& request ) const
		{
			if(!_line.get())
			{
				return false;
			}

			return
				request.getUser()->getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_line->getKey()))
			;
		}



		void BookableCommercialLineAdmin::_buildTabs( const security::Profile& profile ) const
		{
			_tabs.clear();

			_tabs.push_back(Tab("Liste des réservations", TAB_RESAS, true, "resa_compulsory.png"));

			// Areas
			FreeDRTAreaTableSync::SearchResult areas(
				FreeDRTAreaTableSync::Search(
					Env::GetOfficialEnv(),
					_line->getKey(),
					0,
					1,
					false
			)	);
			if(!areas.empty())
			{
				_tabs.push_back(
					Tab(
						"Saisie de réservation",
						TAB_FREE_DRT,
						true,
						FreeDRTBookingAdmin::ICON
				)	);
			}

			_tabBuilded = true;
		}
}	}
