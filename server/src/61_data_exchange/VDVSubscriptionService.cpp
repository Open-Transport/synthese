
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
	using namespace impex;
	using namespace pt;
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace util::XmlToolkit;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVSubscriptionService>::FACTORY_KEY = "VDVSubscription";
	
	namespace data_exchange
	{
		const std::string VDVSubscriptionService::DATA_RESULT = "result";



		VDVSubscriptionService::VDVSubscriptionService():
		FactorableTemplate<server::Function,VDVSubscriptionService>(),
			_client(NULL)
		{
		}



		ParametersMap VDVSubscriptionService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVSubscriptionService::_setFromParametersMap(const ParametersMap& map)
		{
			string content(map.getDefault<string>(PARAMETER_POST_DATA));

			XMLResults results;
			XMLNode allNode = XMLNode::parseString(content.c_str(), "AboAnfrage", &results);
			if(	results.error != eXMLErrorNone ||
				allNode.isEmpty()
			){
				_errorNumber = "100";
				_errorText = "Malformed XML";
				return;
			}

			try
			{
				string sender(allNode.getAttribute("Sender"));
				_client = &DataExchangeModule::GetVDVClient(sender);

				// Trace
				_client->trace("AboAnfrage", content);

				// Check if the client is declared as active
				if(!_client->get<Active>())
				{
					_errorNumber = "300";
					_errorText = "Sender is forbidden right now";
					return;
				}

				// Subscriptions cleaning
				XMLNode cleanNode(allNode.getChildNode("AboLoeschenAlle"));
				if(!cleanNode.isEmpty())
				{
					string cleanNodeStr(cleanNode.getText());
					if(	cleanNodeStr == "true" ||
						cleanNodeStr == "1"
					){
						_client->cleanSubscriptions();
					}
				}

				// New subscriptions
				size_t nbNodes(allNode.nChildNode("AboAZB"));
				for(size_t i(0); i<nbNodes; ++i)
				{
					XMLNode aboAZBNode(allNode.getChildNode("AboAZB", static_cast<int>(i)));
					string id(aboAZBNode.getAttribute("AboID"));
					if(id.empty())
					{
						continue;
					}

					boost::shared_ptr<VDVClientSubscription> subscription(new VDVClientSubscription(id, *_client));

					if(aboAZBNode.nChildNode("AZBID"))
					{
						XMLNode azbidNode(aboAZBNode.getChildNode("AZBID"));
						StopArea* stopArea(
							_client->get<DataSource>()->getObjectByCode<StopArea>(azbidNode.getText())
						);
						if(!stopArea)
						{
							// Try without first char (may be a Z or a S before the DIDOK code)
							string stopCode = azbidNode.getText();
							stopArea = _client->get<DataSource>()->getObjectByCode<StopArea>(stopCode.substr(1));
							if (!stopArea)
							{
								continue;
							}
						}
						subscription->setStopArea(stopArea);
					}

					if(aboAZBNode.nChildNode("LinienID"))
					{
						XMLNode linienNode(aboAZBNode.getChildNode("LinienID"));
						CommercialLine* line(
							_client->get<DataSource>()->getObjectByCode<CommercialLine>(linienNode.getText())
						);
						if(!line)
						{
							continue;
						}
						subscription->setLine(line);
					}

					// Direction filter
					if(aboAZBNode.nChildNode("RichtungsID"))
					{
						XMLNode linienNode(aboAZBNode.getChildNode("RichtungsID"));
						subscription->setDirectionFilter(linienNode.getText());
					}

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
								seconds(
									lexical_cast<long>(hysteresisNode.getText())
							)	);
							subscription->setHysteresis(hysteresis);
						}
						catch(bad_lexical_cast&)
						{
						}
					}
					
					_client->addSubscription(subscription);
				}

			}
			catch (Exception&)
			{
				_errorNumber = "200";
				_errorText = "Invalid sender";
				return;
			}

			// Error if the VDV server is inactive
			if(!DataExchangeModule::GetVDVServerActive())
			{
				_errorNumber = "400";
				_errorText = "Service temporary unavailable";
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
			bool error(!_errorNumber.empty() && _errorNumber != "0");
			
			// XML
			stringstream result;
			result << 
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<AboAntwort>" <<
				"<Bestaetigung Fehlernummer=\"" <<
				(_errorNumber.empty() ? "0" : _errorNumber) <<
				"\" Zst=\"";
			ToXsdDateTime(result, now);
			result <<
				"\" Ergebnis=\"" <<
				(error ? "notok" : "ok") <<
				"\">"
			;
			if(!_errorText.empty())
			{
				result << "<FehlerText>" << _errorText << "</FehlerText>";
			}
			result << "</Bestaetigung></AboAntwort>";
			map.insert(DATA_RESULT, result.str());

			// Output the result (TODO cancel it if the service is called through the CMS)
			stream << result.str();

			// Trace
			if(_client)
			{
				_client->trace("AboAntwort", result.str());
			}

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
