
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
#include "RequestException.h"
#include "Request.h"
#include "ScheduledServiceTableSync.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

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
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function, pt::ServicesListService>::FACTORY_KEY = "services_list";

	namespace pt
	{
		const string ServicesListService::PARAMETER_WAYBACK = "wayback";
		const string ServicesListService::PARAMETER_DISPLAY_DATE = "display_date";
		const string ServicesListService::PARAMETER_BASE_CALENDAR_ID = "base_calendar_id";
		
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
			if(decodeTableId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == CommercialLineTableSync::TABLE.ID)
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

			if(decodeTableId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) == ScheduledServiceTableSync::TABLE.ID)
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

			if(_service.get())
			{
				result.insert(const_cast<ScheduledService*>(_service.get()));
			}

			// Loop on routes
			if(_line.get())
			{
				BOOST_FOREACH(Path* path, _line->getPaths())
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
						// Min departure time filter
						if(	_minDepartureTime &&
							static_cast<SchedulesBasedService*>(service)->getDataFirstDepartureSchedule(0) < *_minDepartureTime
						){
							continue;
						}

						// Max departure time filter
						if(	_maxDepartureTime &&
							static_cast<SchedulesBasedService*>(service)->getDataFirstDepartureSchedule(0) > *_maxDepartureTime
						){
							continue;
						}

						result.insert(service);
					}
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
					if(_baseCalendar.get())
					{
						// Calendar analysis
						CalendarModule::BaseCalendar baseCalendar(
							CalendarModule::GetBestCalendarTitle(sservice, _baseCalendar->getResult())
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

					// Stops
					BOOST_FOREACH(const JourneyPattern::LineStops::value_type& lineStop, static_cast<const JourneyPattern*>(sservice.getPath())->getLineStops())
					{
						if(dynamic_cast<const StopPoint*>(&*lineStop->get<LineNode>()))
						{
							boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
							const StopArea* stopArea(
								dynamic_cast<const StopPoint*>(&*lineStop->get<LineNode>())->getConnectionPlace()
							);

							stopPM->insert(ATTR_CITY_ID, stopArea->getCity()->getKey());
							stopPM->insert(ATTR_CITY_NAME, stopArea->getCity()->getName());
							stopPM->insert(ATTR_STOP_NAME, stopArea->getName());
							if(lineStop->get<IsDeparture>())
							{
								stopPM->insert(ATTR_DEPARTURE_TIME, sservice.getDepartureSchedule(false, lineStop->get<RankInPath>()));
							}
							if(lineStop->get<IsArrival>())
							{
								stopPM->insert(ATTR_ARRIVAL_TIME, sservice.getArrivalSchedule(false, lineStop->get<RankInPath>()));
							}
							stopPM->insert(ATTR_SCHEDULE_INPUT, lineStop->get<ScheduleInput>());
							stopPM->insert(ATTR_WITH_RESERVATION, lineStop->get<ReservationNeeded>());

							serviceMap->insert(TAG_STOP, stopPM);
						}
						else if(dynamic_cast<const DRTArea*>(&*lineStop->get<LineNode>()))
						{
							const DRTArea& area(dynamic_cast<const DRTArea&>(*lineStop->get<LineNode>()));
							for(Stops::Type::const_iterator it(area.get<Stops>().begin());
								it != area.get<Stops>().end();
								++it
							){
								const StopArea* stopArea(*it);

								boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
								
								stopPM->insert(ATTR_CITY_ID, stopArea->getCity()->getKey());
								stopPM->insert(ATTR_CITY_NAME, stopArea->getCity()->getName());
								stopPM->insert(ATTR_STOP_NAME, stopArea->getName());
								if(lineStop->get<IsDeparture>())
								{
									stopPM->insert(ATTR_DEPARTURE_TIME, sservice.getDepartureSchedule(false, lineStop->get<RankInPath>()));
								}
								if(lineStop->get<IsArrival>())
								{
									stopPM->insert(ATTR_ARRIVAL_TIME, sservice.getArrivalSchedule(false, lineStop->get<RankInPath>()));
								}
								stopPM->insert(ATTR_SCHEDULE_INPUT, lineStop->get<ScheduleInput>());
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
}	}
