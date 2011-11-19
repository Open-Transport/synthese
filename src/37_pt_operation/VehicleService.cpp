
/** VehicleService service class implementation.
	@file VehicleService.cpp

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

#include "VehicleService.hpp"
#include "SchedulesBasedService.h"
#include "DriverService.hpp"

using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<>
		const std::string Registry<pt_operation::VehicleService>::KEY("VehicleService");
	}

	namespace pt_operation
	{
		VehicleService::VehicleService(RegistryKeyType id):
			Registrable(id)
		{}



		SchedulesBasedService* VehicleService::getService(std::size_t rank) const
		{
			if(rank >= _services.size())
			{
				return NULL;
			}
			return _services[rank];
		}



		void VehicleService::insert(pt::SchedulesBasedService& value)
		{
			Services::iterator it(_services.begin());
			for(; it != _services.end(); ++it)
			{
				if(*it == &value)
				{
					return;
				}
				if((*it)->getDepartureSchedule(false, 0) > value.getDepartureSchedule(false,0))
				{
					break;
				}
			}
			if(it == _services.end())
			{
				_services.push_back(&value);
			}
			else
			{
				_services.insert(it, &value);
			}
		}



		void VehicleService::clear()
		{
			_services.clear();
		}



		void VehicleService::addDriverService( const DriverService& value )
		{
			_driverServices.insert(&value);
		}



		void VehicleService::removeDriverService( const DriverService& value )
		{
			_driverServices.erase(&value);
		}



		bool VehicleService::DriverServiceCompare::operator()( const DriverService* ds1, const DriverService* ds2 ) const
		{
			if(ds1 == ds2)
			{
				return false;
			}
			if(ds1 == NULL || ds1->getChunks().empty() || ds1->getChunks().begin()->elements.empty())
			{
				return false;
			}
			if(ds2 == NULL || ds2->getChunks().empty() || ds2->getChunks().begin()->elements.empty())
			{
				return true;
			}
			const SchedulesBasedService& s1(*ds1->getChunks().begin()->elements.begin()->service);
			const SchedulesBasedService& s2(*ds2->getChunks().begin()->elements.begin()->service);

			const time_duration& t1(s1.getDepartureSchedule(false, 0));
			const time_duration& t2(s2.getDepartureSchedule(false, 0));

			if(t1 != t2)
			{
				return t1 < t2;
			}

			const time_duration& at1(s1.getLastArrivalSchedule(false));
			const time_duration& at2(s2.getLastArrivalSchedule(false));

			return at1 < at2;
		}
}	}
