
/** VDVServerSubscription class implementation.
	@file VDVServerSubscription.cpp

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

#include "VDVServerSubscription.hpp"

using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace util;

	CLASS_DEFINITION(VDVServerSubscription, "t098_vdv_server_subscriptions", 98)
	FIELD_DEFINITION_OF_OBJECT(VDVServerSubscription, "vdv_server_subscription_id", "vdv_server_subscription_ids")

	FIELD_DEFINITION_OF_TYPE(StopAreaPointer, "stop_area_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(SubscriptionDuration, "subscription_duration", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TimeSpan, "time_span", SQL_INTEGER)
	
	namespace data_exchange
	{
		VDVServerSubscription::VDVServerSubscription(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<VDVServerSubscription, VDVServerSubscriptionRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(VDVServer),
					FIELD_DEFAULT_CONSTRUCTOR(StopAreaPointer),
					FIELD_VALUE_CONSTRUCTOR(SubscriptionDuration, hours(24)),
					FIELD_VALUE_CONSTRUCTOR(TimeSpan, minutes(60))
			)	),
			_expiration(not_a_date_time),
			_online(false)
		{
		}



		void VDVServerSubscription::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<VDVServer>())
			{
				get<VDVServer>()->addSubscription(this);
			}

		}



		void VDVServerSubscription::unlink()
		{
			if(get<VDVServer>())
			{
				get<VDVServer>()->removeSubscription(this);
			}
		}
}	}

