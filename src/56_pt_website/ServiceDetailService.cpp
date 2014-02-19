
//////////////////////////////////////////////////////////////////////////////////////////
///	ServiceDetailService class implementation.
///	@file ServiceDetailService.cpp
///	@author hromain
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ServiceDetailService.hpp"

#include "CalendarModule.h"
#include "CalendarTemplate.h"
#include "City.h"
#include "DRTArea.hpp"
#include "Language.hpp"
#include "LineStop.h"
#include "PTUseRule.h"
#include "RequestException.h"
#include "Request.h"
#include "ResaModule.h"
#include "ScheduledServiceTableSync.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace calendar;
	using namespace graph;
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace resa;

	template<>
	const string FactorableTemplate<Function,pt_website::ServiceDetailService>::FACTORY_KEY = "service_detail";
	
	namespace pt_website
	{
		const string ServiceDetailService::PARAMETER_READ_RESERVATIONS_FROM_DAY = "read_reservations_from_day";

		const string ServiceDetailService::TAG_STOP = "stop";
		const string ServiceDetailService::ATTR_CITY_ID = "city_id";
		const string ServiceDetailService::ATTR_CITY_NAME = "city_name";
		const string ServiceDetailService::ATTR_STOP_NAME = "stop_name";
		const string ServiceDetailService::ATTR_DEPARTURE_TIME = "departure_time";
		const string ServiceDetailService::ATTR_ARRIVAL_TIME = "arrival_time";
		const string ServiceDetailService::ATTR_SCHEDULE_INPUT = "schedule_input";
		const string ServiceDetailService::ATTR_WITH_RESERVATION = "with_reservation";
		const string ServiceDetailService::ATTR_FIRST_IN_AREA = "first_in_area";
		const string ServiceDetailService::ATTR_LAST_IN_AREA = "last_in_area";
		const string ServiceDetailService::ATTR_IS_AREA = "is_area";
		
		const string ServiceDetailService::ATTR_DEPARTURE_SCHEDULE = "departure_schedule";
		const string ServiceDetailService::ATTR_DEPARTURE_PLACE_NAME = "departure_place_name";
		const string ServiceDetailService::ATTR_ARRIVAL_SCHEDULE = "arrival_schedule";
		const string ServiceDetailService::ATTR_ARRIVAL_PLACE_NAME = "arrival_place_name";

		const string ServiceDetailService::TAG_RESERVATION_AT_DEPARTURE = "reservation_at_departure";
		const string ServiceDetailService::TAG_RESERVATION_AT_ARRIVAL = "reservation_at_arrival";


		ParametersMap ServiceDetailService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void ServiceDetailService::_setFromParametersMap(const ParametersMap& map)
		{
			if(decodeTableId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == ScheduledServiceTableSync::TABLE.ID)
			{
				try
				{
					_service = Env::GetOfficialEnv().get<ScheduledService>(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					).get();
				}
				catch(ObjectNotFoundException<ScheduledService>&)
				{
					throw RequestException("No such service");
				}
			}
			if(!_service)
			{
				throw RequestException("Unspecified service");
			}


			// Base calendar
			if(map.isDefined(PARAMETER_BASE_CALENDAR_ID))
			{
				// Load
				try
				{
					_baseCalendar = Env::GetOfficialEnv().get<CalendarTemplate>(
						map.get<RegistryKeyType>(PARAMETER_BASE_CALENDAR_ID)
					);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("No such calendar");
				}

				// Check if the calendar is usable
				if(!_baseCalendar->isLimited())
				{
					throw RequestException("This calendar is not usable as a base calendar");
				}
			}

			// Read reservations from day
			string rrfdtext(map.getDefault<string>(PARAMETER_READ_RESERVATIONS_FROM_DAY));
			if(!rrfdtext.empty())
			{
				_readReservationsFromDay = from_string(rrfdtext);
			}
		}

		ParametersMap ServiceDetailService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			// Standard export of the service
			_service->toParametersMap(map, true);

			// Departure schedule
			map.insert(
				ATTR_DEPARTURE_SCHEDULE,
				to_simple_string(_service->getDepartureSchedule(false, 0))
			);
		
			// Departure place name
			const Vertex* departureVertex(_service->getPath()->getEdge(0)->getFromVertex());
			map.insert(
				ATTR_DEPARTURE_PLACE_NAME,
				dynamic_cast<const StopPoint*>(departureVertex) ?
					dynamic_cast<const StopPoint*>(departureVertex)->getConnectionPlace()->getFullName() :
					dynamic_cast<const DRTArea*>(departureVertex)->getName()
			);

			// Arrival schedule
			map.insert(
				ATTR_ARRIVAL_SCHEDULE,
				to_simple_string(_service->getLastArrivalSchedule(false))
			);
			
			// Arrival place name
			const Vertex* arrivalVertex(_service->getPath()->getLastEdge()->getFromVertex());
			map.insert(
				ATTR_ARRIVAL_PLACE_NAME,
				dynamic_cast<const StopPoint*>(arrivalVertex) ?
					dynamic_cast<const StopPoint*>(arrivalVertex)->getConnectionPlace()->getFullName() :
					dynamic_cast<const DRTArea*>(arrivalVertex)->getName()
			);

			// Calendar
			if(_baseCalendar.get())
			{
				// Calendar analysis
				CalendarModule::BaseCalendar baseCalendar(
					CalendarModule::GetBestCalendarTitle(*_service, _baseCalendar->getResult())
				);

				// Output
				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				if(baseCalendar.first)
				{
					baseCalendar.first->toParametersMap(*calendarPM, true);
				}
				else
				{
					calendarPM->insert(CalendarTemplate::ATTR_NAME, baseCalendar.second);
				}
				map.insert(TAG_CALENDAR, calendarPM);
			}

			// Reservation rule
			const UseRule& useRule(_service->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
			bool serviceIsReservable(
				dynamic_cast<const PTUseRule*>(&useRule) &&
				static_cast<const PTUseRule&>(useRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
			);

			// Reservations
			Resas resas;
			if(	serviceIsReservable &&
				!_readReservationsFromDay.is_not_a_date()
			){
				// Use cache if for today
				if(_readReservationsFromDay == day_clock::local_day())
				{
					const ResaModule::ReservationsByService::mapped_type& reservationsEnv(
						ResaModule::GetReservationsByService(
							*_service
					)	);
					BOOST_FOREACH(const ResaModule::ReservationsByService::mapped_type::value_type& resa, reservationsEnv)
					{
						resas.push_back(resa);
					}
				}
				else // Read in database (slower)
				{
					// Reservations reading
					date date2(_readReservationsFromDay);
					date2 += days(1);
					ReservationTableSync::SearchResult reservationsTable(
						ReservationTableSync::Search(
							*_env,
							static_cast<const JourneyPattern*>(_service->getPath())->getCommercialLine()->getKey(),
							_readReservationsFromDay,
							date2,
							boost::none,
							false,
							true,
							true,
							0,
							boost::none,
							UP_LINKS_LOAD_LEVEL,
							_service->getKey()
					)	);
					BOOST_FOREACH(const ReservationTableSync::SearchResult::value_type& resa, reservationsTable)
					{
						resas.push_back(resa.get());
					}
				}
			}


			const JourneyPattern::LineStops& lineStops(static_cast<const JourneyPattern*>(_service->getPath())->getLineStops());

			// Stops loop
			for(JourneyPattern::LineStops::const_iterator itLineStop(lineStops.begin());
				itLineStop != lineStops.end();
				++itLineStop
			){
				const LineStop& lineStop(**itLineStop);
				if(dynamic_cast<const StopPoint*>(&*lineStop.get<LineNode>()))
				{
					boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
					const StopArea* stopArea(
						dynamic_cast<const StopPoint*>(&*lineStop.get<LineNode>())->getConnectionPlace()
					);

					stopPM->insert(ATTR_CITY_ID, stopArea->getCity()->getKey());
					stopPM->insert(ATTR_CITY_NAME, stopArea->getCity()->getName());
					stopPM->insert(ATTR_STOP_NAME, stopArea->getName());
					JourneyPattern::LineStops::const_iterator itLineStop2(itLineStop);
					++itLineStop2;
					if(lineStop.get<IsDeparture>() && itLineStop2 != lineStops.end())
					{
						stopPM->insert(ATTR_DEPARTURE_TIME, _service->getDepartureSchedule(false, lineStop.get<RankInPath>()));
					}
					if(lineStop.get<IsArrival>() && itLineStop != lineStops.begin())
					{
						stopPM->insert(ATTR_ARRIVAL_TIME, _service->getArrivalSchedule(false, lineStop.get<RankInPath>()));
					}
					stopPM->insert(ATTR_SCHEDULE_INPUT, lineStop.get<ScheduleInput>());

					if(serviceIsReservable && lineStop.get<ReservationNeeded>())
					{
						stopPM->insert(ATTR_WITH_RESERVATION, true);

						// Get the reservations
						StopInstructions instructions(
							_hasToStop(*stopArea, lineStop.get<RankInPath>(), resas)
						);

						// Export the reservations into the stop parameters map
						_exportReservations(*stopPM, instructions);
					}

					map.insert(TAG_STOP, stopPM);
				}
				else if(dynamic_cast<const DRTArea*>(&*lineStop.get<LineNode>()))
				{
					const DRTArea& area(dynamic_cast<const DRTArea&>(*lineStop.get<LineNode>()));
					for(Stops::Type::const_iterator it(area.get<Stops>().begin());
						it != area.get<Stops>().end();
						++it
					){
						const StopArea* stopArea(*it);

						boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
						
						stopPM->insert(ATTR_CITY_ID, stopArea->getCity()->getKey());
						stopPM->insert(ATTR_CITY_NAME, stopArea->getCity()->getName());
						stopPM->insert(ATTR_STOP_NAME, stopArea->getName());
						if(lineStop.get<IsDeparture>())
						{
							stopPM->insert(ATTR_DEPARTURE_TIME, _service->getDepartureSchedule(false, lineStop.get<RankInPath>()));
						}
						if(lineStop.get<IsArrival>())
						{
							stopPM->insert(ATTR_ARRIVAL_TIME, _service->getArrivalSchedule(false, lineStop.get<RankInPath>()));
						}
						stopPM->insert(ATTR_SCHEDULE_INPUT, lineStop.get<ScheduleInput>());
						stopPM->insert(ATTR_WITH_RESERVATION, true);

						// Get the reservations
						StopInstructions instructions(
							_hasToStop(*stopArea, lineStop.get<RankInPath>(), resas)
						);

						// Export the reservations into the stop parameters map
						_exportReservations(*stopPM, instructions);

						stopPM->insert(ATTR_FIRST_IN_AREA, it == area.get<Stops>().begin());
						Stops::Type::const_iterator it2(it);
						++it2;
						stopPM->insert(ATTR_LAST_IN_AREA, it2 == area.get<Stops>().end());
						stopPM->insert(ATTR_IS_AREA, true);

						map.insert(TAG_STOP, stopPM);
					}
				}
			}

			return map;
		}
		
		
		
		bool ServiceDetailService::isAuthorized(
			const Session* session
		) const {
			return true; // TODO enforce security
		}



		std::string ServiceDetailService::getOutputMimeType() const
		{
			return "text/html";
		}



		ServiceDetailService::ServiceDetailService():
			_service(NULL)
		{
		}


			
		ServiceDetailService::StopInstructions ServiceDetailService::_hasToStop(
			const StopArea& stopArea,
			size_t rank,
			const Resas& resas
		) const	{
			
			StopInstructions result(
				make_pair(
					StopInstructions::first_type(),
					StopInstructions::second_type()
			)	);
			ptime departureTime(
				_readReservationsFromDay,
				_service->getDepartureSchedule(false, rank)
			);
			ptime arrivalTime(
				_readReservationsFromDay,
				_service->getArrivalSchedule(false, rank)
			);
			BOOST_FOREACH(const Resas::value_type& resa, resas)
			{
				if(	resa->getDeparturePlaceId() == stopArea.getKey() &&
					resa->getDepartureTime() == departureTime
				){
					result.first.insert(resa);
				}
				if(	resa->getArrivalPlaceId() == stopArea.getKey() &&
					resa->getArrivalTime() == arrivalTime
				){
					result.second.insert(resa);
				}
			}
			return result;
		}



		void ServiceDetailService::_exportReservations( util::ParametersMap& pm, const StopInstructions resas )
		{
			// Reservations at departure
			BOOST_FOREACH(const StopInstructions::first_type::value_type& resa, resas.first)
			{
				boost::shared_ptr<ParametersMap> resaPM(new ParametersMap);
				resa->toParametersMap(*resaPM, boost::none);
				pm.insert(TAG_RESERVATION_AT_DEPARTURE, resaPM);
			}

			// Reservations at arrival
			BOOST_FOREACH(const StopInstructions::second_type::value_type& resa, resas.second)
			{
				boost::shared_ptr<ParametersMap> resaPM(new ParametersMap);
				resa->toParametersMap(*resaPM, boost::none);
				pm.insert(TAG_RESERVATION_AT_ARRIVAL, resaPM);
			}
		}
}	}
