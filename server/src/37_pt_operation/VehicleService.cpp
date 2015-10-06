
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

#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "NumericField.hpp"
#include "OperationUnitTableSync.hpp"
#include "Profile.h"
#include "ScheduledService.h"
#include "SchemaMacros.hpp"
#include "StringField.hpp"
#include "User.h"
#include "VehicleServiceTableSync.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace pt_operation;
	using namespace util;

	CLASS_DEFINITION(VehicleService, "t077_vehicle_services", 77)
	FIELD_DEFINITION_OF_OBJECT(VehicleService, "vehicle_service_id", "vehicle_service_ids")

	FIELD_DEFINITION_OF_TYPE(OpeningDuration, "opening_duration", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ClosingDuration, "closing_duration", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DataSourceLinksWithoutUnderscore, "datasource_links", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Services, "services", SQL_TEXT)

	namespace pt_operation
	{
		const string VehicleService::TAG_SERVICE = "service";
		const string VehicleService::ATTR_CLASS = "class";
		const string VehicleService::VALUE_COMMERCIAL = "commercial";
		const string VehicleService::VALUE_DEAD_RUN = "deadRun";

		VehicleService::VehicleService(
			RegistryKeyType id
		):	Registrable(id),
			Object<VehicleService, VehicleServiceSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Services),
					FIELD_DEFAULT_CONSTRUCTOR(DataSourceLinksWithoutUnderscore),
					FIELD_DEFAULT_CONSTRUCTOR(Dates),
					FIELD_DEFAULT_CONSTRUCTOR(OperationUnit),
					FIELD_VALUE_CONSTRUCTOR(OpeningDuration, minutes(0)),
					FIELD_VALUE_CONSTRUCTOR(ClosingDuration, minutes(0))
			)	)
		{}



		SchedulesBasedService* VehicleService::getService(std::size_t rank) const
		{
			if(rank >= get<Services>().size())
			{
				return NULL;
			}
			return get<Services>().operator[](rank);
		}



		void VehicleService::insert(pt::SchedulesBasedService& value)
		{
			Services::Type::iterator it(get<Services>().begin());
			for(; it != get<Services>().end(); ++it)
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
			if(it == get<Services>().end())
			{
				get<Services>().push_back(&value);
			}
			else
			{
				get<Services>().insert(it, &value);
			}
		}

		void VehicleService::remove(pt::SchedulesBasedService& value)
		{
			Services::Type::iterator it(get<Services>().begin());
			for(; it != get<Services>().end(); ++it)
			{
				if(*it == &value)
				{
					break;
				}
			}
			if(it == get<Services>().end())
			{
				return;
			}
			else
			{
				get<Services>().erase(it);
			}
		}



		void VehicleService::clearServices()
		{
			get<Services>().clear();
		}



		void VehicleService::addDriverServiceChunk(
			const DriverService::Chunk& value
		){
			_driverServiceChunks.insert(&value);
		}



		void VehicleService::removeDriverServiceChunk(
			const DriverService::Chunk& value
		){
			_driverServiceChunks.erase(&value);
		}



		void VehicleService::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			// Services detail
			BOOST_FOREACH(const Services::Type::value_type& service, get<Services>())
			{
				boost::shared_ptr<ParametersMap> serviceMap(new ParametersMap);

				serviceMap->insert(ATTR_CLASS, dynamic_cast<ScheduledService*>(service) ? VALUE_COMMERCIAL : VALUE_DEAD_RUN);
				service->toParametersMap(*serviceMap, false);

				map.insert(prefix + TAG_SERVICE, serviceMap);
			}
		}



		void VehicleService::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
		}



		void VehicleService::unlink()
		{
		}



		VehicleService::~VehicleService()
		{
			unlink();
		}



		//////////////////////////////////////////////////////////////////////////
		/// Return true if no date is defined
		bool VehicleService::isActive( const boost::gregorian::date& date ) const
		{
			// Non defined calendar : check of the services
			if(empty())
			{
				// Empty vehicle services is active every day
				if(get<Services>().empty())
				{
					return true;
				}

				// Check if at least one service is active
				BOOST_FOREACH(const Services::Type::value_type& service, get<Services>())
				{
					if(service->isActive(date))
					{
						return true;
					}
				}

				return false;
			}

			// Check the calendar
			return Calendar::isActive(date);
		}



		bool VehicleService::DriverServiceChunkCompare::operator()(
			const DriverService::Chunk* ds1,
			const DriverService::Chunk* ds2
		) const	{
			if(ds1 == ds2)
			{
				return false;
			}
			if(ds1->elements.empty())
			{
				return false;
			}
			if(ds2->elements.empty())
			{
				return true;
			}

			const SchedulesBasedService& s1(*ds1->elements.begin()->service);
			const SchedulesBasedService& s2(*ds2->elements.begin()->service);

			const time_duration& t1(s1.getDepartureSchedule(false, 0));
			const time_duration& t2(s2.getDepartureSchedule(false, 0));

			if(t1 != t2)
			{
				return t1 < t2;
			}

			recursive_mutex::scoped_lock lock1(s1.getSchedulesMutex());
			recursive_mutex::scoped_lock lock2(s2.getSchedulesMutex());

			const time_duration& at1(s1.getLastArrivalSchedule(false));
			const time_duration& at2(s2.getLastArrivalSchedule(false));

			if(at1 != at2)
			{
				return at1 < at2;
			}

			return ds1 < ds2;
		}


		bool VehicleService::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool VehicleService::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool VehicleService::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
