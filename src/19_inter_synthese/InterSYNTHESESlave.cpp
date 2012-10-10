
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
#include "InterSYNTHESEConfigItem.hpp"
#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlaveUpdateService.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
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
		const string InterSYNTHESESlave::TAG_QUEUE_ITEM = "queue_item";



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
			)	),
			_lastSentRange(make_pair(_queue.end(), _queue.end()))
		{
		}



		void InterSYNTHESESlave::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
		}



		void InterSYNTHESESlave::unlink()
		{
		}



		void InterSYNTHESESlave::enqueue(
			const std::string& interSYNTHESEType,
			const std::string& parameter
		) const	{
			ptime now(microsec_clock::local_time());

			if(	isObsolete()
			){
				return;
			}

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



		void InterSYNTHESESlave::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{
			
			BOOST_FOREACH(const Queue::value_type& it, _queue)
			{
				shared_ptr<ParametersMap> itemPM(new ParametersMap);

				it.second->toParametersMap(*itemPM);

				map.insert(prefix + TAG_QUEUE_ITEM, itemPM);
			}
		}



		InterSYNTHESESlave::QueueRange InterSYNTHESESlave::getQueueRange() const
		{
			if(!get<InterSYNTHESEConfig>())
			{
				throw Exception("Inconsistent slave");
			}

			mutex::scoped_lock lock(_queueMutex);
			if(isObsolete())
			{
				_queue.clear();
				BOOST_FOREACH(
					const InterSYNTHESEConfig::Items::value_type& it,
					get<InterSYNTHESEConfig>()->getItems()
				){
					it->getInterSYNTHESE().initQueue(
						*this,
						it->get<SyncParameters>()
					);
				}
			}

			if(_queue.empty())
			{
				return make_pair(_queue.end(), _queue.end());
			}

			Queue::iterator itEnd(_queue.end());
			--itEnd;
			return make_pair(
				_queue.begin(),
				itEnd
			);
		}



		bool InterSYNTHESESlave::isObsolete() const
		{
			ptime now(second_clock::local_time());
			return
				!get<LastActivityReport>().is_not_a_date_time() &&
				get<InterSYNTHESEConfig>() &&
				now - get<LastActivityReport>() < get<InterSYNTHESEConfig>()->get<LinkBreakMinutes>()
			;
		}



		void InterSYNTHESESlave::clearLastSentRange() const
		{
			DBTransaction transaction;

			for(Queue::iterator it(_lastSentRange.first);
				it != _queue.end();
				++it
			){
				InterSYNTHESEQueueTableSync::RemoveRow(it->first, transaction);
			}

			_lastSentRange = make_pair(_queue.end(), _queue.end());

			transaction.run();
		}
}	}

