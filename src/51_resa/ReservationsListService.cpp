
/** ReservationsListService class implementation.
	@file ReservationsListService.cpp
	@author RCSobility
	@date 2011

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

#include "RequestException.h"
#include "Request.h"
#include "ReservationsListService.hpp"
#include "CommercialLineTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
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
#include "UserTableSync.h"
#include "Webpage.h"
#include "Language.hpp"
#include "Vehicle.hpp"

#include <map>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;


namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace cms;


	template<> const string util::FactorableTemplate<Function, resa::ReservationsListService>::FACTORY_KEY("ReservationsListService");

	namespace resa
	{
		const string ReservationsListService::PARAMETER_LINE_ID("li");
		const string ReservationsListService::PARAMETER_DATE("da");
		const string ReservationsListService::PARAMETER_SERVICE_NUMBER("sn");
		const string ReservationsListService::PARAMETER_RESERVATION_PAGE_ID("rp");
		const string ReservationsListService::PARAMETER_SERVICE_ID("se");
		const string ReservationsListService::PARAMETER_LANGUAGE("la");

		const string ReservationsListService::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
		const string ReservationsListService::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string ReservationsListService::DATA_LANGUAGE("language");
		const string ReservationsListService::DATA_NAME("name");
		const string ReservationsListService::DATA_RANK("rank");
		const string ReservationsListService::DATA_TRANSACTION_ID("transaction_id");
		const string ReservationsListService::DATA_SEATS_NUMBER("seats_number");
		const string ReservationsListService::DATA_VEHICLE_ID("vehicle_id");
		const string ReservationsListService::DATA_RESERVATION_ID("reservation_id");
		const string ReservationsListService::DATA_SEAT("seat");

		ParametersMap ReservationsListService::_getParametersMap() const
		{
			ParametersMap map;

			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			else
			{
				// Line
				if(_line.get())
				{
					map.insert(PARAMETER_LINE_ID, _line->getKey());
				}
				map.insert(PARAMETER_SERVICE_NUMBER, _serviceNumber);
			}

			map.insert(PARAMETER_DATE, to_iso_extended_string(_date));

			if(_reservationPage.get())
			{
				map.insert(PARAMETER_RESERVATION_PAGE_ID, _reservationPage->getKey());
			}

			if(_language)
			{
				map.insert(PARAMETER_LANGUAGE, _language->getIso639_2Code());
			}

			return map;
		}



		void ReservationsListService::_setFromParametersMap(const ParametersMap& map)
		{
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
				}
			}
			catch (...)
			{
				throw RequestException("Bad value for date");
			}

			if(map.isDefined(PARAMETER_SERVICE_ID))
			{
				// Service
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID));
				try
				{
					_service = ScheduledServiceTableSync::Get(id, *_env);
 					_line = _env->getSPtr(static_cast<const CommercialLine*>(_service->getPath()->getPathGroup()));
					_serviceNumber = _service->getServiceNumber();
				}
				catch (ObjectNotFoundException<ScheduledServiceTableSync>&)
				{
					throw RequestException("Bad value for service ID");
				}
			}
			else
			{
				// Line
				RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
				try
				{
					_line = CommercialLineTableSync::Get(id, *_env);
				}
				catch (ObjectNotFoundException<CommercialLine>&)
				{
					throw RequestException("Bad value for line ID");
				}
				_serviceNumber = map.get<string>(PARAMETER_SERVICE_NUMBER);
			}

			// Routes reading
			JourneyPatternTableSync::SearchResult routes(
				JourneyPatternTableSync::Search(*_env, _line->getKey())
			);
			BOOST_FOREACH(shared_ptr<JourneyPattern> line, routes)
			{
				LineStopTableSync::Search(
					*_env,
					line->getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true, true,
					UP_LINKS_LOAD_LEVEL
				);
			}

			// Reservation display page
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_RESERVATION_PAGE_ID, 0));
				if(id > 0)
				{
					_reservationPage = Env::GetOfficialEnv().get<Webpage>(id);
				}
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such reservation page");
			}

			if(map.isDefined(PARAMETER_LANGUAGE))
			{
				_language = Language::GetLanguageFromIso639_2Code(map.get<string>(PARAMETER_LANGUAGE));
			}
		}

		void ReservationsListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Rights
			bool globalReadRight(
				// _request.isAuthorized<ResaRight>(security::READ,UNKNOWN_RIGHT_LEVEL)
				true
			);
			bool globalDeleteRight(
				// _request.isAuthorized<ResaRight>(security::DELETE_RIGHT,UNKNOWN_RIGHT_LEVEL)
				true
			);

			// Local variables
			ptime now(second_clock::local_time());

			// Temporary variables
			int seatsNumber(0);

			// Reservations reading
			ReservationTableSync::SearchResult sqlreservations(
				ReservationTableSync::Search(
					*_env,
					_line->getKey(),
					_date,
					_serviceNumber,
					false,
					logic::tribool(false)
			)	);

			// Services reading
			vector<shared_ptr<ScheduledService> > sortedServices;
			{
				map<string, shared_ptr<ScheduledService> > servicesByNumber;

				ScheduledServiceTableSync::SearchResult services(
					ScheduledServiceTableSync::Search(
						*_env,
						optional<RegistryKeyType>(),
						_line->getKey(),
						optional<RegistryKeyType>(),
						_serviceNumber,
						false,
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
			if(sortedServices.empty()) return;

			// Sort reservations
			map<string, ServiceReservations> reservations;
			BOOST_FOREACH(shared_ptr<const Reservation> resa, sqlreservations)
			{
				if(!_env->getRegistry<ScheduledService>().contains(resa->getServiceId())) continue;

				const ScheduledService* service(_env->getRegistry<ScheduledService>().get(resa->getServiceId()).get());
				if(reservations.find(service->getServiceNumber()) == reservations.end())
				{
					reservations.insert(make_pair(service->getServiceNumber(), ServiceReservations()));
				}
				reservations[service->getServiceNumber()].addReservation(resa);
			}

			// Display of services
			BOOST_FOREACH(shared_ptr<ScheduledService> service, sortedServices)
			{
				const ServiceReservations::ReservationsList& serviceReservations (reservations[service->getServiceNumber()].getReservations());
				int serviceSeatsNumber(reservations[service->getServiceNumber()].getSeatsNumber());
				string plural((serviceSeatsNumber > 1) ? "s" : "");
				seatsNumber += serviceSeatsNumber;

				if(_reservationPage.get())
				{
					size_t rank(0);
					BOOST_FOREACH(shared_ptr<const Reservation> reservation, serviceReservations)
					{
						_displayReservation(
							stream,
							request,
							*reservation,
							rank++
						);
					}
				}
			} // End services loop
		}



		bool ReservationsListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ReservationsListService::getOutputMimeType() const
		{
			return "text/html";
		}



		void ReservationsListService::_displayReservation(
			ostream& stream,
			const server::Request& request,
			const Reservation& reservation,
			std::size_t rank
		) const {

			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(DATA_NAME, reservation.getTransaction()->getCustomerName());
			pm.insert(DATA_DEPARTURE_PLACE_NAME, reservation.getDeparturePlaceName());
			pm.insert(DATA_ARRIVAL_PLACE_NAME, reservation.getArrivalPlaceName());
			pm.insert(DATA_RANK, rank);
			pm.insert(DATA_RESERVATION_ID, reservation.getKey());
			pm.insert(DATA_TRANSACTION_ID, reservation.getTransaction()->getKey());
			pm.insert(DATA_SEATS_NUMBER, reservation.getTransaction()->getSeats());

			// Vehicle
			if(reservation.getVehicle())
			{
				pm.insert(DATA_VEHICLE_ID, reservation.getVehicle()->getKey());
			}
			pm.insert(DATA_SEAT, reservation.getSeatNumber());

			// Language
			shared_ptr<const User> user(UserTableSync::Get(reservation.getTransaction()->getCustomerUserId(), *_env));

			if(_language && user->getLanguage())
			{
				pm.insert(DATA_LANGUAGE, user->getLanguage()->getName(*_language));
			}

			// Launch of the display
			_reservationPage->display(stream, request, pm);
		}



		ReservationsListService::ReservationsListService():
			FactorableTemplate<server::Function,ReservationsListService>()
		{
			setEnv(shared_ptr<Env>(new Env));
		}
	}
}
