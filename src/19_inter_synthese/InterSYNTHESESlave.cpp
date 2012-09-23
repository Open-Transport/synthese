
/** InterSYNTHESESlave class implementation.
	@file InterSYNTHESESlave.cpp

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

#include "InterSYNTHESESlave.hpp"

#include "BasicClient.h"
#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlaveUpdateService.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace inter_synthese;
	using namespace server;
	using namespace util;

	CLASS_DEFINITION(InterSYNTHESESlave, "t093_inter_synthese_slaves", 93)
	FIELD_DEFINITION_OF_OBJECT(InterSYNTHESESlave, "slave_id", "slave_ids")

	FIELD_DEFINITION_OF_TYPE(ServerAddress, "address", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ServerPort, "port", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LastActivityReport, "last_activity_report", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(Active, "active", SQL_BOOLEAN)

	namespace inter_synthese
	{
		InterSYNTHESESlave::InterSYNTHESESlave(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<InterSYNTHESESlave, InterSYNTHESESlaveRecord>(
			Schema(
				FIELD_VALUE_CONSTRUCTOR(Key, id),
				FIELD_DEFAULT_CONSTRUCTOR(Name),
				FIELD_DEFAULT_CONSTRUCTOR(ServerAddress),
				FIELD_VALUE_CONSTRUCTOR(ServerPort, "8080"),
				FIELD_VALUE_CONSTRUCTOR(LastActivityReport, posix_time::not_a_date_time),
				FIELD_DEFAULT_CONSTRUCTOR(InterSYNTHESEConfig),
				FIELD_VALUE_CONSTRUCTOR(Active, false)
		)	)
		{
		}



		void InterSYNTHESESlave::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
		}



		void InterSYNTHESESlave::unlink()
		{
		}



		void InterSYNTHESESlave::send() const
		{
			try
			{
				stringstream content;
				bool first(true);
				BOOST_FOREACH(const Queue::value_type& item, _queue)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						content << InterSYNTHESESlaveUpdateService::SYNCS_SEPARATOR;
					}
					content
						<< item.second->get<SyncType>() << ":"
						<< item.second->get<SyncContent>()
					;
				}

				string url("?SERVICE="+ InterSYNTHESESlaveUpdateService::FACTORY_KEY);

				BasicClient c(
					get<ServerAddress>(),
					get<ServerPort>(),
					0,
					true
				);
				stringstream out;
				c.post(out, url, content.str());
				string outStr(out.str());

				if(outStr == "OK")
				{
					_queue.clear();
				}
			}
			catch(...)
			{

			}
		}



		void InterSYNTHESESlave::enqueue( const std::string& interSYNTHESEType, const std::string& parameter ) const
		{
			ptime now(microsec_clock::local_time());

			InterSYNTHESEQueue q;
			q.set<InterSYNTHESESlave>(*const_cast<InterSYNTHESESlave*>(this));
			q.set<RequestTime>(now);
			q.set<SyncType>(interSYNTHESEType);
			q.set<SyncContent>(parameter);
			InterSYNTHESEQueueTableSync::Save(&q);
		}



		void InterSYNTHESESlave::queue( InterSYNTHESEQueue& obj ) const
		{
			_queue.insert(make_pair(obj.get<Key>(), &obj));
		}



		void InterSYNTHESESlave::removeFromQueue( util::RegistryKeyType id ) const
		{
			_queue.erase(id);
		}
}	}

