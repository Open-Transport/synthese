
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

#include "Action.h"
#include "BasicClient.h"
#include "Import.hpp"
#include "InterSYNTHESEFileFormat.hpp"
#include "InterSYNTHESEPackage.hpp"
#include "InterSYNTHESEUpdatePushService.hpp"
#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESESlaveUpdateService.hpp"
#include "ServerModule.h"
#include "SpecificPostInstall.hpp"
#include "StaticFunctionRequest.h"
#include "URI.hpp"
#include "User.h"

#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

using namespace boost::posix_time;

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace inter_synthese;
	using namespace server;
	using namespace util;
	
	template<>
	const string FactorableTemplate<ModuleClass, InterSYNTHESEModule>::FACTORY_KEY = "19_inter_synthese";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_MASTER_HOST = "inter_synthese_master_host";
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_MASTER_PORT = "inter_synthese_master_port";
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_WAITING_TIME = "inter_synthese_waiting_time";
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_SLAVE_ACTIVE = "inter_synthese_slave_active";
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_SLAVE_ID = "inter_synthese_slave_id";
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL = "post_install";
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_PASSIVE_IMPORT_ID = "post_install_passive_import_id";
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_SLAVE_TO_MASTER_IP = "post_install_slave_to_master_ip";
		const string InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_TABLES = "post_install_tables";
		const RegistryKeyType InterSYNTHESEModule::FAKE_IMPORT_ID = 1;

		string InterSYNTHESEModule::_masterHost;
		string InterSYNTHESEModule::_masterPort;
		bool InterSYNTHESEModule::_slaveActive(true);
		bool InterSYNTHESEModule::_postInstall(false);
		RegistryKeyType InterSYNTHESEModule::_postInstallPassiveImportId(0);
		string InterSYNTHESEModule::_postInstallSlaveToMasterIp;
		string InterSYNTHESEModule::_postInstallTables;
		time_duration InterSYNTHESEModule::_syncWaitingTime(seconds(5));
		RegistryKeyType InterSYNTHESEModule::_slaveId(0);
		InterSYNTHESEModule::PackagesBySmartURL InterSYNTHESEModule::_packagesBySmartURL;
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<InterSYNTHESEModule>::NAME("Inter-SYNTHESE");

		template<> void ModuleClassTemplate<InterSYNTHESEModule>::PreInit()
		{
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_MASTER_HOST, string(), &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_MASTER_PORT, "8080", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_SLAVE_ACTIVE, "0", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_SLAVE_ID, "0", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_WAITING_TIME, "5", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL, "0", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_PASSIVE_IMPORT_ID, "0", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_SLAVE_TO_MASTER_IP, "", &InterSYNTHESEModule::ParameterCallback);
			RegisterParameter(InterSYNTHESEModule::MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_TABLES, "", &InterSYNTHESEModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<InterSYNTHESEModule>::Init()
		{
			InterSYNTHESEModule::GenerateFakeImport();
			InterSYNTHESEModule::PostInstall();
		}

		template<> void ModuleClassTemplate<InterSYNTHESEModule>::Start()
		{
			ServerModule::AddThread(&InterSYNTHESESlaveUpdateService::RunBackgroundUpdater, "IS-FullUpdater");

			// Expired queue entries cleaner
			ServerModule::AddThread(&InterSYNTHESEModule::QueueCleaner, "IS-QueueCleaner");
			
			// Passive slaves updater (for multimaster config)
			ServerModule::AddThread(&InterSYNTHESEModule::PassiveSlavesUpdater, "Passive slaves updater");
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
		void InterSYNTHESEModule::Enqueue(
			const InterSYNTHESEContent& content,
			boost::optional<db::DBTransaction&> transaction,
			Registrable* objectToRemember
		){
			BOOST_FOREACH(
				InterSYNTHESEConfig::Registry::value_type& config,
				Env::GetOfficialEnv().getEditableRegistry<InterSYNTHESEConfig>()
			){
				config.second->enqueueIfInPerimeter(
					content,
					transaction,
					objectToRemember
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
			else if(name == MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL)
			{
				if(value == "1")
				{
					_postInstall = true;
				}
				else
				{
					_postInstall = false;
				}
			}
			else if(name == MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_PASSIVE_IMPORT_ID)
			{
				try
				{
					_postInstallPassiveImportId = lexical_cast<RegistryKeyType>(value);
				}
				catch(bad_lexical_cast&)
				{
					// Log
				}
			}
			else if(name == MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_SLAVE_TO_MASTER_IP)
			{
				try
				{
					_postInstallSlaveToMasterIp = lexical_cast<string>(value);
				}
				catch(bad_lexical_cast&)
				{
					// Log
				}
			}
			else if(name == MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_TABLES)
			{
				try
				{
					_postInstallTables = lexical_cast<string>(value);
				}
				catch(bad_lexical_cast&)
				{
					// Log
				}
			}
		}



		void InterSYNTHESEModule::GenerateFakeImport()
		{
			if(!_slaveActive)
			{
				if(Env::GetOfficialEnv().getRegistry<Import>().contains(FAKE_IMPORT_ID))
				{
					boost::shared_ptr<Import> import(
						Env::GetOfficialEnv().getRegistry<Import>().getEditable(FAKE_IMPORT_ID)
					);
					import->unlink();
					Env::GetOfficialEnv().getEditableRegistry<Import>().remove(FAKE_IMPORT_ID);
				}
			}
			else
			{
				boost::shared_ptr<Import> import;
				bool created(false);
				if(Env::GetOfficialEnv().getRegistry<Import>().contains(FAKE_IMPORT_ID))
				{
					import = Env::GetOfficialEnv().getRegistry<Import>().getEditable(FAKE_IMPORT_ID);
				}
				else
				{
					import.reset(new Import(FAKE_IMPORT_ID));
					import->set<Name>("Auto generated Inter-SYNTHESE import");
					import->set<FileFormatKey>(InterSYNTHESEFileFormat::FACTORY_KEY);
					import->set<Active>(true);
					created = true;
				}

				// Import parameters
				ParametersMap pm;
				pm.insert(ConnectionImporter<InterSYNTHESEFileFormat>::PARAMETER_ADDRESS, _masterHost);
				pm.insert(ConnectionImporter<InterSYNTHESEFileFormat>::PARAMETER_PORT, _masterPort);
				pm.insert(InterSYNTHESEFileFormat::Importer_::PARAMETER_SLAVE_ID, _slaveId);
				import->set<synthese::Parameters>(pm);
				import->set<AutoImportDelay>(_syncWaitingTime);

				import->link(Env::GetOfficialEnv(), true);
				if(created)
				{
					Env::GetOfficialEnv().getEditableRegistry<Import>().add(import);
				}
			}
		}



		void InterSYNTHESEModule::RemovePackage( const std::string& smartURL )
		{
			_packagesBySmartURL.erase(smartURL);
		}



		void InterSYNTHESEModule::AddPackage( InterSYNTHESEPackage& package )
		{
			_packagesBySmartURL[package.get<Code>()] = &package;
		}



		InterSYNTHESEPackage* InterSYNTHESEModule::GetPackageBySmartURL( const std::string& smartURL )
		{
			PackagesBySmartURL::iterator it(_packagesBySmartURL.find(smartURL));
			if(it == _packagesBySmartURL.end())
			{
				return NULL;
			}
			return it->second;
		}



		void InterSYNTHESEModule::QueueCleaner()
		{
			while(true)
			{
				// Protect this section with the interSyntheseVersusRTMutex to avoid deadlock
				/*boost::unique_lock<shared_mutex> lock(ServerModule::interSyntheseVersusRTMutex, boost::try_to_lock);
				int tries = 10;
				while (!lock.owns_lock() && tries > 0)
				{
					lock.try_lock();
					this_thread::sleep((seconds(1)));
					Log::GetInstance().debug("InterSYNTHESESlave::clearLastSentRange locked by interSyntheseVersusRTMutex (try " + lexical_cast<string>(tries) + "/10)");
					tries--;
				}
				if(!lock.owns_lock())
				{
					Log::GetInstance().error("InterSYNTHESESlave::clearLastSentRange locked by interSyntheseVersusRTMutex (max tries reached)");
					return;
				}*/
				
				ServerModule::SetCurrentThreadRunningAction();

				DBTransaction transaction;

				// Loop on slaves
				{
					// Copy of the list of slaves to avoid to keep the lock on the registry during the clear action
					const InterSYNTHESESlave::Registry& registry(Env::GetOfficialEnv().getRegistry<InterSYNTHESESlave>());
					InterSYNTHESESlave::Registry::Vector slaves(registry.getVector());
					BOOST_FOREACH(const InterSYNTHESESlave::Registry::Vector::value_type& slave, slaves)
					{
						slave->clearUselessQueueEntries(transaction);
					}
				}

				if(!transaction.getQueries().empty())
				{
					// Log in debug mode
					Log::GetInstance().debug("Cleaned "+ lexical_cast<string>(transaction.getQueries().size()) + " useless Inter-SYNTHESE queue items.");

					transaction.run();
				}
	
				ServerModule::SetCurrentThreadWaiting();
				boost::this_thread::sleep(boost::posix_time::minutes(1));
			}
		}



		bool InterSYNTHESEModule::_passiveSlaveUpdaterSelector( const InterSYNTHESESlave& object )
		{
			// Handle only slaves in passive mode
			if(!object.get<PassiveModeImportId>())
			{
				return false;
			}

			// Jump over empty queues
			if(object.getQueue().empty())
			{
				return false;
			}

			return true;
		}



		void InterSYNTHESEModule::PassiveSlavesUpdater()
		{
			while(true)
			{
				ServerModule::SetCurrentThreadRunningAction();

				// Loop on slaves
				{
					const InterSYNTHESESlave::Registry& registry(Env::GetOfficialEnv().getRegistry<InterSYNTHESESlave>());
					InterSYNTHESESlave::Registry::Vector slaves(
						registry.getVector(&_passiveSlaveUpdaterSelector)
					);

					// Loop on the selected slaves
					BOOST_FOREACH(const boost::shared_ptr<InterSYNTHESESlave>& slave, slaves)
					{
						try
						{
							recursive_mutex::scoped_lock lock(slave->getQueueMutex());
							InterSYNTHESESlave::QueueRange range(slave->getQueueRange());
							stringstream data;
							slave->sendToSlave(
								data,
								range
							);

							StaticFunctionRequest<InterSYNTHESEUpdatePushService> r;

							r.getFunction()->setImportId(slave->get<PassiveModeImportId>());
							r.getFunction()->setSlaveId(slave->getKey());
							r.getFunction()->setContent(data.str());

							BasicClient c(
								slave->get<ServerAddress>(),
								slave->get<ServerPort>()
							);
							string responseStr(
								c.post(
									r.getClientURL(),
									r.getURI(),
									"application/x-www-form-urlencoded"
							)	);

							vector<string> rangeStr;
							split(rangeStr, responseStr, is_any_of(InterSYNTHESEUpdatePushService::RANGE_SEPARATOR));
							if(	rangeStr.size() == 2 &&
								lexical_cast<RegistryKeyType>(rangeStr[0]) == slave->getLastSentRange().first->first &&
								lexical_cast<RegistryKeyType>(rangeStr[1]) == slave->getLastSentRange().second->first
							){
								slave->clearLastSentRange();
							}
						}
						catch(...)
						{

						}
					}
				}

				ServerModule::SetCurrentThreadWaiting();
				boost::this_thread::sleep(boost::posix_time::seconds(10));
			}
		}

		void InterSYNTHESEModule::PostInstall()
		{
			if(_postInstall)
			{
				boost::shared_ptr<Action>
					action(boost::shared_ptr<Action>(util::Factory<Action>::create("SpecificPostInstall")));
				Request request;
				ParametersMap pm;
				pm.insert(SpecificPostInstall::PARAMETER_POST_INSTALL_PASSIVE_IMPORT_ID, _postInstallPassiveImportId);
				pm.insert(SpecificPostInstall::PARAMETER_POST_INSTALL_SLAVE_ID, _slaveId);
				pm.insert(SpecificPostInstall::PARAMETER_POST_INSTALL_SLAVE_TO_MASTER_IP, _postInstallSlaveToMasterIp);
				pm.insert(SpecificPostInstall::PARAMETER_POST_INSTALL_TABLES, _postInstallTables);
				action->_setFromParametersMap(pm);
				action->run(request);
			}
		}
}	}

