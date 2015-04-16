
/** DriverAllocationTemplateTemplate class implementation.
	@file DriverAllocationTemplateTemplate.cpp

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

#include "DriverAllocationTemplate.hpp"

using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace impex;
	using namespace pt_operation;
	using namespace util;

	CLASS_DEFINITION(DriverAllocationTemplate, "t088_driver_allocation_templates", 88)
	FIELD_DEFINITION_OF_OBJECT(DriverAllocationTemplate, "driver_allocation_template_id", "driver_allocation_template_ids")
	FIELD_DEFINITION_OF_TYPE(Amount, "amount", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(MaxBoniAmount, "max_boni_amount", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(MaxBoniTime, "max_boni_time", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(WorkRange, "work_range", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(WorkDuration, "work_duration", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(WithTicketSales, "with_ticket_sales", SQL_BOOLEAN)

	namespace pt_operation
	{
		DriverAllocationTemplate::DriverAllocationTemplate(
			RegistryKeyType id
		):	Registrable(id),
			Object<DriverAllocationTemplate, DriverAllocationTemplateSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(DriverService::Vector),
					FIELD_VALUE_CONSTRUCTOR(Date, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(Amount, 0.0),
					FIELD_VALUE_CONSTRUCTOR(MaxBoniAmount, 0.0),
					FIELD_VALUE_CONSTRUCTOR(MaxBoniTime, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_VALUE_CONSTRUCTOR(WorkRange, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(WorkDuration, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(WithTicketSales, false)
			)	)
		{}



		boost::posix_time::time_duration DriverAllocationTemplate::getWorkRange() const
		{
			if(get<WorkRange>().is_not_a_date_time())
			{
				return getServiceEnd() - getServiceBeginning();
			}
			return get<WorkRange>();
		}



		boost::posix_time::time_duration DriverAllocationTemplate::getWorkDuration() const
		{
			if(get<WorkDuration>().is_not_a_date_time())
			{
				time_duration r(minutes(0));
				BOOST_FOREACH(const DriverService::Vector::Type::value_type& item, get<DriverService::Vector>())
				{
					r += item->getWorkDuration();
				}
				return r;
			}
			return get<WorkDuration>();
		}



		boost::posix_time::time_duration DriverAllocationTemplate::getServiceBeginning() const
		{
			if(!get<DriverService::Vector>().empty())
			{
				return (*get<DriverService::Vector>().begin())->getServiceBeginning();
			}
			return time_duration(not_a_date_time);
		}



		boost::posix_time::time_duration DriverAllocationTemplate::getServiceEnd() const
		{
			if(!get<DriverService::Vector>().empty())
			{
				return (*get<DriverService::Vector>().rbegin())->getServiceEnd();
			}
			return time_duration(not_a_date_time);
		}
}	}
