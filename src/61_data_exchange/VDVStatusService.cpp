
//////////////////////////////////////////////////////////////////////////////////////////
///	VDVStatusService class implementation.
///	@file VDVStatusService.cpp
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

#include "VDVStatusService.hpp"

#include "DataExchangeModule.hpp"
#include "Request.h"
#include "RequestException.h"
#include "ServerConstants.h"
#include "ServerModule.h"
#include "VDVClient.hpp"
#include "VDVClientSubscription.hpp"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace util::XmlToolkit;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVStatusService>::FACTORY_KEY = "VDVStatus";
	
	namespace data_exchange
	{
		const string VDVStatusService::DATA_RESULT = "result";



		VDVStatusService::VDVStatusService():
			FactorableTemplate<server::Function,VDVStatusService>(),
			_ok(true),
			_client(NULL)
		{
		}
		


		ParametersMap VDVStatusService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVStatusService::_setFromParametersMap(const ParametersMap& map)
		{
			string request(map.getDefault<string>(PARAMETER_POST_DATA));

			XMLResults results;
			XMLNode allNode = XMLNode::parseString(request.c_str(), "StatusAnfrage", &results);
			if (results.error != eXMLErrorNone)
			{
				_ok = false;
				return;
			}

			try
			{
				string sender(allNode.getAttribute("Sender"));
				_client = &DataExchangeModule::GetVDVClient(sender);

				// Trace
				_client->trace("StatusAnfrage", request);

				if(!_client->get<Active>())
				{
					_ok = false;
					return;
				}
			}
			catch (Exception&)
			{
				_ok = false;
				return;
			}

			_ok = DataExchangeModule::GetVDVServerActive();
		}



		ParametersMap VDVStatusService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Map creation
			ParametersMap map;

			// Local variables
			ptime now(second_clock::local_time());
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;
			ptime serverStartingTime(DataExchangeModule::GetVDVStartingTime());
			serverStartingTime -= diff_from_utc;

			// Check if new data
			bool newData = false;
			BOOST_FOREACH(const VDVClient::Subscriptions::value_type& it, _client->getSubscriptions())
			{
				// Run an update
				it.second->checkUpdate();
				if(it.second->getDeletions().empty() && it.second->getAddings().empty())
				{
					continue;
				}
				// There is new data
				newData = true;
				break;
			}
			
			// XML
			stringstream result;
			result << 
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<StatusAntwort>" <<
				"<Status Zst=\"";
			ToXsdDateTime(result, now);
			result <<
				"\" Ergebnis=\"" <<
				(_ok ? "ok" : "notok") <<
				"\" />" <<
				"<DatenBereit>" <<
				(newData ? "true" : "false")
				<< "</DatenBereit>" <<
				"<StartDienstZst>";
			ToXsdDateTime(result, serverStartingTime);
			result <<
				"</StartDienstZst>" <<
				"</StatusAntwort>"
			;
			map.insert(DATA_RESULT, result.str());

			// Output the result (TODO cancel it if the service is called through the CMS)
			stream << result.str();

			// Trace
			_client->trace("StatusAntwort", result.str());

			// Map return
			return map;
		}
		
		
		
		bool VDVStatusService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VDVStatusService::getOutputMimeType() const
		{
			return "text/xml";
		}
}	}
