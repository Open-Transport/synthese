
/** DataExchangeModule class implementation.
    @file DataExchangeModule.cpp

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

#include "DataExchangeModule.hpp"

#include "ServerModule.h"

#ifdef WITH_61_DATA_EXCHANGE_INEO_NCE
#include "IneoNCEConnection.hpp"
#endif

#ifdef WITH_61_DATA_EXCHANGE_MG_SCREEN
#include "MGScreenConnection.hpp"
#endif

#ifdef WITH_61_DATA_EXCHANGE_STOP_BUTTON_POLLER
#include "StopButtonFilePoller.hpp"
#endif

#ifdef WITH_61_DATA_EXCHANGE_VDV
#include "VDVClient.hpp"
#include "VDVClientSubscription.hpp"
#include "VDVServer.hpp"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace lexical_matcher;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,DataExchangeModule>::FACTORY_KEY = "36_data_exchange";
	}

	namespace data_exchange
	{
#ifdef WITH_61_DATA_EXCHANGE_VDV
		DataExchangeModule::VDVClients DataExchangeModule::_vdvClients;
		DataExchangeModule::VDVServers DataExchangeModule::_vdvServers;
		bool DataExchangeModule::_vdvClientActive = true;
		bool DataExchangeModule::_vdvServerActive = true;
		ptime DataExchangeModule::_vdvStartingTime;
		const string DataExchangeModule::MODULE_PARAM_VDV_SERVER_ACTIVE = "vdv_server_active";
		const string DataExchangeModule::MODULE_PARAM_VDV_CLIENT_ACTIVE = "vdv_client_active";
#endif
	}


	namespace server
	{
		template<> const string ModuleClassTemplate<DataExchangeModule>::NAME = "Echange de données";

		template<> void ModuleClassTemplate<DataExchangeModule>::PreInit()
		{
#ifdef WITH_61_DATA_EXCHANGE_VDV
			RegisterParameter(DataExchangeModule::MODULE_PARAM_VDV_SERVER_ACTIVE, "1", &DataExchangeModule::ParameterCallback);
			RegisterParameter(DataExchangeModule::MODULE_PARAM_VDV_CLIENT_ACTIVE, "1", &DataExchangeModule::ParameterCallback);
#endif

#ifdef WITH_61_DATA_EXCHANGE_INEO_NCE
			RegisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_HOST, "", &IneoNCEConnection::ParameterCallback);
			RegisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_PORT, "", &IneoNCEConnection::ParameterCallback);
#endif

#ifdef WITH_61_DATA_EXCHANGE_MG_SCREEN
			RegisterParameter(MGScreenConnection::MODULE_PARAM_MG_SCREEN_HOST, "", &MGScreenConnection::ParameterCallback);
			RegisterParameter(MGScreenConnection::MODULE_PARAM_MG_SCREEN_PORT, "", &MGScreenConnection::ParameterCallback);
			RegisterParameter(MGScreenConnection::MODULE_PARAM_MG_SCREEN_SPEED, "5", &MGScreenConnection::ParameterCallback);
			RegisterParameter(MGScreenConnection::MODULE_PARAM_MG_SCREEN_VALUE, "60", &MGScreenConnection::ParameterCallback);
			RegisterParameter(MGScreenConnection::MODULE_PARAM_MG_SCREEN_MIN, "1", &MGScreenConnection::ParameterCallback);
			RegisterParameter(MGScreenConnection::MODULE_PARAM_MG_SCREEN_MAX, "255", &MGScreenConnection::ParameterCallback);
			RegisterParameter(MGScreenConnection::MODULE_PARAM_MG_ARCHIVE_MONITORING, "0", &MGScreenConnection::ParameterCallback);
#endif

#ifdef WITH_61_DATA_EXCHANGE_STOP_BUTTON_POLLER
			RegisterParameter(StopButtonFilePoller::MODULE_PARAM_STOP_MONITORING_FILE, "", &StopButtonFilePoller::ParameterCallback);
			RegisterParameter(StopButtonFilePoller::MODULE_PARAM_STOP_MONITORING_PERIOD_MS, "", &StopButtonFilePoller::ParameterCallback);
#endif
		}

		template<> void ModuleClassTemplate<DataExchangeModule>::Init()
		{
			// In the init section in order to read this parameter after the data load (DBModule::Init)
#ifdef WITH_61_DATA_EXCHANGE_INEO_NCE
			RegisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_DATASOURCE_ID, "", &IneoNCEConnection::ParameterCallback);
			RegisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_MESSAGE_RECIPIENTS, "", &IneoNCEConnection::ParameterCallback);
#endif

#ifdef WITH_61_DATA_EXCHANGE_MG_SCREEN
			RegisterParameter(MGScreenConnection::MODULE_PARAM_MG_CPU_NAME, "MG CPU", &MGScreenConnection::ParameterCallback);
#endif
		}

		template<> void ModuleClassTemplate<DataExchangeModule>::Start()
		{
			// VDV Server poller
#ifdef WITH_61_DATA_EXCHANGE_VDV
			ServerModule::AddThread(&DataExchangeModule::ClientsPoller, "VDVClientsPoller");

			// VDV Client connector
			ServerModule::AddThread(&DataExchangeModule::ServersConnector, "VDVServerConnector");
#endif

#ifdef WITH_61_DATA_EXCHANGE_INEO_NCE
			// Ineo NCE connector
			ServerModule::AddThread(&IneoNCEConnection::RunThread, "IneoNCEConnector");
#endif

#ifdef WITH_61_DATA_EXCHANGE_MG_SCREEN
			// MG Screen connector
			ServerModule::AddThread(&MGScreenConnection::RunThread, "MGScreenConnector");
#endif
			// Stop Button File Poller
#ifdef WITH_61_DATA_EXCHANGE_STOP_BUTTON_POLLER
			ServerModule::AddThread(&StopButtonFilePoller::RunThread, "StopButtonPoller");
#endif
		}

		template<> void ModuleClassTemplate<DataExchangeModule>::End()
		{
#ifdef WITH_61_DATA_EXCHANGE_VDV
			UnregisterParameter(DataExchangeModule::MODULE_PARAM_VDV_SERVER_ACTIVE);
			UnregisterParameter(DataExchangeModule::MODULE_PARAM_VDV_CLIENT_ACTIVE);
#endif

#ifdef WITH_61_DATA_EXCHANGE_INEO_NCE
			UnregisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_HOST);
			UnregisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_PORT);
#endif

#ifdef WITH_61_DATA_EXCHANGE_STOP_BUTTON_POLLER
			UnregisterParameter(StopButtonFilePoller::MODULE_PARAM_STOP_MONITORING_FILE);
			UnregisterParameter(StopButtonFilePoller::MODULE_PARAM_STOP_MONITORING_PERIOD_MS);
#endif
		}



		template<> void ModuleClassTemplate<DataExchangeModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<DataExchangeModule>::CloseThread(
		){
		}
	}

	namespace data_exchange
	{
#ifdef WITH_61_DATA_EXCHANGE_VDV
		void DataExchangeModule::AddVDVClient( VDVClient& value )
		{
			_vdvClients.insert(
				make_pair(
					value.get<ClientControlCentreCode>(),
					&value
			)	);
		}



		void DataExchangeModule::AddVDVServer( VDVServer& value )
		{
			_vdvServers.insert(
				make_pair(
					value.get<ServerControlCentreCode>(),
					&value
			)	);
		}



		void DataExchangeModule::RemoveVDVClient( const std::string& key )
		{
			_vdvClients.erase(key);
		}



		void DataExchangeModule::RemoveVDVServer( const std::string& key )
		{
			_vdvServers.erase(key);
		}



		VDVClient& DataExchangeModule::GetVDVClient( const std::string& name )
		{
			VDVClients::const_iterator it(_vdvClients.find(name));
			if(it == _vdvClients.end())
			{
				throw Exception("No such VDV client : "+ name);
			}
			return *it->second;
		}



		VDVServer& DataExchangeModule::GetVDVServer( const std::string& name )
		{
			VDVServers::const_iterator it(_vdvServers.find(name));
			if(it == _vdvServers.end())
			{
				throw Exception("No such VDV server : "+ name);
			}
			return *it->second;
		}
#endif


		void DataExchangeModule::ClientsPoller()
		{
			ptime now(second_clock::local_time());
			while(true)
			{
#ifdef WITH_61_DATA_EXCHANGE_VDV
				if(_vdvServerActive)
				{
					ServerModule::SetCurrentThreadRunningAction();
					
					// get upgradable access
					boost::upgrade_lock<boost::shared_mutex> lock(ServerModule::IneoBDSIAgainstVDVMutex);
					// get exclusive access
					boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

					BOOST_FOREACH(
						VDVClient::Registry::value_type& client,
						Env::GetOfficialEnv().getEditableRegistry<VDVClient>()
					){
						if(!client.second->checkUpdate())
						{
							continue;
						}
						if(	client.second->getLastDataReady().is_not_a_date_time() ||
							now - client.second->getLastDataReady() > minutes(2)
						){
							client.second->sendUpdateSignal();
							client.second->setLastDataReadyNow();
						}
					}
				}
#endif
				ServerModule::SetCurrentThreadWaiting();
				this_thread::sleep(posix_time::seconds(30));
			}
		}



		void DataExchangeModule::ServersConnector()
		{
			while(true)
			{
				// Checks all server if the client is active
#ifdef WITH_61_DATA_EXCHANGE_VDV
				if(_vdvClientActive)
				{
					ServerModule::SetCurrentThreadRunningAction();

					BOOST_FOREACH(
						VDVServer::Registry::value_type& server,
						Env::GetOfficialEnv().getEditableRegistry<VDVServer>()
					){
						if(	server.second->getSubscriptions().empty()
						){
							continue;
						}

						server.second->connect();
					}
				}
#endif
				// Wait 30 s
				ServerModule::SetCurrentThreadWaiting();
				this_thread::sleep(seconds(30));
			}
		}



		void DataExchangeModule::ParameterCallback(
			const std::string& name,
			const std::string& value
		){
#ifdef WITH_61_DATA_EXCHANGE_VDV
			// VDV client activation
			if(name == MODULE_PARAM_VDV_CLIENT_ACTIVE)
			{
				try
				{
					bool bvalue(value.empty() ? false : lexical_cast<bool>(value));
					if(bvalue && !_vdvClientActive)
					{
						_vdvClientActive = true;
					}
					else if(!bvalue && _vdvClientActive)
					{
						_vdvClientActive = false;
					}
				}
				catch(bad_lexical_cast&)
				{
				
				}
			}

			// VDV server activation
			if(name == MODULE_PARAM_VDV_SERVER_ACTIVE)
			{
				try
				{
					bool bvalue(value.empty() ? false : lexical_cast<bool>(value));
					if(bvalue && !_vdvServerActive)
					{
						_vdvServerActive = true;
						_vdvStartingTime = second_clock::local_time();						
					}
					else if(!bvalue && _vdvServerActive)
					{
						_vdvServerActive = false;
						BOOST_FOREACH(VDVClients::value_type& client, _vdvClients)
						{
							client.second->cleanSubscriptions();
						}
					}
				}
				catch(bad_lexical_cast&)
				{

				}
			}
#endif
		}

		void DataExchangeModule::Init()
		{
#ifdef WITH_61_DATA_EXCHANGE_VDV
			_vdvStartingTime = second_clock::local_time();
#endif
		}
}	}
