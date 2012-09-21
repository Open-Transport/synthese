
//////////////////////////////////////////////////////////////////////////////////////////
///	VDVSubscriptionService class implementation.
///	@file VDVSubscriptionService.cpp
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

#include "VDVSubscriptionService.hpp"

#include "DataExchangeModule.hpp"
#include "Request.h"
#include "RequestException.h"
#include "ServerConstants.h"
#include "VDVClient.hpp"
#include "VDVClientSubscription.hpp"
#include "XmlParser.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVSubscriptionService>::FACTORY_KEY = "VDVAboAnfrage";
	
	namespace data_exchange
	{
		ParametersMap VDVSubscriptionService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVSubscriptionService::_setFromParametersMap(const ParametersMap& map)
		{
			string content(map.getDefault<string>(PARAMETER_POST_DATA));

			XMLResults results;
			XMLNode allNode = XMLNode::parseString(content.c_str(), "vdv453:AboAnfrage", &results);
			if (results.error != eXMLErrorNone)
			{
				return;
			}

			size_t nbNodes(allNode.nChildNode("AboAZB"));
			if(!nbNodes)
			{
				return;
			}

			string sender(allNode.getAttribute("Sender"));
			try
			{
				VDVClient& client(
					DataExchangeModule::GetVDVClient(sender)
				);

				for(size_t i(0); i<nbNodes; ++i)
				{
					XMLNode aboAZBNode(allNode.getChildNode("AboAZB", i));

					shared_ptr<VDVClientSubscription> subscription(new VDVClientSubscription);


					if(aboAZBNode.nChildNode("AZBID"))
					{
						XMLNode azbidNode(aboAZBNode.getChildNode("AZBID"));
					}


/*					<AboAZB AboID="12" VerfallZst="2007-08-14T00:00:00Z">
						<AZBID>ZZVVHBHF06</AZBID>
						<LinienID>ZVV105</LinienID>
						<RichtungsID>ZVV105B</RichtungsID>
						<Vorschauzeit>30</Vorschauzeit>
						<Hysterese>60</Hysterese>
						</AboAZB>
*/
					client.addSubscription(subscription);
				}

			}
			catch (Exception&)
			{
				return;
			}
		}



		ParametersMap VDVSubscriptionService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			/// @todo Fill it
			return map;
		}
		
		
		
		bool VDVSubscriptionService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VDVSubscriptionService::getOutputMimeType() const
		{
			return "text/xml";
		}
}	}
