
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
			// Synchronization
			shared_ptr<thread> theThread(
				new thread(
					&InterSYNTHESEModule::InterSYNTHESE
			)	);
			ServerModule::AddThread(theThread, "Inter-SYNTHESE client synchronization");
		}



		template<> void ModuleClassTemplate<InterSYNTHESEModule>::End()
		{
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
						StaticFunctionRequest<InterSYNTHESESlaveUpdateService> r;
						r.getFunction()->setSlaveId(_slaveId);
						BasicClient c(
							_masterHost,
							_masterPort
						);
						stringstream result;
						c.get(
							result,
							r.getURL()
						);

						string contentStr = result.str();
						trim(contentStr);
						if(!contentStr.empty())
						{
							typedef std::map<
								util::RegistryKeyType,
								std::pair<
									std::string,
									std::string
							>	> Content;
							Content content;
				
							vector<string> rows;
							typedef split_iterator<string::iterator> string_split_iterator;
							for(string_split_iterator its=make_split_iterator(const_cast<string&>(contentStr), first_finder(InterSYNTHESESlaveUpdateService::SYNCS_SEPARATOR, is_iequal()));
								its != string_split_iterator();
								++its
							){
								string param(copy_range<std::string>(*its));
								if(param.empty())
								{
									continue;
								}

								size_t i(0);
								for(;i<param.size(); ++i)
								{
									if(param[i] == ':')
									{
										break;
									}
								}
								size_t j(i+1);
								for(;j<param.size(); ++j)
								{
									if(param[j] == ':')
									{
										break;
									}
								}

								if(i == param.size() || i == 0)
								{
									continue;
								}

								content.insert(
									make_pair(
										lexical_cast<RegistryKeyType>(param.substr(0, i-1)),
										make_pair(param.substr(i+1, j-i-1), param.substr(j+1))
								)	);
							}

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
							stringstream result2;
							c2.get(
								result2,
								ackRequest.getURL()
							);
							if(result2.str() == InterSYNTHESEUpdateAckService::VALUE_OK)
							{
								// Reading the content
								BOOST_FOREACH(const Content::value_type& item, content)
								{
									try
									{
										auto_ptr<InterSYNTHESESyncTypeFactory> interSYNTHESE(
											Factory<InterSYNTHESESyncTypeFactory>::create(item.second.first)
										);
										interSYNTHESE->sync(item.second.second);
									}
									catch(...)
									{
										// Log
									}
								}
							}
						}
					}
					catch(...)
					{
						// Log
					}
				}

				ServerModule::SetCurrentThreadWaiting();

				this_thread::sleep(_syncWaitingTime);
			}

		}



		void InterSYNTHESEModule::Enqueue( const std::string& interSYNTHESEType, const std::string& parameter )
		{
			if(parameter.find(InterSYNTHESEQueueTableSync::TABLE.NAME) != string::npos)
			{
				return;
			}
			BOOST_FOREACH(
				InterSYNTHESESlave::Registry::value_type& slave,
				Env::GetOfficialEnv().getEditableRegistry<InterSYNTHESESlave>()
			){
				slave.second->enqueue(interSYNTHESEType, parameter);
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

