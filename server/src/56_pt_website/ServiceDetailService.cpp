
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
#include "DescentTableSync.hpp"
#include "DRTArea.hpp"
#include "Language.hpp"
#include "LineStop.h"
#include "PTModule.h"
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
	using namespace vehicle;

	template<>
	const string FactorableTemplate<Function,pt_website::ServiceDetailService>::FACTORY_KEY = "service_detail";
	
	namespace pt_website
	{
		const string ServiceDetailService::TAG_SERVICE_DETAIL = "service_detail";
		const string ServiceDetailService::PARAMETER_READ_RESERVATIONS_FROM_DAY = "read_reservations_from_day";
		const string ServiceDetailService::PARAMETER_READ_DESCENTS_FROM_DAY = "read_descents_from_day";
		const string ServiceDetailService::PARAMETER_BASE_CALENDAR_ID = "base_calendar_id";

		const string ServiceDetailService::TAG_STOP = "stop";
		const string ServiceDetailService::ATTR_CITY_ID = "city_id";
		const string ServiceDetailService::ATTR_CITY_NAME = "city_name";
		const string ServiceDetailService::ATTR_STOP_NAME = "stop_name";
		const string ServiceDetailService::ATTR_DEPARTURE_TIME = "departure_time";
		const string ServiceDetailService::ATTR_ARRIVAL_TIME = "arrival_time";
		const string ServiceDetailService::ATTR_SCHEDULE_INPUT = "schedule_input";
		const string ServiceDetailService::ATTR_WITH_RESERVATION = "with_reservation";
		const string ServiceDetailService::ATTR_WITH_DESCENT = "with_descent";
		const string ServiceDetailService::ATTR_DESCENT_ID = "descent_id";
		const string ServiceDetailService::ATTR_FIRST_IN_AREA = "first_in_area";
		const string ServiceDetailService::ATTR_LAST_IN_AREA = "last_in_area";
		const string ServiceDetailService::ATTR_IS_AREA = "is_area";
		const string ServiceDetailService::ATTR_IS_RESERVABLE = "is_reservable";
		const string ServiceDetailService::ATTR_RANK = "rank";
		const string ServiceDetailService::ATTR_STOP_POINT_ID = "stop_point_id";
		const string ServiceDetailService::ATTR_STOP_AREA_ID = "stop_area_id";
		const string ServiceDetailService::TAG_RESERVATION_WITH_ARRIVAL_BEFORE_DEPARTURE = "reservation_with_arrival_before_departure";
		
		const string ServiceDetailService::TAG_CALENDAR = "calendar";

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
			
			// Read descents from day
			string rdfdtext(map.getDefault<string>(PARAMETER_READ_DESCENTS_FROM_DAY));
			if(!rdfdtext.empty())
			{
				_readDescentsFromDay = from_string(rdfdtext);
			}
			
			Function::setOutputFormatFromMap(map,string());
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
			map.insert(ATTR_IS_RESERVABLE, serviceIsReservable);

			// Reservations
			Resas resas;
			if(	serviceIsReservable &&
				!_readReservationsFromDay.is_not_a_date()
			){
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
					resas.push_back(
						make_pair(
							resa.get(),
							false
					)	);
				}
			}
			
			// Descents
			Descents descents;
			if(	!_readDescentsFromDay.is_not_a_date() )
			{
				date date2(_readDescentsFromDay);
				date2 += days(1);
				DescentTableSync::SearchResult descentsTable(
					DescentTableSync::Search(
						*_env,
						_service->getKey(),
						_readDescentsFromDay,
						date2
				)	);
				
				BOOST_FOREACH(const DescentTableSync::SearchResult::value_type& descent, descentsTable)
				{
					descents.push_back(
						make_pair(
							descent.get(),
							false
					)	);
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
					JourneyPattern::LineStops::const_iterator itLineStop2(itLineStop);
					++itLineStop2;

					_exportStop(
						map,
						*dynamic_cast<const StopPoint*>(&*lineStop.get<LineNode>())->getConnectionPlace(),
						lineStop.get<IsDeparture>() && itLineStop2 != lineStops.end(),
						lineStop.get<IsArrival>() && itLineStop != lineStops.begin(),
						lineStop.get<RankInPath>(),
						lineStop.get<ScheduleInput>(),
						serviceIsReservable && lineStop.get<ReservationNeeded>(),
						resas,
						false,
						false,
						false,
						descents,
						lineStop.get<LineNode>()->getKey(),
						dynamic_cast<const StopPoint*>(&*lineStop.get<LineNode>())->getConnectionPlace()->getKey()
					);
				}
				else if(dynamic_cast<const DRTArea*>(&*lineStop.get<LineNode>()))
				{
					const DRTArea& area(dynamic_cast<const DRTArea&>(*lineStop.get<LineNode>()));
					if(lineStop.get<ReverseDRTArea>())
					{
						for(Stops::Type::const_reverse_iterator it(area.get<Stops>().rbegin());
							it != area.get<Stops>().rend();
							++it
						){
							Stops::Type::const_reverse_iterator it2(it);
							++it2;
							
							_exportStop(
								map,
								**it,
								lineStop.get<IsDeparture>(),
								lineStop.get<IsArrival>(),
								lineStop.get<RankInPath>(),
								lineStop.get<ScheduleInput>(),
								true,
								resas,
								true,
								it == area.get<Stops>().rbegin(),
								it2 == area.get<Stops>().rend(),
								descents,
								0,
								(**it).getKey()
							);
						}
					}
					else
					{
						for(Stops::Type::const_iterator it(area.get<Stops>().begin());
							it != area.get<Stops>().end();
							++it
						){
							Stops::Type::const_iterator it2(it);
							++it2;
							
							_exportStop(
								map,
								**it,
								lineStop.get<IsDeparture>(),
								lineStop.get<IsArrival>(),
								lineStop.get<RankInPath>(),
								lineStop.get<ScheduleInput>(),
								true,
								resas,
								true,
								it == area.get<Stops>().begin(),
								it2 == area.get<Stops>().end(),
								descents,
								0,
								(**it).getKey()
							);
						}
					}
				}
			}
			
			if (_outputFormat == MimeTypes::JSON)
			{
				map.outputJSON(stream, TAG_SERVICE_DETAIL);
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
			return getOutputMimeTypeFromOutputFormat();
		}



		ServiceDetailService::ServiceDetailService():
			_service(NULL)
		{
		}



		void ServiceDetailService::_exportStop(
			util::ParametersMap& pm,
			const StopArea& stopArea,
			bool isDeparture,
			bool isArrival,
			size_t rank,
			bool scheduleInput,
			bool withReservation,
			Resas& resas,
			bool isArea,
			bool firstInArea,
			bool lastInArea,
			Descents& descents,
			RegistryKeyType stopPointId,
			RegistryKeyType stopAreaId
		) const {
			boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);

			stopPM->insert(ATTR_CITY_ID, stopArea.getCity()->getKey());
			stopPM->insert(ATTR_CITY_NAME, stopArea.getCity()->getName());
			stopPM->insert(ATTR_STOP_NAME, stopArea.getName());
			if(isDeparture)
			{
				stopPM->insert(ATTR_DEPARTURE_TIME, _service->getDepartureSchedule(false, rank));
			}
			if(isArrival)
			{
				stopPM->insert(ATTR_ARRIVAL_TIME, _service->getArrivalSchedule(false, rank));
			}
			stopPM->insert(ATTR_SCHEDULE_INPUT, scheduleInput);

			if(withReservation)
			{
				stopPM->insert(ATTR_WITH_RESERVATION, true);

				// Get the reservations
				StopInstructions instructions(
					_hasToStop(stopArea, rank, resas)
				);

				// Export the reservations into the stop parameters map
				_exportReservations(*stopPM, instructions);
			}
			
			bool withDescent(false);
			RegistryKeyType descentId(0);
			BOOST_FOREACH(Descents::value_type& descent, descents)
			{
				BOOST_FOREACH(const graph::Vertex *vertex, stopArea.getVertices(PTModule::GRAPH_ID))
				{
					if (descent.first->get<Stop>() &&
						descent.first->get<Stop>()->getKey() == vertex->getKey())
					{
						withDescent = true;
						descentId = descent.first->get<Key>();
						break;
					}
				}
				if (withDescent)
				{
					break;
				}
			}
			if (withDescent)
			{
				stopPM->insert(ATTR_WITH_DESCENT, true);
				stopPM->insert(ATTR_DESCENT_ID, descentId);
			}
			else
			{
				stopPM->insert(ATTR_WITH_DESCENT, false);
			}

			stopPM->insert(ATTR_IS_AREA, isArea);
			stopPM->insert(ATTR_FIRST_IN_AREA, firstInArea);
			stopPM->insert(ATTR_LAST_IN_AREA, lastInArea);

			stopPM->insert(ATTR_RANK, rank);
			stopPM->insert(ATTR_STOP_POINT_ID, stopPointId);
			stopPM->insert(ATTR_STOP_AREA_ID, stopAreaId);

			pm.insert(TAG_STOP, stopPM);

		}

			
		ServiceDetailService::StopInstructions ServiceDetailService::_hasToStop(
			const StopArea& stopArea,
			size_t rank,
			Resas& resas
		) const	{
			
			StopInstructions result;
			ptime departureTime(
				_readReservationsFromDay,
				_service->getDepartureSchedule(false, rank)
			);
			ptime arrivalTime(
				_readReservationsFromDay,
				_service->getArrivalSchedule(false, rank)
			);
			BOOST_FOREACH(Resas::value_type& resa, resas)
			{
				if(	resa.first->get<DeparturePlaceId>() == stopArea.getKey() &&
					resa.first->get<DepartureTime>() == departureTime
				){
					result.get<0>().insert(resa.first);
					resa.second = true;
				}
				if(	resa.first->get<ArrivalPlaceId>() == stopArea.getKey() &&
					resa.first->get<ArrivalTime>() == arrivalTime
				){
					result.get<1>().insert(resa.first);
					if(!resa.second)
					{
						result.get<2>().insert(resa.first);
					}
				}
			}
			return result;
		}



		void ServiceDetailService::_exportReservations( util::ParametersMap& pm, const StopInstructions resas )
		{
			// Reservations at departure
			BOOST_FOREACH(const Reservation* resa, resas.get<0>())
			{
				boost::shared_ptr<ParametersMap> resaPM(new ParametersMap);
				resa->toParametersMap(*resaPM, boost::none);
				pm.insert(TAG_RESERVATION_AT_DEPARTURE, resaPM);
			}

			// Reservations at arrival
			BOOST_FOREACH(const Reservation* resa, resas.get<1>())
			{
				boost::shared_ptr<ParametersMap> resaPM(new ParametersMap);
				resa->toParametersMap(*resaPM, boost::none);
				pm.insert(TAG_RESERVATION_AT_ARRIVAL, resaPM);
			}

			// Alert on arrival before departure
			BOOST_FOREACH(const Reservation* resa, resas.get<2>())
			{
				boost::shared_ptr<ParametersMap> resaPM(new ParametersMap);
				resa->toParametersMap(*resaPM, boost::none);
				pm.insert(TAG_RESERVATION_WITH_ARRIVAL_BEFORE_DEPARTURE, resaPM);
			}
		}
}	}
