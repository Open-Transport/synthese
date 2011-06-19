
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
#include "StopPoint.hpp"

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
	using namespace graph;

	template<> const string util::FactorableTemplate<Function, resa::ReservationsListService>::FACTORY_KEY("ReservationsListService");

	namespace resa
	{
		const string ReservationsListService::PARAMETER_LINE_ID("li");
		const string ReservationsListService::PARAMETER_DATE("da");
		const string ReservationsListService::PARAMETER_SERVICE_NUMBER("sn");
		const string ReservationsListService::PARAMETER_RESERVATION_PAGE_ID("rp");
		const string ReservationsListService::PARAMETER_SERVICE_ID("se");
		const string ReservationsListService::PARAMETER_LANGUAGE("la");
		const string ReservationsListService::PARAMETER_MINIMAL_DEPARTURE_RANK("min_dp_rank");
		const string ReservationsListService::PARAMETER_MAXIMAL_DEPARTURE_RANK("max_dp_rank");
		const string ReservationsListService::PARAMETER_MINIMAL_ARRIVAL_RANK("min_arr_rank");
		const string ReservationsListService::PARAMETER_MAXIMAL_ARRIVAL_RANK("max_arr_rank");
		const string ReservationsListService::PARAMETER_LINKED_WITH_VEHICLE_ONLY("linked_with_vehicle_only");

		const string ReservationsListService::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
		const string ReservationsListService::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string ReservationsListService::DATA_ARRIVAL_PLACE_ID("arrival_place_id");
		const string ReservationsListService::DATA_DEPARTURE_PLACE_ID("departure_place_id");
		const string ReservationsListService::DATA_LANGUAGE("language");
		const string ReservationsListService::DATA_NAME("name");
		const string ReservationsListService::DATA_RANK("rank");
		const string ReservationsListService::DATA_TRANSACTION_ID("transaction_id");
		const string ReservationsListService::DATA_SEATS_NUMBER("seats_number");
		const string ReservationsListService::DATA_VEHICLE_ID("vehicle_id");
		const string ReservationsListService::DATA_RESERVATION_ID("reservation_id");
		const string ReservationsListService::DATA_SEAT("seat");
		const string ReservationsListService::DATA_SERVICE_NUMBER("service_number");
		const string ReservationsListService::DATA_SERVICE_ID("service_id");
		const string ReservationsListService::DATA_DEPARTURE_TIME("departure_time");
		const string ReservationsListService::DATA_ARRIVAL_TIME("arrival_time");
		const string ReservationsListService::DATA_CANCELLATION_TIME("cancellation_time");

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
				if(_serviceNumber)
				{
					map.insert(PARAMETER_SERVICE_NUMBER, *_serviceNumber);
				}
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

			if(_minDepartureRank)
			{
				map.insert(PARAMETER_MINIMAL_DEPARTURE_RANK, *_minDepartureRank);
			}
			if(_maxDepartureRank)
			{
				map.insert(PARAMETER_MAXIMAL_DEPARTURE_RANK, *_maxDepartureRank);
			}
			if(_minArrivalRank)
			{
				map.insert(PARAMETER_MINIMAL_ARRIVAL_RANK, *_minArrivalRank);
			}
			if(_maxArrivalRank)
			{
				map.insert(PARAMETER_MAXIMAL_ARRIVAL_RANK, *_maxArrivalRank);
			}
			map.insert(PARAMETER_LINKED_WITH_VEHICLE_ONLY, _linkedWithVehicleOnly);

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
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_LINE_ID));
				try
				{
					_line = CommercialLineTableSync::Get(id, *_env);
				}
				catch (ObjectNotFoundException<CommercialLine>&)
				{
					throw RequestException("Bad value for line ID");
				}
				if(!map.getDefault<string>(PARAMETER_SERVICE_NUMBER).empty())
				{
					_serviceNumber = map.get<string>(PARAMETER_SERVICE_NUMBER);
				}
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

			_minDepartureRank = map.getOptional<size_t>(PARAMETER_MINIMAL_DEPARTURE_RANK);
			_maxDepartureRank = map.getOptional<size_t>(PARAMETER_MAXIMAL_DEPARTURE_RANK);
			_minArrivalRank = map.getOptional<size_t>(PARAMETER_MINIMAL_ARRIVAL_RANK);
			_maxArrivalRank = map.getOptional<size_t>(PARAMETER_MAXIMAL_ARRIVAL_RANK);
			_linkedWithVehicleOnly = map.getDefault<bool>(PARAMETER_LINKED_WITH_VEHICLE_ONLY, false);
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
			size_t rank(0);
			BOOST_FOREACH(shared_ptr<ScheduledService> service, sortedServices)
			{
				const ServiceReservations::ReservationsList& serviceReservations (reservations[service->getServiceNumber()].getReservations());
				int serviceSeatsNumber(reservations[service->getServiceNumber()].getSeatsNumber());
				string plural((serviceSeatsNumber > 1) ? "s" : "");
				seatsNumber += serviceSeatsNumber;

				if(_reservationPage.get())
				{
					BOOST_FOREACH(shared_ptr<const Reservation> reservation, serviceReservations)
					{
						// Departure rank check
						if(_minDepartureRank || _maxDepartureRank)
						{
							bool result(true);
							shared_ptr<const StopArea> stopArea(
								Env::GetOfficialEnv().get<StopArea>(reservation->getDeparturePlaceId())
							);
							BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, stopArea->getPhysicalStops())
							{
								try
								{
									const Edge& edge(service->getPath()->findEdgeByVertex(itStop.second));
									if( _minDepartureRank && edge.getRankInPath() < *_minDepartureRank ||
										_maxDepartureRank && edge.getRankInPath() > *_maxDepartureRank
									){
										result = false;
										break;
									}
								}
								catch (Path::VertexNotFoundException&)
								{
								}
							}
							if(!result)
							{
								continue;
							}
						}

						// Arrival rank check
						if(_minArrivalRank || _maxArrivalRank)
						{
							bool result(true);
							shared_ptr<const StopArea> stopArea(
								Env::GetOfficialEnv().get<StopArea>(reservation->getArrivalPlaceId())
							);
							BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, stopArea->getPhysicalStops())
							{
								try
								{
									const Edge& edge(service->getPath()->findEdgeByVertex(itStop.second));
									if( _minArrivalRank && edge.getRankInPath() < *_minArrivalRank ||
										_maxArrivalRank && edge.getRankInPath() > *_maxArrivalRank
									){
										result = false;
										break;
									}
								}
								catch (Path::VertexNotFoundException&)
								{
								}
							}
							if(!result)
							{
								continue;
							}
						}

						// Check of link with vehicle
						if(_linkedWithVehicleOnly && !reservation->getVehicle())
						{
							continue;
						}

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
			pm.insert(DATA_DEPARTURE_PLACE_ID, reservation.getDeparturePlaceId());
			pm.insert(DATA_ARRIVAL_PLACE_ID, reservation.getArrivalPlaceId());
			pm.insert(DATA_RANK, rank);
			pm.insert(DATA_RESERVATION_ID, reservation.getKey());
			pm.insert(DATA_TRANSACTION_ID, reservation.getTransaction()->getKey());
			pm.insert(DATA_SEATS_NUMBER, reservation.getTransaction()->getSeats());
			pm.insert(DATA_SERVICE_NUMBER, reservation.getServiceCode());
			pm.insert(DATA_SERVICE_ID, reservation.getServiceId());
			pm.insert(DATA_DEPARTURE_TIME, reservation.getDepartureTime());
			pm.insert(DATA_ARRIVAL_TIME, reservation.getArrivalTime());
			if(!reservation.getTransaction()->getCancellationTime().is_not_a_date_time())
			{
				pm.insert(DATA_CANCELLATION_TIME, reservation.getTransaction()->getCancellationTime());
			}

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
			FactorableTemplate<server::Function,ReservationsListService>(),
			_linkedWithVehicleOnly(false)
		{
			setEnv(shared_ptr<Env>(new Env));
		}
	}
}
