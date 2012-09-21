
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
	}


	namespace server
	{
		template<> const string ModuleClassTemplate<DataExchangeModule>::NAME = "Echange de données";

		template<> void ModuleClassTemplate<DataExchangeModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<DataExchangeModule>::Init()
		{
			// VDV Server poller
			shared_ptr<thread> pollerThread(
				new thread(
					&DataExchangeModule::ClientsPoller
			)	);
			ServerModule::AddThread(pollerThread, "VDV clients poller");

			// VDV Clients connector
			shared_ptr<thread> serversThread(
				new thread(
					&DataExchangeModule::ServersConnector
			)	);
			ServerModule::AddThread(serversThread, "VDV servers connector");
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
					value.get<Name>(),
					&value
			)	);
		}



		void DataExchangeModule::RemoveVDVClient( const std::string& key )
		{
			_vdvClients.erase(key);
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



		void DataExchangeModule::ClientsPoller()
		{
			while(true)
			{
				ServerModule::SetCurrentThreadRunningAction();

				BOOST_FOREACH(
					VDVClient::Registry::value_type& client,
					Env::GetOfficialEnv().getEditableRegistry<VDVClient>()
				){
					VDVClient::UpdatedItems items(
						client.second->checkUpdate()
					);
					if(items.empty())
					{
						continue;
					}
					client.second->sendUpdate(items);
				}

				ServerModule::SetCurrentThreadWaiting();

				this_thread::sleep(posix_time::seconds(10));
			}
		}



		void DataExchangeModule::ServersConnector()
		{
			ptime now(second_clock::local_time());

			BOOST_FOREACH(
				VDVServer::Registry::value_type& server,
				Env::GetOfficialEnv().getEditableRegistry<VDVServer>()
			){
				if(	server.second->getOnline() ||
					server.second->getSubscriptions().empty()
				){
					continue;
				}
				server.second->connect();

				this_thread::sleep(minutes(1));
			}
		}
}	}
