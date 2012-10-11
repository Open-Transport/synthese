
//////////////////////////////////////////////////////////////////////////////////////////
///	VDVDataSupplyService class implementation.
///	@file VDVDataSupplyService.cpp
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

#include "VDVDataSupplyService.hpp"

#include "CommercialLine.h"
#include "DataExchangeModule.hpp"
#include "Destination.hpp"
#include "IConv.hpp"
#include "JourneyPattern.hpp"
#include "Path.h"
#include "Request.h"
#include "RequestException.h"
#include "ScheduledService.h"
#include "ServerConstants.h"
#include "Service.h"
#include "TransportNetwork.h"
#include "VDVClient.hpp"
#include "VDVClientSubscription.hpp"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace pt;
	using namespace security;
	using namespace server;
	using namespace util;
	using namespace util::XmlToolkit;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVDataSupplyService>::FACTORY_KEY = "VDVDataSupply";
	
	namespace data_exchange
	{
		const string VDVDataSupplyService::DATA_RESULT = "result";
		


		VDVDataSupplyService::VDVDataSupplyService():
			FactorableTemplate<server::Function,VDVDataSupplyService>(),
			_withNonUpdatedContent(false),
			_vdvClient(NULL)
		{}



		ParametersMap VDVDataSupplyService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVDataSupplyService::_setFromParametersMap(const ParametersMap& map)
		{
			string content(map.getDefault<string>(PARAMETER_POST_DATA));

			XMLResults results;
			XMLNode allNode = XMLNode::parseString(content.c_str(), "vdv453:DatenAbrufenAnfrage", &results);
			if (results.error != eXMLErrorNone)
			{
				_errorNumber = "100";
				_errorText = "Malformed XML";
				return;
			}

			try
			{
				string sender(allNode.getAttribute("Sender"));
				_vdvClient = &DataExchangeModule::GetVDVClient(sender);

				// Trace
				_vdvClient->trace("DatenAbrufenAnfrage", content);

				// Check if the client is declared as active
				if(!_vdvClient->get<Active>())
				{
					_errorNumber = "300";
					_errorText = "Sender is forbidden right now";
					return;
				}

				XMLNode allDataNode(allNode.getChildNode("DatensatzAlle"));
				if(!allDataNode.isEmpty())
				{
					try
					{
						_withNonUpdatedContent = lexical_cast<bool>(
							allDataNode.getText()
						);
					}
					catch(bad_lexical_cast&)
					{						
					}
					
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

		ParametersMap VDVDataSupplyService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Client update
			_vdvClient->clearLastDataReady();

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
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<vdv453:DatenAbrufenAntwort xmlns:vdv453=\"vdv453ger\">" <<
				"<Bestaetigung Zst=\"";
			ToXsdDateTime(result, now);
			result <<
				"\" Ergebnis=\"" <<
				(error ? "notok" : "ok") <<
				"\" Fehlernummer=\"" <<
				(_errorNumber.empty() ? "0" : _errorNumber) <<
				"\">"
			;
			if(!_errorText.empty())
			{
				result << "<Fehlertext>" << _errorText << "</FehlerText>";
			}
			result << "</Bestaetigung>";

			if(!error)
			{
				IConv iconv("UTF-8", "ISO-8859-1");

				BOOST_FOREACH(const VDVClient::Subscriptions::value_type& it, _vdvClient->getSubscriptions())
				{
					if(it.second->getDeletions().empty() && it.second->getAddings().empty())
					{
						continue;
					}

					result << "<AZBNachricht AboID=\"" << it.second->getId() << "\">";

					// Deletions
					BOOST_FOREACH(const VDVClientSubscription::ServicesList::value_type& dep, it.second->getDeletions())
					{
						// Local variables
						const ServicePointer& sp(dep.second);
						const CommercialLine& line(
							*static_cast<CommercialLine*>(sp.getService()->getPath()->getPathGroup())
						);
						const JourneyPattern& jp(
							*static_cast<const JourneyPattern*>(sp.getService()->getPath())
						);
						const TransportNetwork& network(
							*line.getNetwork()
						);
						string networkId(
							network.getACodeBySource(
								*_vdvClient->get<DataSourcePointer>()
						)	);
						string serviceNumber;
						if(!networkId.empty())
						{
							serviceNumber = networkId + "-";
						}
						if(sp.getService()->getServiceNumber().empty())
						{
							serviceNumber += "00000";
						}
						else
						{
							serviceNumber += sp.getService()->getServiceNumber();
						}
						serviceNumber += "-" + lexical_cast<string>(sp.getService()->getKey());
					
						// XML generation
						result <<
							"<AZBFahrtLoeschen Zst=\"";
						ToXsdDateTime(result, now);
						result <<
							"\">" <<
							"<AZBID>" << it.second->getStopArea()->getACodeBySource(*_vdvClient->get<DataSourcePointer>()) << "</AZBID>" <<
							"<FahrtID>" <<
							"<FahrtBezeichner>" << serviceNumber << "</FahrtBezeichner>" <<
							"<Betriebstag>" << to_iso_extended_string(sp.getOriginDateTime().date()) << "</Betriebstag>" << 
							"</FahrtID>" <<
							"<HstSeqZaehler>1</HstSeqZaehler>" <<
							"<LinienID>" << line.getACodeBySource(*_vdvClient->get<DataSourcePointer>())  << "</LinienID>" <<
							"<LinienText>" << line.getShortName() << "</LinienText>" <<
							"<RichtungsID>" << _vdvClient->getDirectionID(jp) << "</RichtungsID>" <<
							"<RichtungsText>" << iconv.convert(jp.getDirectionObj() ? jp.getDirectionObj()->getDisplayedText() : jp.getDirection()) << "</RichtungsText>" <<
							"<AufAZB>false</AufAZB>" <<
							"<FahrtStatus>Ist</FahrtStatus>" << 
							"</AZBFahrtLoeschen>"
						;
					}

					// Addings
					BOOST_FOREACH(const VDVClientSubscription::ServicesList::value_type& dep, it.second->getAddings())
					{
						// Local variables
						const ServicePointer& sp(dep.second);
						const ScheduledService* service(
							dynamic_cast<const ScheduledService*>(
								sp.getService()
						)	);
						const JourneyPattern& jp(
							*static_cast<const JourneyPattern*>(service->getPath())
						);
						const CommercialLine& line(
							*static_cast<CommercialLine*>(jp.getPathGroup())
						);
						const TransportNetwork& network(
							*line.getNetwork()
						);
						ptime departureDateTime(sp.getDepartureDateTime());
						departureDateTime -= diff_from_utc;
						ptime plannedDepartureDateTime(sp.getTheoreticalDepartureDateTime());
						plannedDepartureDateTime -= diff_from_utc;
						
						ptime arrivalDateTime(
							(sp.getDepartureEdge() && sp.getDepartureEdge()->isArrival() && sp.getDepartureEdge()->getRankInPath()) ?
							ptime(
								sp.getOriginDateTime().date(),
								service->getArrivalSchedule(
									true,
									sp.getDepartureEdge()->getRankInPath()
							)	):
							ptime(not_a_date_time)
						);
						arrivalDateTime -= diff_from_utc;
						ptime plannedArrivalDateTime(
							(sp.getDepartureEdge() && sp.getDepartureEdge()->isArrival() && sp.getDepartureEdge()->getRankInPath()) ?
							ptime(
								sp.getOriginDateTime().date(),
								service->getArrivalSchedule(
									false,
									sp.getDepartureEdge()->getRankInPath()
							)	):
							ptime(not_a_date_time)
						);
						plannedArrivalDateTime -= diff_from_utc;
						string networkId(
							network.getACodeBySource(
								*_vdvClient->get<DataSourcePointer>()
						)	);
						string serviceNumber;
						if(!networkId.empty())
						{
							serviceNumber = networkId + "-";
						}
						if(sp.getService()->getServiceNumber().empty())
						{
							serviceNumber += "00000";
						}
						else
						{
							serviceNumber += sp.getService()->getServiceNumber();
						}
						serviceNumber += "-" + lexical_cast<string>(sp.getService()->getKey());
					
						// XML generation
						result << "<AZBFahrplanlage Zst=\"";
						ToXsdDateTime(result, now);
						result <<
							"\">" <<
							"<AZBID>" << it.second->getStopArea()->getACodeBySource(*_vdvClient->get<DataSourcePointer>()) << "</AZBID>" <<
							"<FahrtID>" <<
							"<FahrtBezeichner>" << serviceNumber << "</FahrtBezeichner>" <<
							"<Betriebstag>" << to_iso_extended_string(sp.getOriginDateTime().date()) << "</Betriebstag>" << 
							"</FahrtID>" <<
							"<HstSeqZaehler>1</HstSeqZaehler>" <<
							"<LinienID>" << line.getACodeBySource(*_vdvClient->get<DataSourcePointer>())  << "</LinienID>" <<
							"<LinienText>" << line.getShortName() << "</LinienText>" <<
							"<RichtungsID>" << _vdvClient->getDirectionID(jp) << "</RichtungsID>" <<
							"<RichtungsText>" << iconv.convert(jp.getDirectionObj() ? jp.getDirectionObj()->getDisplayedText() : jp.getDirection()) << "</RichtungsText>" <<
							"<ZielHst>" << iconv.convert(jp.getDirectionObj() ? jp.getDirectionObj()->getDisplayedText() : jp.getDirection()) << "</ZielHst>" <<
							"<AufAZB>false</AufAZB>" <<
							"<FahrtStatus>Ist</FahrtStatus>"
						;
						if(!plannedArrivalDateTime.is_not_a_date_time())
						{
							result << "<AnkunftszeitAZBPlan>";
							ToXsdDateTime(result, plannedArrivalDateTime);
							result << "</AnkunftszeitAZBPlan>";
						}
						if(!arrivalDateTime.is_not_a_date_time())
						{
							result << "<AnkunftszeitAZBPrognose>";
							ToXsdDateTime(result, arrivalDateTime);
							result << "</AnkunftszeitAZBPrognose>";
						}
						if(!plannedDepartureDateTime.is_not_a_date_time())
						{
							result << "<AbfahrtszeitAZBPlan>";
							ToXsdDateTime(result, plannedDepartureDateTime);
							result << "</AbfahrtszeitAZBPlan>";
						}
						if(!departureDateTime.is_not_a_date_time())
						{
							result << "<AbfahrtszeitAZBPrognose>";
							ToXsdDateTime(result, departureDateTime);
							result << "</AbfahrtszeitAZBPrognose>";
						}
						result << "<HaltepositionsText></HaltepositionsText>";
						result << "<HaltID></HaltID>";
						result << "</AZBFahrplanlage>";
					}
					result << "</AZBNachricht>";
					
					it.second->declareSending();
			}	}
			result << "</vdv453:DatenAbrufenAntwort>";

			// Output the result (TODO cancel it if the service is called through the CMS)
			map.insert(DATA_RESULT, result.str());
			stream << result.str();

			// Trace
			_vdvClient->trace("DatenAbrufenAntwort", result.str());

			// Map return
			return map;
		}
		
		
		
		bool VDVDataSupplyService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VDVDataSupplyService::getOutputMimeType() const
		{
			return "text/xml";
		}
}	}
