
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

#include "CommercialLine.h"
#include "DataExchangeModule.hpp"
#include "Request.h"
#include "RequestException.h"
#include "ServerConstants.h"
#include "StopArea.hpp"
#include "VDVClient.hpp"
#include "VDVClientSubscription.hpp"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace util::XmlToolkit;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVSubscriptionService>::FACTORY_KEY = "VDVAboAnfrage";
	
	namespace data_exchange
	{
		const std::string VDVSubscriptionService::DATA_RESULT = "result";



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
				_errorNumber = "100";
				_errorText = "Malformed XML";
				return;
			}

			try
			{
				string sender(allNode.getAttribute("Sender"));
				VDVClient& client(
					DataExchangeModule::GetVDVClient(sender)
				);

				// Subscriptions cleaning
				XMLNode cleanNode(allNode.getChildNode("AboLoeschenAlle"));
				if(!cleanNode.isEmpty())
				{
					if(	cleanNode.getText() == "true" ||
						cleanNode.getText() == "1"
					){
						client.cleanSubscriptions();
					}
				}

				// New subscriptions
				size_t nbNodes(allNode.nChildNode("AboAZB"));
				for(size_t i(0); i<nbNodes; ++i)
				{
					XMLNode aboAZBNode(allNode.getChildNode("AboAZB", i));

					shared_ptr<VDVClientSubscription> subscription(new VDVClientSubscription);


					if(aboAZBNode.nChildNode("AZBID"))
					{
						XMLNode azbidNode(aboAZBNode.getChildNode("AZBID"));
						StopArea* stopArea(
							client.get<DataSourcePointer>()->getObjectByCode<StopArea>(azbidNode.getText())
						);
						if(!stopArea)
						{
							continue;
						}
						subscription->setStopArea(stopArea);
					}

					if(aboAZBNode.nChildNode("LinienID"))
					{
						XMLNode linienNode(aboAZBNode.getChildNode("LinienID"));
						CommercialLine* line(
							client.get<DataSourcePointer>()->getObjectByCode<CommercialLine>(linienNode.getText())
						);
						if(line)
						{
							subscription->setLine(line);
						}
					}

					// TODO Handle directions

					XMLNode durationNode(aboAZBNode.getChildNode("Vorschauzeit"));
					if(!durationNode.isEmpty())
					{
						try
						{
							time_duration timeSpan(
								minutes(
									lexical_cast<long>(durationNode.getText())
							)	);
							subscription->setTimeSpan(timeSpan);
						}
						catch(bad_lexical_cast&)
						{
						}
					}

					XMLNode hysteresisNode(aboAZBNode.getChildNode("Hysterese"));
					if(!hysteresisNode.isEmpty())
					{
						try
						{
							time_duration hysteresis(
								minutes(
									lexical_cast<long>(hysteresisNode.getText())
							)	);
							subscription->setHysteresis(hysteresis);
						}
						catch(bad_lexical_cast&)
						{
						}
					}
					
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

			// Map creation
			ParametersMap map;

			// Local variables
			ptime now(second_clock::local_time());
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;
			
			// XML
			stringstream result;
			result << 
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:AboAntwort xmlns:vdv453=\"vdv453ger\">" <<
				"<Bestaetigung Fehlernummer=\"" <<
				(_errorNumber.empty() ? "0" : _errorNumber) <<
				"\" Zst=\"";
			ToXsdDateTime(result, now);
			result << "\"";
			if(!_errorText.empty())
			{
				result << " FehlerText=\"" << _errorText << "\"";
			}
			result <<
				" Ergebnis=\"" <<
				((!_errorNumber.empty() && _errorNumber != "0") ? "notok" : "ok") <<
				"\" />" <<
				"</vdv453:AboAntwort>"
			;
			map.insert(DATA_RESULT, result.str());

			// Output the result (TODO cancel it if the service is called through the CMS)
			stream << result.str();

			// Map return
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
