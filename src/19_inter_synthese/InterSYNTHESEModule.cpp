
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

#include "Import.hpp"
#include "InterSYNTHESEFileFormat.hpp"
#include "InterSYNTHESEPackage.hpp"
#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESESlaveUpdateService.hpp"
#include "ServerModule.h"
#include "URI.hpp"
#include "User.h"

#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

using namespace boost::posix_time;

using namespace boost;
using namespace std;

namespace synthese
{
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
		const RegistryKeyType InterSYNTHESEModule::FAKE_IMPORT_ID = 1;

		string InterSYNTHESEModule::_masterHost;
		string InterSYNTHESEModule::_masterPort;
		bool InterSYNTHESEModule::_slaveActive(true);
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
		}



		template<> void ModuleClassTemplate<InterSYNTHESEModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<InterSYNTHESEModule>::Start()
		{
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
			_generateFakeImport();
		}



		void InterSYNTHESEModule::_generateFakeImport()
		{
			if(!_slaveActive)
			{
				if(Env::GetOfficialEnv().getRegistry<Import>().contains(FAKE_IMPORT_ID))
				{
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
}	}

