
//////////////////////////////////////////////////////////////////////////////////////////
///	VDVClientsListService class implementation.
///	@file VDVClientsListService.cpp
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

#include "VDVClientsListService.hpp"

#include "DataExchangeModule.hpp"
#include "Request.h"
#include "RequestException.h"
#include "VDVClient.hpp"
#include "VDVClientSubscription.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVClientsListService>::FACTORY_KEY = "vdv_clients_list";
	
	namespace data_exchange
	{
		const string VDVClientsListService::TAG_CLIENT = "client";
		


		ParametersMap VDVClientsListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVClientsListService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap VDVClientsListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			
			BOOST_FOREACH(const DataExchangeModule::VDVClients::value_type& client, DataExchangeModule::GetVDVClients())
			{
				boost::shared_ptr<ParametersMap> clientPM(new ParametersMap);
				client.second->toParametersMap(*clientPM, true);
				map.insert(TAG_CLIENT, clientPM);
			}

			return map;
		}
		
		
		
		bool VDVClientsListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VDVClientsListService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
