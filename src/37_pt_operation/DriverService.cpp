
/** DriverService class implementation.
	@file DriverService.cpp

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

#include "DriverService.hpp"

#include "DriverActivity.hpp"
#include "ScheduledService.h"
#include "StopPoint.hpp"
#include "VehicleService.hpp"

using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace pt_operation;
	using namespace util;

	namespace util
	{
		template<>
		const std::string Registry<pt_operation::DriverService>::KEY("DriverService");
	}

	FIELD_DEFINITION_OF_OBJECT(DriverService, "driver_service_id", "driver_service_ids")

	namespace pt_operation
	{
		const std::string DriverService::TAG_CHUNK = "chunk";
		const std::string DriverService::TAG_ELEMENT = "element";
		const std::string DriverService::TAG_VEHICLE_SERVICE = "vehicle_service";
		const std::string DriverService::TAG_ACTIVITY = "activity";
		const std::string DriverService::ATTR_START_TIME = "start_time";
		const std::string DriverService::ATTR_DRIVER_START_TIME = "driver_start_time";
		const std::string DriverService::ATTR_END_TIME = "end_time";
		const std::string DriverService::ATTR_DRIVER_END_TIME = "driver_end_time";
		const std::string DriverService::ATTR_START_STOP = "start_stop";
		const std::string DriverService::ATTR_END_STOP = "end_stop";
		const std::string DriverService::ATTR_WORK_DURATION = "work_duration";
		const std::string DriverService::ATTR_WORK_RANGE = "work_range";
		const std::string DriverService::ATTR_SERVICE_ID = "service_id";
		const std::string DriverService::ATTR_CLASS = "class";
		const std::string DriverService::VALUE_DEAD_RUN = "dead_run";
		const std::string DriverService::VALUE_COMMERCIAL = "commercial";



		DriverService::DriverService(util::RegistryKeyType id):
			util::Registrable(id),
			impex::ImportableTemplate<DriverService>()
		{}



		void DriverService::setChunks(const Chunks& value )
		{
			_chunks = value;
			BOOST_FOREACH(Chunk& chunk, _chunks)
			{
				chunk.driverService = this;
			}
		}



		void DriverService::toParametersMap(
			ParametersMap& map,
			bool recursive,
			const VehicleService* vehicleServiceFilter
		) const	{

			map.insert(Key::FIELD.name, getKey());
			map.insert(Name::FIELD.name, getName());

			// Service times
			map.insert(ATTR_WORK_DURATION, getWorkDuration());
			map.insert(ATTR_WORK_RANGE, getWorkRange());
			map.insert(ATTR_DRIVER_START_TIME, getServiceBeginning());
			map.insert(ATTR_DRIVER_END_TIME, getServiceEnd());
			if(	!getChunks().empty()
			){
				// Stops
				if(!getChunks().begin()->elements.empty())
				{
					const Importable* startStopPoint(
						dynamic_cast<Importable*>(
							getChunks().begin()->elements.begin()->service->getPath()->getEdge(
								getChunks().begin()->elements.begin()->startRank
							)->getFromVertex()
					)	);
					if(startStopPoint)
					{
						map.insert(ATTR_START_STOP, startStopPoint->getCodeBySources());
					}
				}
				if(!getChunks().rbegin()->elements.empty())
				{
					const Importable* endStopPoint(
						dynamic_cast<Importable*>(
							getChunks().rbegin()->elements.rbegin()->service->getPath()->getEdge(
								getChunks().rbegin()->elements.rbegin()->endRank
							)->getFromVertex()
					)	);
					if(endStopPoint)
					{
						map.insert(ATTR_END_STOP, endStopPoint->getCodeBySources());
					}
				}
			}

			BOOST_FOREACH(const DriverService::Chunks::value_type& chunk, getChunks())
			{
				// Vehicle service filter
				if(vehicleServiceFilter && chunk.vehicleService != vehicleServiceFilter)
				{
					continue;
				}

				// Declarations
				boost::shared_ptr<ParametersMap> chunkPM(new ParametersMap);

				// Times
				if(!chunk.elements.empty())
				{
					chunkPM->insert(
						ATTR_START_TIME,
						chunk.elements.begin()->service->getDepartureSchedule(false, chunk.elements.begin()->startRank)
					);
					chunkPM->insert(
						ATTR_END_TIME,
						chunk.elements.rbegin()->service->getArrivalSchedule(false, chunk.elements.rbegin()->endRank)
					);
				}
				chunkPM->insert(ATTR_DRIVER_START_TIME, chunk.getDriverStartTime());
				chunkPM->insert(ATTR_DRIVER_END_TIME, chunk.getDriverEndTime());

				// Vehicle service
				if(recursive && chunk.vehicleService)
				{
					boost::shared_ptr<ParametersMap> vsPM(new ParametersMap);
					chunk.vehicleService->toParametersMap(*vsPM, false);
					chunkPM->insert(TAG_VEHICLE_SERVICE, vsPM);
				}

				// Activity
				if(recursive && chunk.activity)
				{
					boost::shared_ptr<ParametersMap> activityPM(new ParametersMap);
					chunk.activity->toParametersMap(*activityPM, false);
					chunkPM->insert(TAG_ACTIVITY, activityPM);
				}

				// Stops
				if(!chunk.elements.empty())
				{
					const Importable* startStopPoint(dynamic_cast<Importable*>(chunk.elements.begin()->service->getPath()->getEdge(chunk.elements.begin()->startRank)->getFromVertex()));
					if(startStopPoint)
					{
						chunkPM->insert(ATTR_START_STOP, startStopPoint->getCodeBySources());
					}
					const Importable* endStopPoint(dynamic_cast<Importable*>(chunk.elements.rbegin()->service->getPath()->getEdge(chunk.elements.rbegin()->endRank)->getFromVertex()));
					if(endStopPoint)
					{
						chunkPM->insert(ATTR_END_STOP, endStopPoint->getCodeBySources());
					}
				}

				// Elements
				BOOST_FOREACH(const DriverService::Chunk::Element& element, chunk.elements)
				{
					// Declaration
					boost::shared_ptr<ParametersMap> elementPM(new ParametersMap);

					// Service
					elementPM->insert(ATTR_SERVICE_ID, element.service->getKey());
					elementPM->insert(ATTR_CLASS, dynamic_cast<ScheduledService*>(element.service) ? VALUE_COMMERCIAL : VALUE_DEAD_RUN);
					elementPM->insert(ATTR_START_TIME, element.service->getDepartureSchedule(false, element.startRank));
					elementPM->insert(ATTR_END_TIME, element.service->getArrivalSchedule(false, element.endRank));

					const Importable* startStopPoint(dynamic_cast<Importable*>(element.service->getPath()->getEdge(element.startRank)->getFromVertex()));
					if(startStopPoint)
					{
						elementPM->insert(ATTR_START_STOP, startStopPoint->getCodeBySources());
					}
					const Importable* endStopPoint(dynamic_cast<Importable*>(element.service->getPath()->getEdge(element.endRank)->getFromVertex()));
					if(endStopPoint)
					{
						elementPM->insert(ATTR_END_STOP, endStopPoint->getCodeBySources());
					}

					// Storage in chunk map
					chunkPM->insert(TAG_ELEMENT, elementPM);
				}

				map.insert(TAG_CHUNK, chunkPM);
			}
		}



		void DriverService::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{
			toParametersMap(
				pm,
				true,
				NULL
			);
		}



		time_duration DriverService::getWorkRange() const
		{
			return getServiceEnd() - getServiceBeginning();
		}



		boost::posix_time::time_duration DriverService::getWorkDuration() const
		{
			time_duration result(minutes(0));
			BOOST_FOREACH(const DriverService::Chunks::value_type& chunk, getChunks())
			{
				result += (chunk.getDriverEndTime() - chunk.getDriverStartTime());
			}
			return result;
		}



		time_duration DriverService::getServiceBeginning() const
		{
			if(_chunks.empty())
			{
				return time_duration(not_a_date_time);
			}
			return _chunks.begin()->getDriverStartTime();
		}



		time_duration DriverService::getServiceEnd() const
		{
			if(_chunks.empty())
			{
				return time_duration(not_a_date_time);
			}
			return _chunks.rbegin()->getDriverEndTime();
		}



		DriverService::Chunk::Chunk(
			DriverService* _driverService,
			VehicleService* _vehicleService,
			DriverActivity* _activity,
			const boost::gregorian::date& date,
			const boost::posix_time::time_duration& startTime,
			const boost::posix_time::time_duration& endTime,
			const boost::posix_time::time_duration& startTimeD,
			const boost::posix_time::time_duration& endTimeD
		):	driverService(_driverService),
			vehicleService(_vehicleService),
			activity(_activity),
			driverStartTime(startTimeD),
			driverEndTime(endTimeD)
		{
			if(_vehicleService)
			{
				const VehicleService::Services& services(_vehicleService->getServices());
				BOOST_FOREACH(const VehicleService::Services::value_type& service, services)
				{
					if(	!service->isActive(date) ||
						service->getLastArrivalSchedule(false) < startTime
					){
						continue;
					}
					if(service->getDepartureSchedule(false, 0) > endTime)
					{
						break;
					}

					// Add service to chunk
					Chunk::Element element;
					element.service = service;
					for(size_t i(0); i<service->getDepartureSchedules(true, false).size(); ++i)
					{
						if(service->getDepartureSchedule(false, i) >= startTime)
						{
							element.startRank = i;
							break;
						}
					}
					for(size_t i(service->getArrivalSchedules(true, false).size()); i>0; --i)
					{
						if(service->getArrivalSchedule(false, i-1) <= endTime)
						{
							element.endRank = i-1;
							break;
						}
					}
					elements.push_back(element);
			}	}
		}



		DriverService::Chunk::Chunk(
			VehicleService* _vehicleService
		):	driverService(NULL),
			vehicleService(_vehicleService),
			activity(NULL),
			driverStartTime(not_a_date_time),
			driverEndTime(not_a_date_time)
		{}



		boost::posix_time::time_duration DriverService::Chunk::getDriverStartTime() const
		{
			if(!driverStartTime.is_not_a_date_time())
			{
				return driverStartTime;
			}
			if(!elements.empty())
			{
				return elements.begin()->service->getDepartureSchedule(false, elements.begin()->startRank);
			}
			return time_duration(not_a_date_time);
		}



		boost::posix_time::time_duration DriverService::Chunk::getDriverEndTime() const
		{
			if(!driverEndTime.is_not_a_date_time())
			{
				return driverEndTime;
			}
			if(!elements.empty())
			{
				return elements.rbegin()->service->getArrivalSchedule(false, elements.rbegin()->endRank);
			}
			return time_duration(not_a_date_time);
		}
}	}
