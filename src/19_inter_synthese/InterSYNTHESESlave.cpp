
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

#include "ActionException.h"
#include "BasicClient.h"
#include "InterSYNTHESEConfigItem.hpp"
#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlaveTableSync.hpp"
#include "InterSYNTHESESlaveUpdateService.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
#include "ServerModule.h"

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
	
	namespace inter_synthese
	{
		const string InterSYNTHESESlave::TAG_QUEUE_ITEM = "queue_item";
		const string InterSYNTHESESlave::TAG_QUEUE_SIZE = "queue_size";



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
			_lastSentRange(make_pair(_queue.end(), _queue.end())),
			_previousConfig(NULL)
		{
		}

		InterSYNTHESESlave::~InterSYNTHESESlave()
		{
			if(_previousConfig)
			{
				_previousConfig->eraseSlave(this);
			}
		}


		void InterSYNTHESESlave::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<InterSYNTHESEConfig>() && _previousConfig != get_pointer(get<InterSYNTHESEConfig>()))
			{
				if(_previousConfig)
				{
					_previousConfig->eraseSlave(this);
					_previousConfig = NULL;
				}
				get<InterSYNTHESEConfig>()->insertSlave(this);
			}
		}



		void InterSYNTHESESlave::unlink()
		{
			if(get<InterSYNTHESEConfig>())
			{
				_previousConfig = get_pointer(get<InterSYNTHESEConfig>());
			}
		}



		void InterSYNTHESESlave::enqueue(
			const std::string& interSYNTHESEType,
			const std::string& parameter,
			boost::optional<db::DBTransaction&> transaction,
			bool force
		) const	{
			ptime now(microsec_clock::local_time());

			if(	!force && isObsolete()
			){
				return;
			}

			InterSYNTHESEQueue q;
			q.set<InterSYNTHESESlave>(*const_cast<InterSYNTHESESlave*>(this));
			q.set<RequestTime>(now);
			q.set<SyncType>(interSYNTHESEType);
			q.set<SyncContent>(parameter);
			InterSYNTHESEQueueTableSync::Save(&q, transaction);
		}



		void InterSYNTHESESlave::queue( InterSYNTHESEQueue& obj ) const
		{
			recursive_mutex::scoped_lock lock(_queueMutex);
			_queue.insert(make_pair(obj.get<Key>(), &obj));
		}



		void InterSYNTHESESlave::removeFromQueue( util::RegistryKeyType id ) const
		{
			recursive_mutex::scoped_lock lock(_queueMutex);
			_queue.erase(id);
		}



		void InterSYNTHESESlave::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			map.insert(prefix + TAG_QUEUE_SIZE, _queue.size());

			size_t count(30);
			BOOST_FOREACH(const Queue::value_type& it, _queue)
			{
				boost::shared_ptr<ParametersMap> itemPM(new ParametersMap);

				it.second->toParametersMap(*itemPM);

				map.insert(prefix + TAG_QUEUE_ITEM, itemPM);

				if(--count == 0)
				{
					// We don't want to publish this way all the queue content
					// as it may be huge.
					break;
				}
			}
		}


		bool InterSYNTHESESlave::fullUpdateNeeded() const
		{
			if(!get<InterSYNTHESEConfig>())
			{
				throw Exception("Invalid slave configuration");
			}

			return(isObsolete() || get<InterSYNTHESEConfig>()->get<ForceDump>());
		}

		void InterSYNTHESESlave::processFullUpdate() const
		{
			if(!get<InterSYNTHESEConfig>())
			{
				throw Exception("Invalid slave configuration");
			}

			if(isObsolete() || get<InterSYNTHESEConfig>()->get<ForceDump>())
			{
				// Clean the obsolete queue items
				DBTransaction deleteTransaction;
				{
					// Do no run transation with the lock or we will deadlock if
					// a transaction triggers a real time update
					recursive_mutex::scoped_lock lock(_queueMutex);
					BOOST_FOREACH(const Queue::value_type& it, _queue)
					{
						DBModule::GetDB()->deleteStmt(it.first, deleteTransaction);
					}
				}
				deleteTransaction.run();

				boost::unique_lock<shared_mutex> lock(ServerModule::baseWriterMutex, boost::try_to_lock);
				if(!lock.owns_lock())
				{
					throw ActionException("InterSYNTHESESlaveUpdateService: Already running");
				}

				// Load new queue items
				{
					typedef map<string, InterSYNTHESESyncTypeFactory::RandomItems> RandomItems;
					RandomItems randItems;
					BOOST_FOREACH(
						const InterSYNTHESEConfig::Items::value_type& it,
						get<InterSYNTHESEConfig>()->getItems()
					){
						randItems[it->get<SyncType>()].push_back(it);
					}
					BOOST_FOREACH(const RandomItems::value_type& it, randItems)
					{
						boost::shared_ptr<InterSYNTHESESyncTypeFactory> interSYNTHESE(
							Factory<InterSYNTHESESyncTypeFactory>::create(it.first)
						);
						InterSYNTHESESyncTypeFactory::SortedItems sortedItems(
							interSYNTHESE->sort(it.second)
						);
						BOOST_FOREACH(
							const InterSYNTHESESyncTypeFactory::SortedItems::value_type& item,
							sortedItems
						){
							interSYNTHESE->initQueue(
								*this,
								item->get<SyncPerimeter>()
							);
						}
					}
				}
			}
		}



		InterSYNTHESESlave::QueueRange InterSYNTHESESlave::getQueueRange() const
		{
			if(!get<InterSYNTHESEConfig>())
			{
				throw Exception("Invalid slave configuration");
			}

			if(_queue.empty())
			{
				return make_pair(_queue.end(), _queue.end());
			}

			Queue::iterator itEnd(_queue.end());
			--itEnd;
			if(_queue.size() > get<InterSYNTHESEConfig>()->get<MaxQueriesNumber>())
			{
				itEnd = _queue.begin();
				size_t count(get<InterSYNTHESEConfig>()->get<MaxQueriesNumber>());
				while(--count)
				{
					++itEnd;
				}
			}
			return make_pair(
				_queue.begin(),
				itEnd
			);
		}



		// In the Master/Slave communication, it is possible to have one
		// thread calling isObsolete() and one calling markAsUpToDate()
		// In this case we can have a race around our config that is
		// rewritten by the DB stack.
		void InterSYNTHESESlave::markAsUpToDate()
		{
			recursive_mutex::scoped_lock lock(_slaveChangeMutex);
			ptime now(second_clock::local_time());
			set<LastActivityReport>(now);
			InterSYNTHESESlaveTableSync::Save(this);
		}



		bool InterSYNTHESESlave::isObsolete() const
		{
			recursive_mutex::scoped_lock lock(_slaveChangeMutex);

			// In multimaster mode can not handle obsolescence after the first sync
			if(	get<InterSYNTHESEConfig>() &&
				get<InterSYNTHESEConfig>()->get<Multimaster>()
			){
				return get<LastActivityReport>().is_not_a_date_time();
			}

			ptime now(second_clock::local_time());
			return
				get<LastActivityReport>().is_not_a_date_time() ||
				!get<InterSYNTHESEConfig>() ||
				now - get<LastActivityReport>() >= get<InterSYNTHESEConfig>()->get<LinkBreakMinutes>()
			;
		}



		void InterSYNTHESESlave::clearLastSentRange() const
		{
			DBTransaction transaction;
			{
				// Do no run transation with the lock or we will deadlock if
				// a transaction triggers a real time update
				recursive_mutex::scoped_lock lock(_queueMutex);

				for(Queue::iterator it(_lastSentRange.first);
					it != _queue.end();
					++it
					){
					DBModule::GetDB()->deleteStmt(it->first, transaction);
					// Exit on last item
					if(it == _lastSentRange.second)
					{
						break;
					}
				}

				_lastSentRange = make_pair(_queue.end(), _queue.end());
			}
			transaction.run();
		}
}	}

