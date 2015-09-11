
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

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace pt;
	using namespace util;

	CLASS_DEFINITION(VDVServerSubscription, "t098_vdv_server_subscriptions", 98)
	FIELD_DEFINITION_OF_OBJECT(VDVServerSubscription, "vdv_server_subscription_id", "vdv_server_subscription_ids")

	FIELD_DEFINITION_OF_TYPE(SubscriptionDuration, "subscription_duration", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TimeSpan, "time_span", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(VDVSubscriptionType, "vdv_subscription_type", SQL_INTEGER)
	
	namespace data_exchange
	{
		const string VDVServerSubscription::ATTR_ONLINE = "online";
		const string VDVServerSubscription::ATTR_EXPIRATION = "expiration";



		VDVServerSubscription::VDVServerSubscription(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<VDVServerSubscription, VDVServerSubscriptionRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(VDVServer),
					FIELD_DEFAULT_CONSTRUCTOR(StopArea),
					FIELD_VALUE_CONSTRUCTOR(SubscriptionDuration, hours(24)),
					FIELD_VALUE_CONSTRUCTOR(TimeSpan, minutes(60)),
					FIELD_VALUE_CONSTRUCTOR(VDVSubscriptionType, Dfi)
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



		void VDVServerSubscription::addAdditionalParameters(
			ParametersMap& map,
			string prefix
		) const	{
			map.insert(ATTR_ONLINE, _online);
			if(!_expiration.is_not_a_date_time())
			{
				map.insert(ATTR_EXPIRATION, to_iso_extended_string(_expiration));
			}
		}
}	}

