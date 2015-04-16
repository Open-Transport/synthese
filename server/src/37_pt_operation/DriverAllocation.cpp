
/** DriverAllocation class implementation.
	@file DriverAllocation.cpp

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

#include "DriverAllocation.hpp"

#include "User.h"

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace impex;
	using namespace pt_operation;
	using namespace util;

	CLASS_DEFINITION(DriverAllocation, "t085_driver_allocations", 85)
	FIELD_DEFINITION_OF_OBJECT(DriverAllocation, "driver_allocation_id", "driver_allocation_ids")
	FIELD_DEFINITION_OF_TYPE(Driver, "driver_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(BoniAmount, "boni_amount", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(BoniTime, "boni_time", SQL_INTEGER)

	namespace pt_operation
	{
		DriverAllocation::DriverAllocation(
			RegistryKeyType id
		):	Registrable(id),
			Object<DriverAllocation, DriverAllocationSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(DriverAllocationTemplate),
					FIELD_DEFAULT_CONSTRUCTOR(Driver),
					FIELD_VALUE_CONSTRUCTOR(Date, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(BoniAmount, 0.0),
					FIELD_VALUE_CONSTRUCTOR(BoniTime, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_DEFAULT_CONSTRUCTOR(DriverActivity)
			)	)
		{}
}	}
