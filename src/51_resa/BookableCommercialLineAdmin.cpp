
/** BookableCommercialLineAdmin class implementation.
	@file BookableCommercialLineAdmin.cpp
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

#include "BookableCommercialLineAdmin.h"
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

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, BookableCommercialLineAdmin>::FACTORY_KEY("BookableCommercialLineAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLineAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLineAdmin>::DEFAULT_TITLE("Ligne inconnue");
	}

	namespace resa
	{
		const string BookableCommercialLineAdmin::PARAMETER_DATE("da");
		const string BookableCommercialLineAdmin::PARAMETER_DISPLAY_CANCELLED("dc");
		const string BookableCommercialLineAdmin::PARAMETER_SERVICE("se");


		BookableCommercialLineAdmin::BookableCommercialLineAdmin()
			: AdminInterfaceElementTemplate<BookableCommercialLineAdmin>(),
			_date(not_a_date_time),
			_hideOldServices(false),
			_displayCancelled(false)
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
			BOOST_FOREACH(shared_ptr<JourneyPattern> line, routes)
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
		
		
		
		server::ParametersMap BookableCommercialLineAdmin::getParametersMap() const
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
			const admin::AdminRequest& _request
		) const	{
			// Rights
			bool globalReadRight(
				_request.isAuthorized<ResaRight>(security::READ,UNKNOWN_RIGHT_LEVEL)
			);
			bool globalDeleteRight(
				_request.isAuthorized<ResaRight>(security::DELETE_RIGHT,UNKNOWN_RIGHT_LEVEL)
			);

			// Requests
			AdminFunctionRequest<BookableCommercialLineAdmin> searchRequest(_request);

			AdminActionFunctionRequest<CancelReservationAction,BookableCommercialLineAdmin> cancelRequest(_request);

			AdminFunctionRequest<ResaCustomerAdmin> customerRequest(_request);

			AdminFunctionRequest<BookableCommercialLineAdmin> printRequest(_request);
			
			

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
					_serviceNumber,
					_hideOldServices,
					_displayCancelled ? logic::indeterminate : logic::tribool(false)
			)	);
			// Services reading
			vector<shared_ptr<ScheduledService> > sortedServices;
			{
				map<string, shared_ptr<ScheduledService> > servicesByNumber;
				
				ScheduledServiceTableSync::SearchResult services(
					ScheduledServiceTableSync::Search(
						_getEnv(),
						optional<RegistryKeyType>(),
						_line->getKey(),
						optional<RegistryKeyType>(),
						_serviceNumber,
						_hideOldServices,
						0,
						optional<size_t>(),
						true, true, UP_LINKS_LOAD_LEVEL
				)	);
				BOOST_FOREACH(shared_ptr<ScheduledService> service, services)
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
			if(_serviceNumber && sortedServices.empty()) return;

			// Sort reservations
			map<string, ServiceReservations> reservations;
			BOOST_FOREACH(shared_ptr<const Reservation> resa, sqlreservations)
			{
				if(!_getEnv().getRegistry<ScheduledService>().contains(resa->getServiceId())) continue;
				
				const ScheduledService* service(_getEnv().getRegistry<ScheduledService>().get(resa->getServiceId()).get());
				if(reservations.find(service->getServiceNumber()) == reservations.end())
				{
					reservations.insert(make_pair(service->getServiceNumber(), ServiceReservations()));
				}
				reservations[service->getServiceNumber()].addReservation(resa);
			}

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
						"resultset_previous.png"
					) << " ";
				
				searchRequest.getPage()->_date = _date;
				searchRequest.getPage()->_hideOldServices = !_hideOldServices;	
					
				stream << HTMLModule::getLinkButton(
						searchRequest.getURL(),
						_hideOldServices ? "Journée entière" : "Prochains services",
						string(),
						_hideOldServices ? "stop_blue.png" : "stop_green.png"
					) << " ";
				
				date = _date;
				date += days(1);
				searchRequest.getPage()->_hideOldServices = false;
				searchRequest.getPage()->_date = date;

				stream << HTMLModule::getLinkButton(
						searchRequest.getURL(),
						"Jour suivant",
						string(),
						"resultset_next.png"
					) << "</p>"
				;
				
				stream << "<h1>Résultats</h1>";
			}
			else
			{
				int serviceSeatsNumber(reservations[sortedServices[0]->getServiceNumber()].getSeatsNumber());
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
			if (globalDeleteRight && !_serviceNumber)
				c.push_back("Actions");
			HTMLTable t(c,"adminresults");
			stream << t.open();

			// Display of services
			BOOST_FOREACH(shared_ptr<ScheduledService> service, sortedServices)
			{
				const ServiceReservations::ReservationsList& serviceReservations (reservations[service->getServiceNumber()].getReservations());
				int serviceSeatsNumber(reservations[service->getServiceNumber()].getSeatsNumber());
				string plural((serviceSeatsNumber > 1) ? "s" : "");
				seatsNumber += serviceSeatsNumber;

				// Display
				if(!_serviceNumber)
				{
					stream << t.row();
					
					stream << t.col(1, string(), true);
					
					{
						UseRule::ReservationAvailabilityType status(
							service->getReservationAbility(_date, USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
							);
						switch(status)
						{
						case UseRule::RESERVATION_COMPULSORY_TOO_EARLY:
							stream << HTMLModule::getHTMLImage("pedestrian_access.png","Piétons") << HTMLModule::getHTMLImage("stop.png", "Pas encore ouvert");
							break;

						case UseRule::RESERVATION_COMPULSORY_POSSIBLE:
							stream << HTMLModule::getHTMLImage("pedestrian_access.png","Piétons") << HTMLModule::getHTMLImage("stop_blue.png", "Ouvert à la réservation");
							break;

						case UseRule::RESERVATION_COMPULSORY_TOO_LATE:
							stream << HTMLModule::getHTMLImage("pedestrian_access.png","Piétons") << HTMLModule::getHTMLImage("tick.png", "Fermé à la réservation");
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
							stream << HTMLModule::getHTMLImage("handicapped_access.png","Handicapés") << HTMLModule::getHTMLImage("stop.png", "Pas encore ouvert");
							break;

						case UseRule::RESERVATION_COMPULSORY_POSSIBLE:
							stream << HTMLModule::getHTMLImage("handicapped_access.png","Handicapés") << HTMLModule::getHTMLImage("stop_blue.png", "Ouvert à la réservation");
							break;

						case UseRule::RESERVATION_COMPULSORY_TOO_LATE:
							stream << HTMLModule::getHTMLImage("handicapped_access.png","Handicapés") << HTMLModule::getHTMLImage("tick.png", "Fermé à la réservation");
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
							stream << HTMLModule::getHTMLImage("bike_access.png","Vélos") << HTMLModule::getHTMLImage("stop.png", "Pas encore ouvert");
							break;

						case UseRule::RESERVATION_COMPULSORY_POSSIBLE:
							stream << HTMLModule::getHTMLImage("bike_access.png","Vélos") << HTMLModule::getHTMLImage("stop_blue.png", "Ouvert à la réservation");
							break;

						case UseRule::RESERVATION_COMPULSORY_TOO_LATE:
							stream << HTMLModule::getHTMLImage("bike_access.png","Vélos") << HTMLModule::getHTMLImage("tick.png", "Fermé à la réservation");
							break;
						}
					}

					stream << t.col(6, string(), true) << "Service " << service->getServiceNumber() << " - départ de " <<
						dynamic_cast<const NamedPlace*>(static_cast<const JourneyPattern*>(service->getPath())->getEdge(0)->getHub())->getFullName() <<
						" à " << Service::GetTimeOfDay(service->getDepartureSchedule(false, 0));
					if (serviceSeatsNumber > 0)
						stream << " - " << serviceSeatsNumber << " place" << plural << " réservée" << plural;

					printRequest.getPage()->setServiceNumber(service->getServiceNumber());

					stream << t.col(1, string(), true) << HTMLModule::getHTMLLink(printRequest.getURL(), HTMLModule::getHTMLImage("printer.png", "Imprimer"));
				}

				if (serviceReservations.empty())
				{
					stream << t.row();
					stream << t.col(8) << "Aucune réservation";
				}
				else
				{
					BOOST_FOREACH(shared_ptr<const Reservation> reservation, serviceReservations)
					{
						ReservationStatus status(reservation->getStatus());

						customerRequest.getPage()->setUser(
							UserTableSync::Get(
								reservation->getTransaction()->getCustomerUserId(),
								_getEnv()
						)	);
						
						cancelRequest.getAction()->setTransaction(
							ReservationTransactionTableSync::GetEditable(
								reservation->getTransaction()->getKey(),
								_getEnv()
						)	);

						stream << t.row();
						stream << t.col() << HTMLModule::getHTMLImage(ResaModule::GetStatusIcon(status), reservation->getFullStatusText());
						stream <<
							t.col() <<
							(	reservation->getDepartureTime().date() != _date ?
								to_simple_string(reservation->getDepartureTime()) :
								to_simple_string(reservation->getDepartureTime().time_of_day())
							)
						;
						stream << t.col() << reservation->getDeparturePlaceName();
						stream << t.col() << reservation->getArrivalPlaceName();
						stream <<
							t.col() <<
							(	reservation->getArrivalTime().date() != _date ?
								to_simple_string(reservation->getArrivalTime())	:
								to_simple_string(reservation->getArrivalTime().time_of_day())
							)
						;
						stream << t.col() << reservation->getTransaction()->getSeats();

						// Customer name
						stream << t.col();
						if (globalReadRight && !_serviceNumber)
						{
							stream  << HTMLModule::getHTMLLink(
								customerRequest.getURL(),
								reservation->getTransaction()->getCustomerName()
							);
						}
						else
						{
							stream << reservation->getTransaction()->getCustomerName();
						}

						if(!_serviceNumber)
						{
							// Cancel link
							if (globalDeleteRight)
							{
								stream << t.col();
								switch(status)
								{
								case OPTION:
									stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler", "Etes-vous sûr de vouloir annuler la réservation ?", ResaModule::GetStatusIcon(CANCELLED));
									break;

								case TO_BE_DONE:
									stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler hors délai", "Etes-vous sûr de vouloir annuler la réservation (hors délai) ?", ResaModule::GetStatusIcon(CANCELLED_AFTER_DELAY));
									break;

								case AT_WORK:
									stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Noter absence", "Etes-vous sûr de noter l'absence du client à l'arrêt ?", ResaModule::GetStatusIcon(NO_SHOW));
									break;
								}
							}
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

					ptime date(_date, services[0]->getDepartureSchedule(false, 0));
					s <<
						"Ligne " << _line->getShortName() <<
						" - service " << services[0]->getServiceNumber() <<
						" - départ de " << dynamic_cast<const NamedPlace*>(
								static_cast<const JourneyPattern*>(services[0]->getPath())->getEdge(0)->getHub()
							)->getFullName() <<
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

		void BookableCommercialLineAdmin::setServiceNumber(const optional<string>& value)
		{
			_serviceNumber = value;
		}
		
		const optional<string>& BookableCommercialLineAdmin::getServiceNumber() const
		{
			return _serviceNumber;
		}
		
		void BookableCommercialLineAdmin::setCommercialLine(boost::shared_ptr<pt::CommercialLine> value)
		{
			_line = const_pointer_cast<const CommercialLine>(value);
		}

		boost::shared_ptr<const pt::CommercialLine> BookableCommercialLineAdmin::getCommercialLine() const
		{
			return _line;
		}
		
		AdminInterfaceElement::PageLinks BookableCommercialLineAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
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
				shared_ptr<BookableCommercialLineAdmin> p(
					getNewPage<BookableCommercialLineAdmin>()
				);
				p->setCommercialLineC(_line);
				p->_serviceNumber = ba->_serviceNumber;

				links.push_back(p);
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
		

		bool BookableCommercialLineAdmin::isPageVisibleInTree( const AdminInterfaceElement& currentPage, const admin::AdminRequest& request ) const
		{
			return
				!request.isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL) &&
				request.isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, string())
			;
		}



		void BookableCommercialLineAdmin::setCommercialLineC( boost::shared_ptr<const pt::CommercialLine> value )
		{
			_line = value;
		}
	}
}
