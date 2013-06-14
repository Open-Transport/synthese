
/** InterSYNTHESEModule class implementation.
	@file InterSYNTHESEModule.cpp

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

#include "InterSYNTHESEModule.hpp"

#include "BasicClient.h"
#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESESlaveUpdateService.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
#include "InterSYNTHESEUpdateAckService.hpp"
#include "ServerModule.h"
#include "StaticFunctionRequest.h"

#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

using namespace boost::posix_time;

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace inter_synthese;
	using namespace server;
	using namespace util;
	
	template<> const std::string util::FactorableTemplate<ModuleClass, InterSYNTHESEModule>::FACTORY_KEY("19_inter_synthese");
	
	namespace inter_synthese
	{
		const std::string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_MASTER_HOST = "inter_synthese_master_host";
		const std::string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_MASTER_PORT = "inter_synthese_master_port";
		const std::string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_WAITING_TIME = "inter_synthese_waiting_time";
		const std::string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_SLAVE_ACTIVE = "inter_synthese_slave_active";
		const std::string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_SLAVE_ID = "inter_synthese_slave_id";

		string InterSYNTHESEModule::_masterHost;
		string InterSYNTHESEModule::_masterPort;
		bool InterSYNTHESEModule::_slaveActive(true);
		time_duration InterSYNTHESEModule::_syncWaitingTime(seconds(5));
		RegistryKeyType InterSYNTHESEModule::_slaveId(0);
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<InterSYNTHESEModule>::NAME("Inter-SYNTHESE");

		template<> void ModuleClassTemplate<InterSYNTHESEModule>::PreInit()
		{
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_MASTER_HOST, string(), &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_MASTER_PORT, "8080", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_SLAVE_ACTIVE, "1", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_SLAVE_ID, "0", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_WAITING_TIME, "5", &InterSYNTHESEModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<InterSYNTHESEModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<InterSYNTHESEModule>::Start()
		{
			ServerModule::AddThread(&InterSYNTHESEModule::InterSYNTHESE, "Inter-SYNTHESE client synchronization");
			ServerModule::AddThread(&InterSYNTHESESlaveUpdateService::RunBackgroundUpdater, "Inter-SYNTHESE slave full updater");
		}



		template<> void ModuleClassTemplate<InterSYNTHESEModule>::End()
		{
		}



		template<> void ModuleClassTemplate<InterSYNTHESEModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<InterSYNTHESEModule>::CloseThread(
		){
		}
	}

	namespace inter_synthese
	{
		void InterSYNTHESEModule::InterSYNTHESE()
		{
			while(true)
			{
				ServerModule::SetCurrentThreadRunningAction();

				posix_time::ptime now(posix_time::second_clock::local_time());

				if(	_slaveActive &&
					!_masterHost.empty() &&
					!_masterPort.empty()
				){
					try
					{
						Log::GetInstance().debug(
							"Inter-SYNTHESE : Attempt to sync with "+ _masterHost +":"+ _masterPort + " as slave id #"+ lexical_cast<string>(_slaveId)
						);
						
						StaticFunctionRequest<InterSYNTHESESlaveUpdateService> r;
						r.getFunction()->setSlaveId(_slaveId);
						BasicClient c(
							_masterHost,
							_masterPort
						);
						string contentStr(
							c.get(r.getURL())
						);

						if(contentStr == InterSYNTHESESlaveUpdateService::NO_CONTENT_TO_SYNC)
						{
							Log::GetInstance().debug(
								"Inter-SYNTHESE : "+ _masterHost +":"+ _masterPort + " has no content to sync for slave id #"+ lexical_cast<string>(_slaveId)
							);
						}
						else
						{
							bool ok(true);
							typedef std::map<
								util::RegistryKeyType,	// id of the update
								std::pair<
									std::string,		// synchronizer
									std::string			// message
							>	> ContentMap;
							ContentMap content;
				
							size_t i(0);
							while(i < contentStr.size())
							{
								ContentMap::mapped_type item;

								// ID + Search for next :
								size_t l=i;
								for(; i < contentStr.size() && contentStr[i] != InterSYNTHESESlaveUpdateService::FIELDS_SEPARATOR[0]; ++i) ;
								if(i == contentStr.size())
								{
									ok = false;
									break;
								}
								RegistryKeyType id(lexical_cast<RegistryKeyType>(contentStr.substr(l, i-l)));
								++i;

								// Synchronizer + Search for next :
								l=i;
								for(; i < contentStr.size() && contentStr[i] != InterSYNTHESESlaveUpdateService::FIELDS_SEPARATOR[0]; ++i) ;
								if(i == contentStr.size())
								{
									ok = false;
									break;
								}
								item.first = contentStr.substr(l, i-l);
								++i;

								// Size + Search for next :
								l=i;
								for(; i < contentStr.size() && contentStr[i] != InterSYNTHESESlaveUpdateService::FIELDS_SEPARATOR[0]; ++i) ;
								if(i == contentStr.size())
								{
									ok = false;
									break;
								}
								size_t contentSize = lexical_cast<size_t>(contentStr.substr(l, i-l));
								++i;

								// Content
								if(i+contentSize > contentStr.size())
								{
									ok = false;
									break;
								}
								item.second = contentStr.substr(i, contentSize);
								i += contentSize + InterSYNTHESESlaveUpdateService::SYNCS_SEPARATOR.size();

								content.insert(
									make_pair(
										id,
										item
								)	);
							}

							if(ok)
							{
								Log::GetInstance().debug(
									"Inter-SYNTHESE : "+ _masterHost +":"+ _masterPort + " has sent "+ lexical_cast<string>(content.size()) +" elements to sync in "+ lexical_cast<string>(contentStr.size()) +" bytes for slave id #"+ lexical_cast<string>(_slaveId)
								);

								StaticFunctionRequest<InterSYNTHESEUpdateAckService> ackRequest;
								ackRequest.getFunction()->setSlaveId(_slaveId);
								if(!content.empty())
								{
									ackRequest.getFunction()->setRangeBegin(
										content.begin()->first
									);
									ackRequest.getFunction()->setRangeEnd(
										content.rbegin()->first
									);
								}
								BasicClient c2(
									_masterHost,
									_masterPort
								);
								string result2(
									c2.get(ackRequest.getURL())
								);
								if(result2 == InterSYNTHESEUpdateAckService::VALUE_OK)
								{
									// Local variables
									auto_ptr<InterSYNTHESESyncTypeFactory> interSYNTHESE;
									string lastFactoryKey;

									// Reading the content
									BOOST_FOREACH(const ContentMap::value_type& item, content)
									{
										try
										{
											const string& factoryKey(item.second.first);
											if(factoryKey != lastFactoryKey)
											{
												if(interSYNTHESE.get())
												{
													interSYNTHESE->closeSync();
												}
												interSYNTHESE.reset(
													Factory<InterSYNTHESESyncTypeFactory>::create(factoryKey)
												);
												lastFactoryKey = factoryKey;
												interSYNTHESE->initSync();
											}

											interSYNTHESE->sync(item.second.second);
										}
										catch(...)
										{
											// Log
										}
									}
									if(interSYNTHESE.get())
									{
										interSYNTHESE->closeSync();
										Log::GetInstance().debug(
											"Inter-SYNTHESE : "+ _masterHost +":"+ _masterPort + " has been synchronized with current instance as slave id #"+ lexical_cast<string>(_slaveId)
										);
									}
								}
							}
						}
					}
					catch(std::exception& e)
					{
						Log::GetInstance().warn(
							"Inter-SYNTHESE : Synchronization with "+ _masterHost +":"+ _masterPort + " as slave id #"+ lexical_cast<string>(_slaveId) +" has failed",
							e
						);
					}
				}

				ServerModule::SetCurrentThreadWaiting();

				this_thread::sleep(_syncWaitingTime);
			}

		}



		void InterSYNTHESEModule::Enqueue(
			const InterSYNTHESEContent& content,
			boost::optional<db::DBTransaction&> transaction
		){
			BOOST_FOREACH(
				InterSYNTHESEConfig::Registry::value_type& config,
				Env::GetOfficialEnv().getEditableRegistry<InterSYNTHESEConfig>()
			){
				config.second->enqueueIfInPerimeter(
					content,
					transaction
				);
			}
		}



		void InterSYNTHESEModule::ParameterCallback( const std::string& name, const std::string& value )
		{
			if(name == MODULE_PARAM_INTER_SYNTHESE_MASTER_HOST)
			{
				_masterHost = value;
			}
			else if(name == MODULE_PARAM_INTER_SYNTHESE_MASTER_PORT)
			{
				_masterPort = value;
			}
			else if(name == MODULE_PARAM_INTER_SYNTHESE_WAITING_TIME)
			{
				if(!value.empty())
				{
					try
					{
						_syncWaitingTime = seconds(
							lexical_cast<long>(value)
						);
					}
					catch(bad_lexical_cast&)
					{
						// Log
					}
				}
			}
			else if(name == MODULE_PARAM_INTER_SYNTHESE_SLAVE_ACTIVE)
			{
				if(value == "1")
				{
					_slaveActive = true;
				}
				else
				{
					_slaveActive = false;
				}
			}
			else if(name == MODULE_PARAM_INTER_SYNTHESE_SLAVE_ID)
			{
				try
				{
					_slaveId = lexical_cast<RegistryKeyType>(value);
				}
				catch(bad_lexical_cast&)
				{
					// Log
				}
			}
		}
}	}

