
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

#include "DeadRunTableSync.hpp"
#include "DriverActivity.hpp"
#include "DriverActivityTableSync.hpp"
#include "OperationUnit.hpp"
#include "Profile.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "StopPoint.hpp"
#include "TransportNetwork.h"
#include "VehicleService.hpp"
#include "VehicleServiceTableSync.hpp"
#include "User.h"

#include <sstream>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace pt_operation;
	using namespace util;

	CLASS_DEFINITION(DriverService, "t081_driver_services", 81)
	FIELD_DEFINITION_OF_OBJECT(DriverService, "driver_service_id", "driver_service_ids")

	FIELD_DEFINITION_OF_TYPE(DriverServiceServices, "services", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DriverServiceDates, "dates", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DriverServiceDataSource, "datasource_links", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DriverServiceOperationUnit, "operation_unit_id", SQL_INTEGER)

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
			impex::ImportableTemplate<DriverService>(),
			Object<DriverService, DriverServiceSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(DriverServiceServices),
					FIELD_DEFAULT_CONSTRUCTOR(DriverServiceDates),
					FIELD_DEFAULT_CONSTRUCTOR(DriverServiceDataSource),
					FIELD_DEFAULT_CONSTRUCTOR(DriverServiceOperationUnit)
			)	)
		{}



		void DriverService::setChunks(const Chunks& value )
		{
			_chunks = value;
			BOOST_FOREACH(Chunk& chunk, _chunks)
			{
				chunk.driverService = this;
			}

			set<DriverServiceServices>(SerializeServices(_chunks));
		}



		void DriverService::toParametersMap(
			ParametersMap& map,
			bool recursive,
			const VehicleService* vehicleServiceFilter
		) const	{

			map.insert(Key::FIELD.name, getKey());
			map.insert(Name::FIELD.name, getName());
			map.insert(OperationUnit::FIELD.name, getOperationUnit() ? getOperationUnit()->getKey() : 0);

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
				const Services::Type& services(_vehicleService->get<Services>());
				BOOST_FOREACH(const Services::Type::value_type& service, services)
				{
					recursive_mutex::scoped_lock lock(service->getSchedulesMutex());

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

		std::string DriverService::SerializeServices( const DriverService::Chunks& services )
		{
			stringstream servicesStr;
			bool firstService(true);
			BOOST_FOREACH(const Chunk& chunk, services)
			{
				bool firstElement(true);
				if(chunk.elements.empty())
				{
					if(firstService)
					{
						firstService = false;
					}
					else
					{
						servicesStr << ",";
					}
					servicesStr <<
						"0:0:0:" <<
						(chunk.activity ? chunk.activity->getKey() : RegistryKeyType(0))
					;
					if(	!chunk.driverStartTime.is_not_a_date_time() &&
						!chunk.driverEndTime.is_not_a_date_time()
					){
						servicesStr << ":" << to_simple_string(chunk.driverStartTime);
						servicesStr << ":" << to_simple_string(chunk.driverEndTime);
					}
				}
				else
				{
					BOOST_FOREACH(const Chunk::Element& service, chunk.elements)
					{
						if(!service.service)
						{
							Log::GetInstance().warn("Null service in driver service has been ignored");
							continue;
						}
						if(firstService)
						{
							firstService = false;
						}
						else
						{
							servicesStr << ",";
						}
						servicesStr <<
							service.service->getKey() << ":" <<
							service.startRank << ":" <<
							service.endRank
						;
						if(firstElement)
						{
							firstElement = false;
							servicesStr <<
								":" <<
								(chunk.vehicleService ? chunk.vehicleService->getKey() : RegistryKeyType(0))
							;
							if(	!chunk.driverStartTime.is_not_a_date_time() &&
								!chunk.driverEndTime.is_not_a_date_time()
							){
								servicesStr << ":" << to_simple_string(chunk.driverStartTime);
								servicesStr << ":" << to_simple_string(chunk.driverEndTime);
							}
						}
			}	}	}
			return servicesStr.str();
		}

		DriverService::Chunks DriverService::UnserializeServices(
			const std::string& value,
			util::Env& env,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			vector<string> servicesStrs;
			if(!value.empty())
			{
				split(servicesStrs, value, is_any_of(","));
			}

			Chunks services;

			Chunks::reverse_iterator itServices(services.rend());
			BOOST_FOREACH(const string& elementStr, servicesStrs)
			{
				if(elementStr.empty())
				{
					continue;
				}
				vector<string> elementStrs;
				split(elementStrs, elementStr, is_any_of(":"));

				if(elementStrs.size() < 3)
				{
					continue;
				}

				if(elementStrs.size() >= 4 || itServices == services.rend())
				{
					Chunk chunk;
					if(	elementStrs.size() >= 4)
					{
						RegistryKeyType activityVehicleServiceId(
							lexical_cast<RegistryKeyType>(elementStrs[3])
						);
						if(activityVehicleServiceId > 0)
						{
							if(decodeTableId(activityVehicleServiceId) == VehicleServiceTableSync::TABLE.ID) try
							{
								chunk.vehicleService = VehicleServiceTableSync::GetEditable(
									activityVehicleServiceId,
									env,
									linkLevel
								).get();
							}
							catch (ObjectNotFoundException<VehicleService>&)
							{
								Log::GetInstance().warn("No such vehicle service "+ elementStrs[3]);
							}
							else if(decodeTableId(activityVehicleServiceId) == DriverActivityTableSync::TABLE.ID) try
							{
								chunk.activity = DriverActivityTableSync::GetEditable(
									activityVehicleServiceId,
									env,
									linkLevel
								).get();
							}
							catch (ObjectNotFoundException<DriverActivity>&)
							{
								Log::GetInstance().warn("No such activity "+ elementStrs[3]);
							}
					}	}
					if(elementStrs.size() >= 9)
					{
						chunk.driverStartTime =
							hours(lexical_cast<long>(elementStrs[4])) +
							minutes(lexical_cast<long>(elementStrs[5]));
						chunk.driverEndTime =
							hours(lexical_cast<long>(elementStrs[7])) +
							minutes(lexical_cast<long>(elementStrs[8]));
					}
					services.push_back(chunk);
					itServices = services.rbegin();
				}

				try
				{
					if(lexical_cast<RegistryKeyType>(elementStrs[0]) > 0)
					{
						Chunk::Element element;

						RegistryKeyType id(lexical_cast<RegistryKeyType>(elementStrs[0]));
						if(decodeTableId(id) == ScheduledServiceTableSync::TABLE.ID)
						{
							element.service = ScheduledServiceTableSync::GetEditable(id, env, linkLevel).get();
						}
						else if(decodeTableId(id) == DeadRunTableSync::TABLE.ID)
						{
							element.service = DeadRunTableSync::GetEditable(id, env, linkLevel).get();
						}
						element.startRank = lexical_cast<size_t>(elementStrs[1]);
						element.endRank = lexical_cast<size_t>(elementStrs[2]);

						itServices->elements.push_back(element);
				}	}
				catch(ObjectNotFoundException<ScheduledService>&)
				{
					Log::GetInstance().warn("No such service "+ elementStrs[0]);
				}
				catch(ObjectNotFoundException<DeadRun>&)
				{
					Log::GetInstance().warn("No such dead run "+ elementStrs[0]);
				}
				catch(bad_lexical_cast&)
				{
					Log::GetInstance().warn("Inconsistent service id "+ elementStrs[0]);
				}
			}

			return services;
		}

		void DriverService::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Dates
			setFromSerializedString(get<DriverServiceDates>());

			// Services
			_chunks = UnserializeServices(
				get<DriverServiceServices>(),
				env
			);
			BOOST_FOREACH(Chunk& chunk, _chunks)
			{
				chunk.driverService = this;
			}
			BOOST_FOREACH(const DriverService::Chunk& chunk, getChunks())
			{
				if(chunk.vehicleService)
				{
					chunk.vehicleService->addDriverServiceChunk(chunk);
				}
			}
		}

		void DriverService::unlink()
		{
			BOOST_FOREACH(const DriverService::Chunk& chunk, getChunks())
			{
				if(chunk.vehicleService)
				{
					chunk.vehicleService->removeDriverServiceChunk(chunk);
				}
			}
		}

		const boost::optional<OperationUnit&> DriverService::getOperationUnit() const
		{
			boost::optional<OperationUnit&> value = boost::optional<OperationUnit&>();
			if (get<DriverServiceOperationUnit>())
			{
				value = get<DriverServiceOperationUnit>().get();
			}
			return value;
		}

		void DriverService::setOperationUnit(const boost::optional<OperationUnit&>& value)
		{
			set<DriverServiceOperationUnit>(value
				? boost::optional<OperationUnit&>(*value)
				: boost::none);
		}

		bool DriverService::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DriverService::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DriverService::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
