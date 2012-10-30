
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

#include "IneoNCEConnection.hpp"
#include "ServerModule.h"
#include "VDVClient.hpp"
#include "VDVClientSubscription.hpp"
#include "VDVServer.hpp"

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
		DataExchangeModule::VDVClients DataExchangeModule::_vdvClients;
		DataExchangeModule::VDVServers DataExchangeModule::_vdvServers;
		bool DataExchangeModule::_vdvClientActive = true;
		bool DataExchangeModule::_vdvServerActive = true;
		ptime DataExchangeModule::_vdvStartingTime = second_clock::local_time();
		const string DataExchangeModule::MODULE_PARAM_VDV_SERVER_ACTIVE = "vdv_server_active";
		const string DataExchangeModule::MODULE_PARAM_VDV_CLIENT_ACTIVE = "vdv_client_active";
	}


	namespace server
	{
		template<> const string ModuleClassTemplate<DataExchangeModule>::NAME = "Echange de données";

		template<> void ModuleClassTemplate<DataExchangeModule>::PreInit()
		{
			RegisterParameter(DataExchangeModule::MODULE_PARAM_VDV_SERVER_ACTIVE, "1", &DataExchangeModule::ParameterCallback);
			RegisterParameter(DataExchangeModule::MODULE_PARAM_VDV_CLIENT_ACTIVE, "1", &DataExchangeModule::ParameterCallback);
			RegisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_HOST, "", &IneoNCEConnection::ParameterCallback);
			RegisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_PORT, "", &IneoNCEConnection::ParameterCallback);
		}

		template<> void ModuleClassTemplate<DataExchangeModule>::Init()
		{
			// In the init section in order to read this parameter after the data load (DBModule::Init)
			RegisterParameter(IneoNCEConnection::MODULE_PARAM_INEO_NCE_DATASOURCE_ID, "", &IneoNCEConnection::ParameterCallback);

			// VDV Server poller
			shared_ptr<thread> pollerThread(
				new thread(
					&DataExchangeModule::ClientsPoller
			)	);
			ServerModule::AddThread(pollerThread, "VDV clients poller");

			// VDV Servers connector
			shared_ptr<thread> serversThread(
				new thread(
					&DataExchangeModule::ServersConnector
			)	);
			ServerModule::AddThread(serversThread, "VDV servers connector");

			// Ineo NCE connector
			shared_ptr<thread> nceThread(
				new thread(
					&IneoNCEConnection::InitThread
			)	);
			ServerModule::AddThread(nceThread, "Ineo NCE connector");
		}

		template<> void ModuleClassTemplate<DataExchangeModule>::End()
		{
		}
	}

	namespace data_exchange
	{
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



		void DataExchangeModule::ClientsPoller()
		{
			ptime now(second_clock::local_time());
			while(true)
			{
				if(_vdvServerActive)
				{
					ServerModule::SetCurrentThreadRunningAction();

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

				ServerModule::SetCurrentThreadWaiting();
				this_thread::sleep(posix_time::seconds(30));
			}
		}



		void DataExchangeModule::ServersConnector()
		{
			while(true)
			{
				// Checks all server if the client is active
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

				// Wait 30 s
				ServerModule::SetCurrentThreadWaiting();
				this_thread::sleep(seconds(30));
			}
		}



		void DataExchangeModule::ParameterCallback(
			const std::string& name,
			const std::string& value
		){
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
		}
}	}
