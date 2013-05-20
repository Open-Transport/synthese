
//////////////////////////////////////////////////////////////////////////////////////////
///	VDVServersListService class implementation.
///	@file VDVServersListService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "VDVServersListService.hpp"

#include "DataExchangeModule.hpp"
#include "Request.h"
#include "RequestException.h"
#include "VDVServer.hpp"
#include "VDVServerSubscription.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVServersListService>::FACTORY_KEY = "vdv_servers_list";
	
	namespace data_exchange
	{
		const string VDVServersListService::PARAMETER_SUBSCRIPTION_ID = "subscription_id";
		const string VDVServersListService::TAG_SERVER = "server";
		


		ParametersMap VDVServersListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVServersListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Server filter
			RegistryKeyType serverFilterId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			if(serverFilterId > 0)
			{
				try
				{
					_serverFilter = Env::GetOfficialEnv().getRegistry<VDVServer>().get(serverFilterId);
				}
				catch(ObjectNotFoundException<VDVServer>&)
				{
					throw RequestException("No such VDV server");
				}
			}

			// Subscription filter
			RegistryKeyType subscriptionId(map.getDefault<RegistryKeyType>(PARAMETER_SUBSCRIPTION_ID));
			if(subscriptionId > 0)
			{
				try
				{
					boost::shared_ptr<const VDVServerSubscription> subscription(
						Env::GetOfficialEnv().getRegistry<VDVServerSubscription>().get(subscriptionId)
					);
					if(!subscription->get<VDVServer>())
					{
						throw RequestException("Invalid VDV server subscription");
					}
					_serverFilter = Env::GetOfficialEnv().getSPtr(&*subscription->get<VDVServer>());
				}
				catch(ObjectNotFoundException<VDVServerSubscription>&)
				{
					throw RequestException("No such VDV server subscription");
				}
			}
		}



		ParametersMap VDVServersListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			if(_serverFilter.get())
			{
				boost::shared_ptr<ParametersMap> serverPM(new ParametersMap);
				_serverFilter->toParametersMap(*serverPM, true);
				map.insert(TAG_SERVER, serverPM);
			}
			else
			{
				BOOST_FOREACH(
					const VDVServer::Registry::value_type& server,
					Env::GetOfficialEnv().getRegistry<VDVServer>()
				){
					boost::shared_ptr<ParametersMap> serverPM(new ParametersMap);
					server.second->toParametersMap(*serverPM, true);
					map.insert(TAG_SERVER, serverPM);
				}
			}

			return map;
		}
		
		
		
		bool VDVServersListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VDVServersListService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
