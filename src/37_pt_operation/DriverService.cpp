
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
#include "VehicleService.hpp"

namespace synthese
{
	using namespace pt_operation;
	
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
