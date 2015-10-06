
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
#include "InterSYNTHESEPacket.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
#include "Profile.h"
#include "ServerModule.h"
#include "User.h"

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
	FIELD_DEFINITION_OF_TYPE(PassiveModeImportId, "passive_mode_import_id", SQL_INTEGER)
	
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
					FIELD_VALUE_CONSTRUCTOR(Active, false),
					FIELD_VALUE_CONSTRUCTOR(PassiveModeImportId, 0)
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



		//////////////////////////////////////////////////////////////////////////
		/// Adds an item in the synchronization queue.
		/// @param persistent Store the item in the database to prevent data loss in case of reboot
		void InterSYNTHESESlave::enqueue(
			const std::string& interSYNTHESEType,
			const std::string& parameter,
			const boost::posix_time::ptime& expirationTime,
			boost::optional<db::DBTransaction&> transaction,
			bool nonPersistent,
			bool force,
			Registrable* objectToRemember
		) const	{

			ptime now(microsec_clock::local_time());

			if(	!force && isObsolete()
			){
				return;
			}

			recursive_mutex::scoped_lock lock(_queueMutex);
			boost::shared_ptr<InterSYNTHESEQueue> q(new InterSYNTHESEQueue);
			q->set<InterSYNTHESESlave>(*const_cast<InterSYNTHESESlave*>(this));
			q->set<RequestTime>(now);
			q->set<SyncType>(interSYNTHESEType);
			q->set<SyncContent>(parameter);
			q->set<ExpirationTime>(expirationTime);

			if(nonPersistent && !objectToRemember)
			{
				// In non persistent mode, the object is only handled by the environment after explicit link method call
				q->setKey(InterSYNTHESEQueueTableSync::getId());
				q->setNonPersistent();
				Env::GetOfficialEnv().add(q);
				q->link(Env::GetOfficialEnv());
			}
			else if (nonPersistent && objectToRemember)
			{
				// In non persistent mode, some queue elements are remembered in a cache to be able to update them
				if (_cacheQueue.find(objectToRemember) != _cacheQueue.end())
				{
					// Object is not present in the cache, we check that it is not sent by looking in lastSentRange
					// if not sent, we update the queue element
					// if sent, we create a new queue element and we update the cache
					RegistryKeyType queueIdToCheck = _cacheQueue.at(objectToRemember);
					if (queueIdToCheck >= _lastSentRange.first->first &&
						queueIdToCheck <= _lastSentRange.second->first)
					{
						// queue element has been sent but is still in queue because we have not received slave ACK
						// we create a new queue element and we update the cache
						q->setKey(InterSYNTHESEQueueTableSync::getId());
						q->setNonPersistent();
						Env::GetOfficialEnv().add(q);
						q->link(Env::GetOfficialEnv());
						_cacheQueue.erase(objectToRemember);
						_cacheQueue.insert(make_pair(objectToRemember,q->getKey()));
					}
					else
					{
						// update the cache
						q->setNonPersistent();
						q->setKey(_cacheQueue.at(objectToRemember));
						Env::GetOfficialEnv().getEditableRegistry<InterSYNTHESEQueue>().remove(_cacheQueue.at(objectToRemember));
						Env::GetOfficialEnv().add(q);
						_queue.erase(_cacheQueue.at(objectToRemember));
						q->link(Env::GetOfficialEnv());
					}
				}
				else
				{
					// Object is not present in the cache, so we create it
					q->setKey(InterSYNTHESEQueueTableSync::getId());
					q->setNonPersistent();
					Env::GetOfficialEnv().add(q);
					q->link(Env::GetOfficialEnv());
					_cacheQueue.insert(make_pair(objectToRemember,q->getKey()));
				}
			}
			else
			{
				// In persistent mode, the object is stored then the standard environment load process will run
				InterSYNTHESEQueueTableSync::Save(q.get(), transaction);
			}
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
			// We look in the cache to remove corresponding item (if exists)
			CacheQueue::iterator it = _cacheQueue.begin();
			while(it != _cacheQueue.end())
			{
				if (it->second == id)
				{
					break;
				}
				it++;
			}
			if (it != _cacheQueue.end())
			{
				_cacheQueue.erase(it);
			}
		}



		void InterSYNTHESESlave::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			// Lock the queue
			recursive_mutex::scoped_lock lock(_queueMutex);

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
					// Do no run transaction with the lock or we will deadlock if
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
		
		void InterSYNTHESESlave::processFullRTUpdate() const
		{
			if(!get<InterSYNTHESEConfig>())
			{
				throw Exception("Invalid slave configuration");
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
						if (item->get<NonPersistent>())
						{
							interSYNTHESE->initQueue(
								*this,
								item->get<SyncPerimeter>()
							);
						}
					}
				}
			}
		}



		/// @pre The queue must be locked by the caller of the function until the returned
		/// QueueRange is destroyed. Use getQueueMutex to lock the queue.
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
			ptime now(second_clock::local_time());
			set<LastActivityReport>(now);
			InterSYNTHESESlaveTableSync::Save(this);
		}



		bool InterSYNTHESESlave::isObsolete() const
		{
			optional<InterSYNTHESEConfig&> config(get<InterSYNTHESEConfig>());

			// In multimaster mode can not handle obsolescence after the first sync
			if(	config &&
				config->get<Multimaster>()
			){
				return get<LastActivityReport>().is_not_a_date_time();
			}

			ptime now(second_clock::local_time());
			// If LinkBreakMinutes is set to 0, it means full dump should only be done manually
			// (resetting LastActivityReport)
			return
				get<LastActivityReport>().is_not_a_date_time() ||
				!config ||
				(config->get<LinkBreakMinutes>() != minutes(0) &&
					now - get<LastActivityReport>() >= config->get<LinkBreakMinutes>())
			;
		}



		void InterSYNTHESESlave::clearLastSentRange() const
		{
			DBTransaction transaction;
			{
				// Do no run transaction with the lock or we will deadlock if
				// a transaction triggers a real time update
				recursive_mutex::scoped_lock lock(_queueMutex);

				Queue::iterator it(_lastSentRange.first);
				while (it != _queue.end())
				{
					bool willBreak(false);
					if(it == _lastSentRange.second)
					{
						willBreak = true;
					}
					// Save the next iterator because we may suppress current it from _queue (by ->unlink)
					Queue::iterator it2(it);
					RegistryKeyType idToRemove = it->first;
					it2++;
					try
					{
						InterSYNTHESEQueue* q(
							it->second
						);
						if(q->getNonPersistent())
						{
							// The item was only stored in the environment : do a manually unload
							q->unlink();

							// The shared pointer will be destroyed just quiting the local scope since it was only linked in the environment
							Env::GetOfficialEnv().getEditableRegistry<InterSYNTHESEQueue>().remove(idToRemove);
						}
						else
						{
							// The item was stored in the database : simply remove it, the standard unload process will run
							DBModule::GetDB()->deleteStmt(idToRemove, transaction);
						}
					}
					catch(ObjectNotFoundException<InterSYNTHESEQueue>&)
					{
					}

					// Exit on last item
					if(willBreak)
					{
						break;
					}
					
					it = it2;
				}

				_lastSentRange = make_pair(_queue.end(), _queue.end());
			}
			transaction.run();
		}



		void InterSYNTHESESlave::clearUselessQueueEntries(DBTransaction& transaction) const
		{
			ptime now(second_clock::local_time());

			// Do no run transaction with the lock or we will deadlock if
			// a transaction triggers a real time update
			recursive_mutex::scoped_lock lock(_queueMutex);

			Queue::iterator it = _queue.begin();
			while(it != _queue.end())
			{
				// Save the next iterator because we may suppress the current iterator from _queue (by ->unlink)
				Queue::iterator it2 = it;
				RegistryKeyType idToRemove = it->first;
				it2++;

				if(	isObsolete() || (!it->second->get<ExpirationTime>().is_not_a_date_time() && it->second->get<ExpirationTime>() < now))
				{
					InterSYNTHESEQueue* q(
						it->second
					);
					if(q->getNonPersistent())
					{
						// The item was only stored in the environment : do a manually unload
						q->unlink();

						// The shared pointer will be destroyed just quiting the local scope since it was only linked in the environment
						Env::GetOfficialEnv().getEditableRegistry<InterSYNTHESEQueue>().remove(idToRemove);
					}
					else
					{
						// The item was stored in the database : simply remove it, the standard unload process will run
						DBModule::GetDB()->deleteStmt(idToRemove, transaction);
					}
				}

				it = it2;
			}
		}



		void InterSYNTHESESlave::sendToSlave(
			std::ostream& stream,
			const QueueRange& range,
			bool askIdRange
		) const	{
			recursive_mutex::scoped_lock lock(_queueMutex);

			if (askIdRange)
			{
				bool first(true);
				for(InterSYNTHESESlave::Queue::iterator it(range.first); it != _queue.end(); ++it)
				{
					if (first)
					{
						stream << it->second->get<Key>() << InterSYNTHESEPacket::FIELDS_SEPARATOR;
						first = false;
					}
					// Exit on last item
					if(it == range.second)
					{
						stream << it->second->get<Key>() << InterSYNTHESEPacket::FIELDS_SEPARATOR <<
							InterSYNTHESEPacket::SYNCS_SEPARATOR;
						break;
					}
				}
			}

			for(InterSYNTHESESlave::Queue::iterator it(range.first); it != _queue.end(); ++it)
			{
				stream <<
					it->second->get<Key>() << InterSYNTHESEPacket::FIELDS_SEPARATOR <<
					it->second->get<SyncType>() << InterSYNTHESEPacket::FIELDS_SEPARATOR <<
					it->second->get<SyncContent>().size() << InterSYNTHESEPacket::FIELDS_SEPARATOR <<
					it->second->get<SyncContent>() <<
					InterSYNTHESEPacket::SYNCS_SEPARATOR;
				;

				// Exit on last item
				if(it == range.second)
				{
					break;
				}
			}

			setLastSentRange(range);
		}


		bool InterSYNTHESESlave::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool InterSYNTHESESlave::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool InterSYNTHESESlave::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}

