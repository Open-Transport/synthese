
/** VehicleServiceUsage class header.
	@file VehicleServiceUsage.hpp

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

#ifndef SYNTHESE_analysis_VehicleServiceUsage_hpp__
#define SYNTHESE_analysis_VehicleServiceUsage_hpp__

#include "SchemaMacros.hpp"
#include "DateField.hpp"
#include "NumericField.hpp"
#include "Object.hpp"
#include "VehicleService.hpp"

namespace synthese
{
	FIELD_DOUBLE(PlannedCommercialHours)
	FIELD_DOUBLE(PlannedDeadRunHours)
	FIELD_DOUBLE(PlannedWaitingHours)
	FIELD_DOUBLE(PlannedOpeningHours)
	FIELD_DOUBLE(PlannedClosingHours)
	FIELD_DOUBLE(PlannedCommercialKM)
	FIELD_DOUBLE(PlannedDeadRunKM)
	FIELD_BOOL(HasRealData)
	FIELD_DOUBLE(RealCommercialHours)
	FIELD_DOUBLE(RealDeadRunHours)
	FIELD_DOUBLE(RealWaitingHours)
	FIELD_DOUBLE(RealOpeningHours)
	FIELD_DOUBLE(RealClosingHours)
	FIELD_DOUBLE(RealCommercialKM)
	FIELD_DOUBLE(RealDeadRunKM)

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(pt_operation::VehicleService),
		FIELD(Date),
		FIELD(PlannedCommercialHours),
		FIELD(PlannedDeadRunHours),
		FIELD(PlannedWaitingHours),
		FIELD(PlannedOpeningHours),
		FIELD(PlannedClosingHours),
		FIELD(PlannedCommercialKM),
		FIELD(PlannedDeadRunKM),
		FIELD(HasRealData),
		FIELD(RealCommercialHours),
		FIELD(RealDeadRunHours),
		FIELD(RealWaitingHours),
		FIELD(RealOpeningHours),
		FIELD(RealClosingHours),
		FIELD(RealCommercialKM),
		FIELD(RealDeadRunKM)
	> VehicleServiceUsageSchema;

	namespace analysis
	{
		/** VehicleServiceUsage class.
			@ingroup m60
		*/
		class VehicleServiceUsage:
			public Object<VehicleServiceUsage, VehicleServiceUsageSchema>
		{
		public:
			VehicleServiceUsage(
				util::RegistryKeyType id = 0,
				pt_operation::VehicleService::Type service = pt_operation::VehicleService::Type(),
				Date::Type day = Date::Type(boost::gregorian::not_a_date_time)
			);

			void generate();

			VehicleServiceUsage& operator+=(const VehicleServiceUsage& rhs);
		};
	}
}

#endif // SYNTHESE_analysis_VehicleServiceUsage_hpp__

