
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

#include "SchedulesBasedService.h"
#include "StopPoint.hpp"
#include "VehicleService.hpp"

using namespace boost;

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
		DriverService::DriverService(util::RegistryKeyType id):
			impex::ImportableTemplate<DriverService>(),
			util::Registrable(id)
		{
		}



		void DriverService::setChunks(const Chunks& value )
		{
			_chunks = value;
			BOOST_FOREACH(Chunk& chunk, _chunks)
			{
				chunk.driverService = this;
			}
		}



		void DriverService::toParametersMap( util::ParametersMap& map ) const
		{
			map.insert("id", getKey());

			BOOST_FOREACH(const DriverService::Chunks::value_type& chunk, getChunks())
			{
				if(chunk.elements.empty())
				{
					continue;
				}

				shared_ptr<ParametersMap> chunkPM(new ParametersMap);
				chunkPM->insert("start_time", chunk.elements.begin()->service->getDepartureSchedule(false, chunk.elements.begin()->startRank));
				chunkPM->insert("end_time", chunk.elements.rbegin()->service->getArrivalSchedule(false, chunk.elements.rbegin()->endRank));
				const StopPoint* startStopPoint(dynamic_cast<StopPoint*>(chunk.elements.begin()->service->getPath()->getEdge(chunk.elements.begin()->startRank)->getFromVertex()));
				if(startStopPoint)
				{
					chunkPM->insert("start_stop", startStopPoint->getCodeBySources());
				}
				const StopPoint* endStopPoint(dynamic_cast<StopPoint*>(chunk.elements.rbegin()->service->getPath()->getEdge(chunk.elements.begin()->endRank)->getFromVertex()));
				if(endStopPoint)
				{
					chunkPM->insert("end_stop", endStopPoint->getCodeBySources());
				}

				BOOST_FOREACH(const DriverService::Chunk::Element& element, chunk.elements)
				{
					shared_ptr<ParametersMap> elementPM(new ParametersMap);
					elementPM->insert("service_id", element.service->getKey());
					elementPM->insert("start_time", element.service->getDepartureSchedule(false, element.startRank));
					elementPM->insert("end_time", element.service->getArrivalSchedule(false, element.endRank));

					const StopPoint* startStopPoint(dynamic_cast<StopPoint*>(element.service->getPath()->getEdge(element.startRank)->getFromVertex()));
					if(startStopPoint)
					{
						elementPM->insert("start_stop", startStopPoint->getCodeBySources());
					}
					const StopPoint* endStopPoint(dynamic_cast<StopPoint*>(element.service->getPath()->getEdge(element.endRank)->getFromVertex()));
					if(endStopPoint)
					{
						elementPM->insert("end_stop", endStopPoint->getCodeBySources());
					}
				}

				map.insert("chunk", chunkPM);
			}
		}



		DriverService::Chunk::Chunk(
			DriverService* _driverService,
			VehicleService& _vehicleService,
			const boost::posix_time::time_duration& startTime,
			const boost::posix_time::time_duration& endTime
		):	driverService(_driverService),
			vehicleService(&_vehicleService)
		{
			const VehicleService::Services& services(_vehicleService.getServices());
			BOOST_FOREACH(const VehicleService::Services::value_type& service, services)
			{
				if(service->getLastArrivalSchedule(false) < startTime)
				{
					continue;
				}
				if(service->getDepartureSchedule(false, 0) > endTime)
				{
					break;
				}

				// Add service to chunk
				Chunk::Element element;
				element.service = service;
				for(size_t i(0); i<service->getDepartureSchedules(false).size(); ++i)
				{
					if(service->getDepartureSchedule(false, i) >= startTime)
					{
						element.startRank = i;
						break;
					}
				}
				for(size_t i(service->getArrivalSchedules(false).size()-1); i<=0; --i)
				{
					if(service->getArrivalSchedule(false, i) <= endTime)
					{
						element.endRank = i;
						break;
					}
				}
				elements.push_back(element);
			}
		}
}	}
