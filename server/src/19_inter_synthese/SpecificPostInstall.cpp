
//////////////////////////////////////////////////////////////////////////////////////////
///	SpecificPostInstall class implementation.
///	@file SpecificPostInstall.cpp
///	@author RCSmobility
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "DBTransaction.hpp"
#include "SpecificPostInstall.hpp"
#include "InterSYNTHESEConfigTableSync.hpp"
#include "InterSYNTHESEConfigItem.hpp"
#include "InterSYNTHESEConfigItemTableSync.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESESlaveTableSync.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

#define MASTER_NAME "__PCCAR_TO_SAE__"

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Action, inter_synthese::SpecificPostInstall>::FACTORY_KEY = "SpecificPostInstall";

	namespace inter_synthese
	{
	
		const string SpecificPostInstall::PARAMETER_POST_INSTALL_PASSIVE_IMPORT_ID = "post_install_passive_import_id";
		const string SpecificPostInstall::PARAMETER_POST_INSTALL_SLAVE_ID = "post_install_slave_id";
		const string SpecificPostInstall::PARAMETER_POST_INSTALL_SLAVE_TO_MASTER_IP = "post_install_slave_to_master_ip";
		const string SpecificPostInstall::PARAMETER_POST_INSTALL_TABLES = "post_install_tables";

		ParametersMap SpecificPostInstall::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void SpecificPostInstall::_setFromParametersMap(const ParametersMap& map)
		{
			_passiveImportId = map.get<RegistryKeyType>(PARAMETER_POST_INSTALL_PASSIVE_IMPORT_ID);
			_slaveId = map.get<RegistryKeyType>(PARAMETER_POST_INSTALL_SLAVE_ID);
			_slaveToMasterIp = map.get<std::string>(PARAMETER_POST_INSTALL_SLAVE_TO_MASTER_IP);
			_tables = map.get<std::string>(PARAMETER_POST_INSTALL_TABLES);
		}


		const boost::shared_ptr<InterSYNTHESEConfig> SpecificPostInstall::getMyConfig() {
			BOOST_FOREACH(const InterSYNTHESEConfig::Registry::value_type& item,
			Env::GetOfficialEnv().getRegistry<InterSYNTHESEConfig>())
			{
				const boost::shared_ptr<InterSYNTHESEConfig> config(item.second);
				Log::GetInstance().info("InterSYNTHESEConfig=" + config->get<Name>());
				if(config->get<Name>() == MASTER_NAME)
				{
					Log::GetInstance().info("InterSYNTHESEPackage PCCAR_TO_SAE Already created");
					return config;
				}
			}
			return boost::shared_ptr<InterSYNTHESEConfig>();
		}

		void SpecificPostInstall::addTable(InterSYNTHESEConfig &config,
			const string tableCode)
		{
			InterSYNTHESEConfigItem vehicleCalls;
			vehicleCalls.set<InterSYNTHESEConfig>(config);
			vehicleCalls.set<SyncType>("db");
			vehicleCalls.set<SyncPerimeter>(tableCode);
			InterSYNTHESEConfigItemTableSync::Save(&vehicleCalls);
		}

		void SpecificPostInstall::run(
			Request& request
		) {

			if(!getMyConfig())
			{
				InterSYNTHESEConfig newConfig;
				newConfig.set<Name>(MASTER_NAME);
				newConfig.set<Multimaster>(true);
				InterSYNTHESEConfigTableSync::Save(&newConfig);
			}

			shared_ptr<InterSYNTHESEConfig> myConfig(getMyConfig());
			InterSYNTHESESlave slave;
			slave.set<Name>("__SAE__");
			slave.set<ServerAddress>(_slaveToMasterIp);
			slave.set<ServerPort>("80");
			slave.set<InterSYNTHESEConfig>(*myConfig);
			slave.set<Active>(true);
			slave.set<PassiveModeImportId>(_passiveImportId);
			slave.setKey(_slaveId);
			ptime now(second_clock::local_time());
			slave.set<LastActivityReport>(now);
			InterSYNTHESESlaveTableSync::Save(&slave);


			// Remove previous entries
			InterSYNTHESEConfigItemTableSync::SearchResult allConfigs(InterSYNTHESEConfigItemTableSync::Search(*_env));

			DBTransaction transaction;
			DB& db(*DBModule::GetDB());
			BOOST_FOREACH(InterSYNTHESEConfigItemTableSync::SearchResult::value_type configItem, allConfigs)
			{

				if(configItem->get<InterSYNTHESEConfig>()->getKey() == myConfig->getKey())
				{
					db.deleteStmt(configItem->getKey(), transaction);
				}
			}
			transaction.run();

			// Add new entries
			vector<string> tablesId;
			split(tablesId, _tables, is_any_of(", "));
			for(std::vector<std::string>::iterator it = tablesId.begin(); it != tablesId.end(); ++it)
			{
				addTable(*myConfig, *it);
				Log::GetInstance().info("InterSYNTHESEConfig post install adding table " + *it);
			}
			Log::GetInstance().info("InterSYNTHESEConfig post install created");
		}



		bool SpecificPostInstall::isAuthorized(
			const Session* session
		) const {
			return true;
		}

}	}

