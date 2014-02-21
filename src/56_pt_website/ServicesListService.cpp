
//////////////////////////////////////////////////////////////////////////////////////////
///	ServicesListService class implementation.
///	@file ServicesListService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "ServicesListService.hpp"

#include "CalendarModule.h"
#include "CalendarTemplate.h"
#include "City.h"
#include "CommercialLineTableSync.h"
#include "DRTArea.hpp"
#include "LineStop.h"
#include "OperationUnit.hpp"
#include "PTUseRule.h"
#include "RequestException.h"
#include "Request.h"
#include "ScheduledServiceTableSync.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "VehicleService.hpp"

using namespace boost;
using namespace boost::gregorian;
using namespace boost::logic;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace calendar;
	using namespace cms;
	using namespace geography;
	using namespace graph;
	using namespace pt;
	using namespace pt_operation;
	using namespace util;
	
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function, pt_website::ServicesListService>::FACTORY_KEY = "services_list";

	namespace pt_website
	{
		const string ServicesListService::PARAMETER_WAYBACK = "wayback";
		const string ServicesListService::PARAMETER_DISPLAY_DATE = "display_date";
		const string ServicesListService::PARAMETER_BASE_CALENDAR_ID = "base_calendar_id";
		const string ServicesListService::PARAMETER_DATE_FILTER = "date_filter";

		const string ServicesListService::DATA_ID = "id";
		const string ServicesListService::DATA_DEPARTURE_SCHEDULE = "departure_schedule";
		const string ServicesListService::DATA_DEPARTURE_PLACE_NAME = "departure_place_name";
		const string ServicesListService::DATA_ARRIVAL_SCHEDULE = "arrival_schedule";
		const string ServicesListService::DATA_ARRIVAL_PLACE_NAME = "arrival_place_name";
		const string ServicesListService::DATA_RUNS_AT_DATE = "runs_at_date";
		const string ServicesListService::DATA_SERVICE = "service";
		const string ServicesListService::TAG_SERVICES = "services";

		const string ServicesListService::ATTR_NUMBER = "number";
		const string ServicesListService::ATTR_PATH_ID = "path_id";
		
		const string ServicesListService::TAG_CALENDAR = "calendar";

		const string ServicesListService::TAG_STOP = "stop";
		const string ServicesListService::ATTR_CITY_ID = "city_id";
		const string ServicesListService::ATTR_CITY_NAME = "city_name";
		const string ServicesListService::ATTR_STOP_NAME = "stop_name";
		const string ServicesListService::ATTR_DEPARTURE_TIME = "departure_time";
		const string ServicesListService::ATTR_ARRIVAL_TIME = "arrival_time";
		const string ServicesListService::ATTR_SCHEDULE_INPUT = "schedule_input";
		const string ServicesListService::ATTR_WITH_RESERVATION = "with_reservation";
		const string ServicesListService::ATTR_FIRST_IN_AREA = "first_in_area";
		const string ServicesListService::ATTR_LAST_IN_AREA = "last_in_area";
		const string ServicesListService::ATTR_IS_AREA = "is_area";
		const string ServicesListService::PARAMETER_MIN_DEPARTURE_TIME = "min_departure_time";
		const string ServicesListService::PARAMETER_MAX_DEPARTURE_TIME = "max_departure_time";
		const string ServicesListService::PARAMETER_DEPARTURE_PLACE = "departure_place";


		ParametersMap ServicesListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void ServicesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Commercial line
			RegistryKeyType roid(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0));
			if(decodeTableId(roid) == CommercialLineTableSync::TABLE.ID)
			{
				try
				{
					_line = Env::GetOfficialEnv().get<CommercialLine>(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					);
				}
				catch(ObjectNotFoundException<CommercialLine>&)
				{
					throw RequestException("No such line");
				}
			}
			else if(decodeTableId(roid) == ScheduledServiceTableSync::TABLE.ID)
			{
				try
				{
					_service = Env::GetOfficialEnv().get<ScheduledService>(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					);
				}
				catch(ObjectNotFoundException<ScheduledService>&)
				{
					throw RequestException("No such service");
				}
			}
			else if(decodeTableId(roid) == OperationUnit::CLASS_NUMBER)
			{
				try
				{
					_operationUnit = Env::GetOfficialEnv().get<OperationUnit>(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
					);
				}
				catch(ObjectNotFoundException<OperationUnit>&)
				{
					throw RequestException("No such operation unit");
				}
			}


			// Base calendar
			if(map.isDefined(PARAMETER_DATE_FILTER))
			{
				date day(from_string(map.get<string>(PARAMETER_DATE_FILTER)));
				_baseCalendar = Calendar(day, day, days(1));
			}
			else if(map.isDefined(PARAMETER_BASE_CALENDAR_ID))
			{
				// Load
				try
				{
					shared_ptr<const CalendarTemplate> baseCalendar(
						Env::GetOfficialEnv().get<CalendarTemplate>(
							map.get<RegistryKeyType>(PARAMETER_BASE_CALENDAR_ID)
					)	);

					// Check if the calendar is usable
					if(!baseCalendar->isLimited())
					{
						throw RequestException("This calendar is not usable as a base calendar");
					}

					_baseCalendar = baseCalendar->getResult();
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("No such calendar");
				}
			}

			// Wayback
			_wayBack = tribool(indeterminate);
			if(!map.getDefault<string>(PARAMETER_WAYBACK).empty())
			{
				_wayBack = map.getDefault<bool>(PARAMETER_WAYBACK, false);
			}

			// Display date
			if(map.isDefined(PARAMETER_DISPLAY_DATE))
			{
				_displayDate = from_string(map.get<string>(PARAMETER_DISPLAY_DATE));
			}

			// Min departure time
			string minDepartureTimeStr(map.getDefault<string>(PARAMETER_MIN_DEPARTURE_TIME));
			if(!minDepartureTimeStr.empty())
			{
				_minDepartureTime = duration_from_string(minDepartureTimeStr);
			}

			// Max departure time
			string maxDepartureTimeStr(map.getDefault<string>(PARAMETER_MAX_DEPARTURE_TIME));
			if(!maxDepartureTimeStr.empty())
			{
				_maxDepartureTime = duration_from_string(maxDepartureTimeStr);
			}

			// Departure place id
			RegistryKeyType departurePlaceId(map.getDefault<RegistryKeyType>(PARAMETER_DEPARTURE_PLACE, 0));
			if(departurePlaceId)
			{
				_departurePlaceId = departurePlaceId;
			}
		}



		ParametersMap ServicesListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			ServiceSet result;

			if(_service)
			{
				result.insert(const_cast<ScheduledService*>(_service.get()));
			}
			else if(_line) // Loop on routes
			{
				_addServices(result, *_line);
			}
			else if(_operationUnit)
			{
				BOOST_FOREACH(const VehicleService::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<VehicleService>())
				{
					// Apply unit filter
					if(!it.second->getOperationUnit() || &*it.second->getOperationUnit() != _operationUnit.get())
					{
						continue;
					}

					// Get all services of the service
					BOOST_FOREACH(const VehicleService::Services::value_type& service, it.second->getServices())
					{
						_addServiceIfCompliant(result, *service);
					}
				}
			}
			else
			{
				BOOST_FOREACH(const CommercialLine::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<CommercialLine>())
				{
					_addServices(result, *it.second);
				}
			}

			// The map is filled
			BOOST_FOREACH(Service* service, result)
			{
				boost::shared_ptr<ParametersMap> serviceMap(new ParametersMap);
				serviceMap->insert(DATA_ID, service->getKey());
				if(!_displayDate.is_not_a_date() && dynamic_cast<NonPermanentService*>(service))
				{
					serviceMap->insert(
						DATA_RUNS_AT_DATE,
						dynamic_cast<NonPermanentService*>(service)->isActive(_displayDate)
					);
				}

				// Departure schedule
				if(dynamic_cast<SchedulesBasedService*>(service))
				{
					SchedulesBasedService& sservice(
						dynamic_cast<SchedulesBasedService&>(*service)
					);
					serviceMap->insert(DATA_DEPARTURE_SCHEDULE, to_simple_string(sservice.getDepartureSchedule(false, 0)));
				}

				// Departure place name
				const Vertex* departureVertex(service->getPath()->getEdge(0)->getFromVertex());
				serviceMap->insert(
					DATA_DEPARTURE_PLACE_NAME,
					dynamic_cast<const StopPoint*>(departureVertex) ?
					dynamic_cast<const StopPoint*>(departureVertex)->getConnectionPlace()->getFullName() :
					dynamic_cast<const DRTArea*>(departureVertex)->getName()
				);

				// Arrival schedule
				if(dynamic_cast<SchedulesBasedService*>(service))
				{
					SchedulesBasedService& sservice(
						dynamic_cast<SchedulesBasedService&>(*service)
					);
					serviceMap->insert(DATA_ARRIVAL_SCHEDULE, to_simple_string(sservice.getLastArrivalSchedule(false)));
				}

				serviceMap->insert(ATTR_NUMBER, service->getServiceNumber());
				serviceMap->insert(ATTR_PATH_ID, service->getPath()->getKey());

				// Arrival place name
				const Vertex* arrivalVertex(service->getPath()->getLastEdge()->getFromVertex());
				serviceMap->insert(
					DATA_ARRIVAL_PLACE_NAME,
					dynamic_cast<const StopPoint*>(arrivalVertex) ?
					dynamic_cast<const StopPoint*>(arrivalVertex)->getConnectionPlace()->getFullName() :
					dynamic_cast<const DRTArea*>(arrivalVertex)->getName()
				);

				if(dynamic_cast<SchedulesBasedService*>(service))
				{
					SchedulesBasedService& sservice(
						dynamic_cast<SchedulesBasedService&>(*service)
					);

					// Calendar
					if(_baseCalendar)
					{
						// Calendar analysis
						CalendarModule::BaseCalendar baseCalendar(
							CalendarModule::GetBestCalendarTitle(sservice, *_baseCalendar)
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
						serviceMap->insert(TAG_CALENDAR, calendarPM);
					}

					// Reservation rule
					const UseRule& useRule(service->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
					bool serviceIsReservable(
						dynamic_cast<const PTUseRule*>(&useRule) &&
						static_cast<const PTUseRule&>(useRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					);

					const JourneyPattern::LineStops& lineStops(static_cast<const JourneyPattern*>(sservice.getPath())->getLineStops());

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
								stopPM->insert(ATTR_DEPARTURE_TIME, sservice.getDepartureSchedule(false, lineStop.get<RankInPath>()));
							}
							if(lineStop.get<IsArrival>() && itLineStop != lineStops.begin())
							{
								stopPM->insert(ATTR_ARRIVAL_TIME, sservice.getArrivalSchedule(false, lineStop.get<RankInPath>()));
							}
							stopPM->insert(ATTR_SCHEDULE_INPUT, lineStop.get<ScheduleInput>());

							if(serviceIsReservable && lineStop.get<ReservationNeeded>())
							{
								stopPM->insert(ATTR_WITH_RESERVATION, true);
							}

							serviceMap->insert(TAG_STOP, stopPM);
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
									stopPM->insert(ATTR_DEPARTURE_TIME, sservice.getDepartureSchedule(false, lineStop.get<RankInPath>()));
								}
								if(lineStop.get<IsArrival>())
								{
									stopPM->insert(ATTR_ARRIVAL_TIME, sservice.getArrivalSchedule(false, lineStop.get<RankInPath>()));
								}
								stopPM->insert(ATTR_SCHEDULE_INPUT, lineStop.get<ScheduleInput>());
								stopPM->insert(ATTR_WITH_RESERVATION, true);

								stopPM->insert(ATTR_FIRST_IN_AREA, it == area.get<Stops>().begin());
								Stops::Type::const_iterator it2(it);
								++it2;
								stopPM->insert(ATTR_LAST_IN_AREA, it2 == area.get<Stops>().end());
								stopPM->insert(ATTR_IS_AREA, true);

								serviceMap->insert(TAG_STOP, stopPM);
							}
						}
					}
				}


				map.insert(DATA_SERVICE, serviceMap);
			}

			outputParametersMap(
				map,
				stream,
				TAG_SERVICES,
				"https://extranet.rcsmobility.com/attachments/download/14018/ServicesListService.xsd"
			);

			return map;
		}



		bool ServicesListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ServicesListService::getOutputMimeType() const
		{
			return getOutputMimeTypeFromOutputFormat();
		}



		ServicesListService::ServicesListService():
			_displayDate(day_clock::local_day())
		{}



		void ServicesListService::_addServices( graph::ServiceSet& result, const pt::CommercialLine& line ) const
		{
			BOOST_FOREACH(Path* path, line.getPaths())
			{
				// Jump over non regular paths
				if(!dynamic_cast<JourneyPattern*>(path))
				{
					continue;
				}
				const JourneyPattern& journeyPattern(
					static_cast<JourneyPattern&>(
						*path
				)	);

				// Applies wayback filter
				if(!indeterminate(_wayBack) && journeyPattern.getWayBack() != _wayBack)
				{
					continue;
				}

				// Departure place filter
				if(	_departurePlaceId &&
					dynamic_cast<const NamedPlace*>(journeyPattern.getEdge(0)->getFromVertex()->getHub())->getKey() != *_departurePlaceId
				){
					continue;
				}

				// Gets all services
				boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
					*journeyPattern.sharedServicesMutex
				);
				BOOST_FOREACH(Service* service, journeyPattern.getAllServices())
				{
					_addServiceIfCompliant(result, static_cast<SchedulesBasedService&>(*service));
				}
			}

		}



		void ServicesListService::_addServiceIfCompliant(
			graph::ServiceSet& result,
			const pt::SchedulesBasedService& service
		) const	{
			// Min departure time filter
			if(	_minDepartureTime &&
				service.getDataFirstDepartureSchedule(0) < *_minDepartureTime
			){
				return;
			}

			// Max departure time filter
			if(	_maxDepartureTime &&
				service.getDataFirstDepartureSchedule(0) > *_maxDepartureTime
			){
				return;
			}

			// Base calendar filter
			if(	_baseCalendar &&
				dynamic_cast<const NonPermanentService*>(&service) &&
				(*_baseCalendar & static_cast<const NonPermanentService&>(service)).empty()
			){
				return;
			}
			
			result.insert(const_cast<Service*>(static_cast<const Service*>(&service)));
		}
}	}
