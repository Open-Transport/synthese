
/** InterSYNTHESEQueue class implementation.
	@file InterSYNTHESEQueue.cpp

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

#include "InterSYNTHESEQueue.hpp"

using namespace boost;

namespace synthese
{
	using namespace inter_synthese;
	using namespace util;

	CLASS_DEFINITION(InterSYNTHESEQueue, "t096_inter_synthese_queue", 96)
	FIELD_DEFINITION_OF_OBJECT(InterSYNTHESEQueue, "queue_id", "queue_ids")

	FIELD_DEFINITION_OF_TYPE(RequestTime, "request_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(ExpirationTime, "expiration_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(SyncType, "sync_type", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SyncContent, "content", SQL_BLOB)

	namespace inter_synthese
	{
		InterSYNTHESEQueue::InterSYNTHESEQueue(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<InterSYNTHESEQueue, InterSYNTHESEQueueRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(InterSYNTHESESlave),
					FIELD_VALUE_CONSTRUCTOR(RequestTime, posix_time::not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(ExpirationTime, posix_time::not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(SyncType),
					FIELD_DEFAULT_CONSTRUCTOR(SyncContent)
			)	),
			_nonPersistent(false)
		{}



		void InterSYNTHESEQueue::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<InterSYNTHESESlave>())
			{
				get<InterSYNTHESESlave>()->queue(*this);
			}
		}



		void InterSYNTHESEQueue::unlink()
		{
			if(get<InterSYNTHESESlave>())
			{
				get<InterSYNTHESESlave>()->removeFromQueue(get<Key>());
			}
		}
}	}
