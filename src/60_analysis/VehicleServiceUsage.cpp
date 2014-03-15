
/** VehicleServiceUsage class implementation.
	@file VehicleServiceUsage.cpp

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

#include "VehicleServiceUsage.hpp"

#include "DeadRun.hpp"

using namespace boost::posix_time;

namespace synthese
{
	using namespace analysis;
	using namespace pt;
	using namespace pt_operation;
	using namespace util;

	CLASS_DEFINITION(VehicleServiceUsage, "t117_vehicle_service_usages", 117)
	FIELD_DEFINITION_OF_OBJECT(VehicleServiceUsage, "vehicle_service_usage_id", "vehicle_service_usage_ids")

	FIELD_DEFINITION_OF_TYPE(PlannedCommercialHours, "planned_commercial_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(PlannedDeadRunHours, "planned_dead_run_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(PlannedWaitingHours, "planned_waiting_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(PlannedOpeningHours, "planned_opening_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(PlannedClosingHours, "planned_closing_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(PlannedCommercialKM, "planned_commercial_km", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(PlannedDeadRunKM, "planned_dead_run_km", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(HasRealData, "has_real_data", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(RealCommercialHours, "real_commercial_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(RealDeadRunHours, "real_dead_run_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(RealWaitingHours, "real_waiting_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(RealOpeningHours, "real_opening_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(RealClosingHours, "real_closing_hours", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(RealCommercialKM, "real_commercial_km", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(RealDeadRunKM, "real_dead_run_km", SQL_DOUBLE)

	namespace analysis
	{
		VehicleServiceUsage::VehicleServiceUsage(
			RegistryKeyType id,
			pt_operation::VehicleService::Type service,
			Date::Type day
		):	Registrable(id),
			Object<VehicleServiceUsage, VehicleServiceUsageSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(VehicleService, service),
					FIELD_VALUE_CONSTRUCTOR(Date, day),
					FIELD_VALUE_CONSTRUCTOR(PlannedCommercialHours, 0),
					FIELD_VALUE_CONSTRUCTOR(PlannedDeadRunHours, 0),
					FIELD_VALUE_CONSTRUCTOR(PlannedWaitingHours, 0),
					FIELD_VALUE_CONSTRUCTOR(PlannedOpeningHours, 0),
					FIELD_VALUE_CONSTRUCTOR(PlannedClosingHours, 0),
					FIELD_VALUE_CONSTRUCTOR(PlannedCommercialKM, 0),
					FIELD_VALUE_CONSTRUCTOR(PlannedDeadRunKM, 0),
					FIELD_VALUE_CONSTRUCTOR(HasRealData, false),
					FIELD_VALUE_CONSTRUCTOR(RealCommercialHours, 0),
					FIELD_VALUE_CONSTRUCTOR(RealDeadRunHours, 0),
					FIELD_VALUE_CONSTRUCTOR(RealWaitingHours, 0),
					FIELD_VALUE_CONSTRUCTOR(RealOpeningHours, 0),
					FIELD_VALUE_CONSTRUCTOR(RealClosingHours, 0),
					FIELD_VALUE_CONSTRUCTOR(RealCommercialKM, 0),
					FIELD_VALUE_CONSTRUCTOR(RealDeadRunKM, 0)
			)	)
		{}



		//////////////////////////////////////////////////////////////////////////
		/// Generates the hours and km from the existing data :
		///  - planned data is read from the planned services
		///  - real data is read from the real scheduled services data (todo implement it)
		/// @warning the linked service must come from an environment containing the
		/// whole vehicle service description
		void VehicleServiceUsage::generate(
		){
			// Check if both vehicle service and day are defined
			if(	!get<VehicleService>() ||
				get<Date>().is_not_a_date()
			){
				return;
			}

			time_duration lastArrival;
			time_duration commercialDuration(minutes(0));
			time_duration deadRunDuration(minutes(0));
			time_duration waitingDuration(minutes(0));
			double commercialKM(0);
			double deadRunKM(0);

			// Loop on services contained in the vehicle service
			BOOST_FOREACH(const Services::Type::value_type& service, get<VehicleService>()->get<Services>())
			{
				// Jump over services running at the specified date
				if(!service->isActive(get<Date>()))
				{
					continue;
				}

				// Get the data from the service
				time_duration startTime(*service->getDataDepartureSchedules().begin());
				time_duration endTime(*service->getDataArrivalSchedules().rbegin());

				// Waiting duration between last arrival and departure
				if(!lastArrival.is_not_a_date_time())
				{
					waitingDuration += startTime - lastArrival;
				}

				// Trip duration and distance
				if(dynamic_cast<DeadRun*>(service))
				{
					deadRunDuration += (endTime - startTime);
					deadRunKM += double(dynamic_cast<DeadRun*>(service)->length()) / 1000;
				}
				else
				{
					commercialDuration += (endTime - startTime);
					JourneyPattern& jp(dynamic_cast<JourneyPattern&>(*service->getPath()));
					commercialKM += double(jp.getPlannedLength() ? jp.getPlannedLength() : jp.length()) / 1000;
				}

				// Save the last arrival time
				lastArrival = endTime;
			}

			// Save data from last loop
			set<PlannedCommercialHours>(double(commercialDuration.total_seconds()) / 3600);
			set<PlannedDeadRunHours>(double(deadRunDuration.total_seconds()) / 3600);
			set<PlannedWaitingHours>(double(waitingDuration.total_seconds()) / 3600);
			set<PlannedCommercialKM>(commercialKM);
			set<PlannedDeadRunKM>(deadRunKM);

			// Add opening and closing hours
			set<PlannedOpeningHours>(double(get<VehicleService>()->get<OpeningDuration>().total_seconds()) / 3600);
			set<PlannedClosingHours>(double(get<VehicleService>()->get<ClosingDuration>().total_seconds()) / 3600);
		}



		VehicleServiceUsage& VehicleServiceUsage::operator+=(
			const VehicleServiceUsage& rhs
		){
			set<PlannedCommercialHours>(get<PlannedCommercialHours>() + rhs.get<PlannedCommercialHours>());
			set<PlannedWaitingHours>(get<PlannedWaitingHours>() + rhs.get<PlannedWaitingHours>());
			set<PlannedOpeningHours>(get<PlannedOpeningHours>() + rhs.get<PlannedOpeningHours>());
			set<PlannedClosingHours>(get<PlannedClosingHours>() + rhs.get<PlannedClosingHours>());
			set<PlannedCommercialKM>(get<PlannedCommercialKM>() + rhs.get<PlannedCommercialKM>());
			set<PlannedDeadRunKM>(get<PlannedDeadRunKM>() + rhs.get<PlannedDeadRunKM>());

			if(rhs.get<HasRealData>())
			{
				// If real data, init existing real data at the planned values
				if(!get<HasRealData>())
				{
					set<RealCommercialHours>(get<PlannedCommercialHours>());
					set<RealWaitingHours>(get<PlannedWaitingHours>());
					set<RealOpeningHours>(get<PlannedOpeningHours>());
					set<RealClosingHours>(get<PlannedClosingHours>());
					set<RealCommercialKM>(get<PlannedCommercialKM>());
					set<RealDeadRunKM>(get<PlannedDeadRunKM>());
					set<HasRealData>(true);
				}

				set<RealCommercialHours>(get<RealCommercialHours>() + rhs.get<RealCommercialHours>());
				set<RealWaitingHours>(get<RealWaitingHours>() + rhs.get<RealWaitingHours>());
				set<RealOpeningHours>(get<RealOpeningHours>() + rhs.get<RealOpeningHours>());
				set<RealClosingHours>(get<RealClosingHours>() + rhs.get<RealClosingHours>());
				set<RealCommercialKM>(get<RealCommercialKM>() + rhs.get<RealCommercialKM>());
				set<RealDeadRunKM>(get<RealDeadRunKM>() + rhs.get<RealDeadRunKM>());
			}
			
			return *this;
		}
}	}
