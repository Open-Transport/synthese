
/** ReservationsListService class implementation.
	@file ReservationsListService.cpp
	@author Hugues Romain
	@date 2011

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

#include "ReservationsListService.hpp"

#include "AdvancedSelectTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStop.h"
#include "LineStopTableSync.h"
#include "MimeTypes.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "ServiceReservations.h"
#include "StopPoint.hpp"
#include "UserTableSync.h"
#include "Webpage.h"

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
		const string ReservationsListService::PARAMETER_USE_CACHE = "use_cache";
		const string ReservationsListService::PARAMETER_OUTPUT_FORMAT = "output_format";

		const string ReservationsListService::DATA_RANK = "rank";
		const string ReservationsListService::DATA_RESERVATION = "reservation";
		const string ReservationsListService::DATA_RESERVATIONS = "reservations";

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
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
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
			if(_useCache)
			{
				map.insert(PARAMETER_USE_CACHE, _useCache);
			}
			map.insert(PARAMETER_LINKED_WITH_VEHICLE_ONLY, _linkedWithVehicleOnly);

			return map;
		}



		void ReservationsListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Cache
			_useCache = map.getDefault<bool>(PARAMETER_USE_CACHE, false);

			// Date
			if(_useCache)
			{
				_date = day_clock::local_day();
			}
			else if(map.isDefined(PARAMETER_DATE))
			{
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
			}

			if(map.isDefined(PARAMETER_SERVICE_ID))
			{
				// Service
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID));
				try
				{
					_service =
						_useCache ?
						Env::GetOfficialEnv().get<ScheduledService>(id) :
						ScheduledServiceTableSync::Get(id, *_env)
					;
					_line = (_useCache ? Env::GetOfficialEnv() : *_env).getSPtr(
						static_cast<const CommercialLine*>(_service->getPath()->getPathGroup())
					);
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
				try
				{
					RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_LINE_ID));
					if(_useCache)
					{
						_line = Env::GetOfficialEnv().get<CommercialLine>(id);
					}
					else
					{
						_line = CommercialLineTableSync::Get(id, *_env);
					}
				}
				catch (ObjectNotFoundException<CommercialLine>&)
				{
					throw RequestException("Bad value for line ID");
				}

				// Service number
				if(!map.getDefault<string>(PARAMETER_SERVICE_NUMBER).empty())
				{
					_serviceNumber = map.get<string>(PARAMETER_SERVICE_NUMBER);
				}
			}

			// Routes reading
			if(!_useCache)
			{
				JourneyPatternTableSync::SearchResult routes(
					JourneyPatternTableSync::Search(*_env, _line->getKey())
				);
				BOOST_FOREACH(const shared_ptr<JourneyPattern>& line, routes)
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

			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);
		}



		util::ParametersMap ReservationsListService::run(
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

			// Services reading
			if(!_useCache)
			{
				ScheduledServiceTableSync::Search(
					*_env,
					optional<RegistryKeyType>(),
					_line->getKey(),
					optional<RegistryKeyType>(),
					_serviceNumber,
					false,
					0,
					optional<size_t>(),
					true, true, ALGORITHMS_OPTIMIZATION_LOAD_LEVEL
				);
			}

			// Get the services sorted by number
			CommercialLine::ServicesVector services(
				_serviceNumber ?
				_line->getServices(*_serviceNumber) :
				_line->getServices()
			);

			// Reservations list
			map<string, ServiceReservations> reservations;
			if(_useCache)
			{
				// Lock the cached reservations list
				recursive_mutex::scoped_lock lock(ResaModule::GetReservationsByServiceMutex());

				// Loop on services
				BOOST_FOREACH(const Service* service, services)
				{
					const ResaModule::ReservationsByService::mapped_type& reservationsEnv(
						ResaModule::GetReservationsByService(
							*service
					)	);
					BOOST_FOREACH(const ResaModule::ReservationsByService::mapped_type::value_type& resa, reservationsEnv)
					{
						reservations[service->getServiceNumber()].addReservation(resa);
					}
				}
			}
			else
			{
				// Reservations reading
				ReservationTableSync::SearchResult sqlreservations(
					ReservationTableSync::Search(
						*_env,
						_line->getKey(),
						_date,
						_serviceNumber,
						logic::tribool(false)
				)	);

				// Sort reservations
				BOOST_FOREACH(const shared_ptr<const Reservation>& resa, sqlreservations)
				{
					if(!_env->getRegistry<ScheduledService>().contains(resa->getServiceId())) continue;

					const ScheduledService* service(
						_env->getRegistry<ScheduledService>().get(resa->getServiceId()).get()
					);
					if(reservations.find(service->getServiceNumber()) == reservations.end())
					{
						reservations.insert(make_pair(service->getServiceNumber(), ServiceReservations()));
					}
					reservations[service->getServiceNumber()].addReservation(resa.get());
				}
			}

			// Display of services
			ParametersMap pm;
			int seatsNumber(0);
			set<string> serviceNumbers;
			BOOST_FOREACH(const Service* service, services)
			{
				serviceNumbers.insert(service->getServiceNumber());
			}
			BOOST_FOREACH(const string& serviceNumber, serviceNumbers)
			{
				const ServiceReservations::ReservationsList& serviceReservations (reservations[serviceNumber].getReservations());
				int serviceSeatsNumber(reservations[serviceNumber].getSeatsNumber());
				seatsNumber += serviceSeatsNumber;

				if(_reservationPage.get())
				{
					BOOST_FOREACH(const Reservation* reservation, serviceReservations)
					{
						// Departure rank check
						if(	(_minDepartureRank || _maxDepartureRank) &&
							decodeTableId(reservation->getServiceId()) == ScheduledServiceTableSync::TABLE.ID
						){
							bool result(true);
							shared_ptr<const ScheduledService> service(
								Env::GetOfficialEnv().get<ScheduledService>(reservation->getServiceId())
							);
							shared_ptr<const StopArea> stopArea(
								Env::GetOfficialEnv().get<StopArea>(reservation->getDeparturePlaceId())
							);
							BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, stopArea->getPhysicalStops())
							{
								try
								{
									const Edge& edge(
										Env::GetOfficialEnv().get<JourneyPattern>(
											service->getPath()->getKey()
										)->findEdgeByVertex(itStop.second)
									);
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
						if(	(_minArrivalRank || _maxArrivalRank) &&
							decodeTableId(reservation->getServiceId()) == ScheduledServiceTableSync::TABLE.ID
						){
							bool result(true);
							shared_ptr<const ScheduledService> service(
								Env::GetOfficialEnv().get<ScheduledService>(reservation->getServiceId())
							);
							shared_ptr<const StopArea> stopArea(
								Env::GetOfficialEnv().get<StopArea>(reservation->getArrivalPlaceId())
							);
							BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, stopArea->getPhysicalStops())
							{
								try
								{
									const Edge& edge(
										Env::GetOfficialEnv().get<JourneyPattern>(service->getPath()->getKey())->findEdgeByVertex(itStop.second)
									);
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

						shared_ptr<ParametersMap> resaPM(new ParametersMap);
						reservation->toParametersMap(*resaPM, _language);
						pm.insert(DATA_RESERVATION, resaPM);
					}
				}
			} // End services loop

			// Output
			if(_reservationPage.get())
			{
				size_t rank(0);
				BOOST_FOREACH(shared_ptr<ParametersMap> resaPM, pm.getSubMaps(DATA_RESERVATION))
				{
					resaPM->merge(getTemplateParameters());
					resaPM->insert(DATA_RANK, rank++);
					_reservationPage->display(stream, request, *resaPM);
				}
			}
			else if(_outputFormat == MimeTypes::XML)
			{
				pm.outputXML(
					stream,
					DATA_RESERVATIONS,
					true,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/51_resa/ReservationsListService.xsd"
				);
			}
			else if(_outputFormat == MimeTypes::JSON)
			{
				pm.outputJSON(stream, DATA_RESERVATIONS);
			}
			else if(_outputFormat == MimeTypes::CSV)
			{
				pm.outputCSV(stream, DATA_RESERVATIONS);
			}

			return pm;
		}



		bool ReservationsListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ReservationsListService::getOutputMimeType() const
		{
			return _reservationPage.get() ? _reservationPage->getMimeType() : _outputFormat;
		}



		ReservationsListService::ReservationsListService():
			FactorableTemplate<server::Function,ReservationsListService>(),
			_linkedWithVehicleOnly(false),
			_useCache(false)
		{
			if(!_useCache)
			{
				setEnv(shared_ptr<Env>(new Env));
			}
		}
}	}
