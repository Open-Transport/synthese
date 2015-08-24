
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
#include "ServerModule.h"
#include "Service.h"
#include "StopPoint.hpp"
#include "TransportNetwork.h"
#include "VDVClient.hpp"
#include "VDVClientSubscription.hpp"
#include "VDVServer.hpp"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace boost;
using namespace boost::posix_time;
using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace graph;
	using namespace impex;
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
			XMLNode allNode = XMLNode::parseString(content.c_str(), "DatenAbrufenAnfrage", &results);
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
					if ((string)(allDataNode.getText()) == "true")
						_withNonUpdatedContent = true;
					else
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
			// get upgradable access
			boost::upgrade_lock<boost::shared_mutex> lock(ServerModule::IneoBDSIAgainstVDVMutex);
			// get exclusive access
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

			// Client update
			Log::GetInstance().debug("VDVDataSupply : starting run");
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
				"<DatenAbrufenAntwort>" <<
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
			
			Log::GetInstance().debug("VDVDataSupply : finished headers, starting content");

			if(!error)
			{
				IConv iconv("UTF-8", "ISO-8859-1");

				BOOST_FOREACH(const VDVClient::Subscriptions::value_type& it, _vdvClient->getSubscriptions())
				{
					// Run an update
					Log::GetInstance().debug("VDVDataSupply : check update of AboID " + it.second->getId());
					it.second->checkUpdate();

					if(it.second->getDeletions().empty() && it.second->getAddings().empty())
					{
						continue;
					}

					result << "<AZBNachricht AboID=\"" << it.second->getId() << "\">";

					// Addings
					Log::GetInstance().debug("VDVDataSupply : starting addings of AboID " + it.second->getId());
					try
					{
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
							// Check that network is OK
							bool networkOK(false);
							BOOST_FOREACH(const TransportNetworks::Type::value_type& transportNetwork, _vdvClient->get<TransportNetworks>())
							{
								if (network.getKey() == transportNetwork->getKey())
								{
									networkOK = true;
									break;
								}
							}
							if (!networkOK)
							{
								continue;
							}
							Log::GetInstance().debug("VDVDataSupply : Network is OK");
							ptime departureDateTime(sp.getDepartureDateTime());
							if (!departureDateTime.is_not_a_date_time())
								departureDateTime -= diff_from_utc;
							ptime plannedDepartureDateTime(sp.getTheoreticalDepartureDateTime());
							if (!plannedDepartureDateTime.is_not_a_date_time())
								plannedDepartureDateTime -= diff_from_utc;

							bool isRealTime(sp.getRTData());

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
							if (!arrivalDateTime.is_not_a_date_time())
							{
								arrivalDateTime -= diff_from_utc;
								// Patch for arrivals after midnight (not managed by DEPARTURE board)
								// arrival can not be after departure !!
								if (!departureDateTime.is_not_a_date_time() &&
									arrivalDateTime.date() > departureDateTime.date())
								{
									arrivalDateTime -= days(1);
								}
							}
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
							if (!plannedArrivalDateTime.is_not_a_date_time())
							{
								plannedArrivalDateTime -= diff_from_utc;
								// Patch for arrivals after midnight (not managed by DEPARTURE board)
								// arrival can not be after departure !!
								if (!plannedDepartureDateTime.is_not_a_date_time() &&
									plannedArrivalDateTime.date() > plannedDepartureDateTime.date())
								{
									plannedArrivalDateTime -= days(1);
								}
							}
							Log::GetInstance().debug("VDVDataSupply : Network " + lexical_cast<string>(network.getKey()));
							string networkId = "";
							try {
								networkId = network.getACodeBySource(
									*_vdvClient->get<DataSource>()
								);
							}
							catch (...) {
								Log::GetInstance().debug("VDVDataSupply : Exception reading network code of " + lexical_cast<string>(network.getKey()));
							}
							Log::GetInstance().debug("VDVDataSupply : Network id " + networkId);
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
							Log::GetInstance().debug("VDVDataSupply : Service number " + serviceNumber);
							string direction;
							if(jp.getDirectionObj())
							{
								direction = jp.getDirectionObj()->get<DisplayedText>();
							}
							else if(!jp.getDirection().empty())
							{
								direction = jp.getDirection();
							}
							trim(direction);
							if(direction.empty())
							{
								direction = jp.getDestination()->getConnectionPlace()->getName26();
							}
							if(direction.empty())
							{
								direction = jp.getDestination()->getConnectionPlace()->getName();
							}
							direction = iconv.convert(direction);
							Log::GetInstance().debug("VDVDataSupply : Direction " + direction);

							//Provenance
							string provenance = jp.getOrigin()->getConnectionPlace()->getName26();
							if(provenance.empty())
							{
								provenance = jp.getOrigin()->getConnectionPlace()->getName();
							}
							provenance = iconv.convert(provenance);
							Log::GetInstance().debug("VDVDataSupply : Provenance " + provenance);

							// Expiration time
							ptime expirationTime(
								(now.time_of_day() <= time_duration(2, 30, 0)) ? now.date() : now.date() + days(1),
								time_duration(2,30, 0)
								);
							expirationTime -= diff_from_utc;

							// XML generation
							result << "<AZBFahrplanlage Zst=\"";
							ToXsdDateTime(result, now);
							result << "\" VerfallZst=\"";
							ToXsdDateTime(result, expirationTime);

							// Halt ID
							string haltID = "";
							if (sp.getDepartureEdge()->getFromVertex() &&
								dynamic_cast<const StopPoint*>(sp.getDepartureEdge()->getFromVertex()))
							{
								const StopPoint* ps_test = static_cast<const StopPoint*>(sp.getDepartureEdge()->getFromVertex());
								try {
									haltID = ps_test->getACodeBySource(*_vdvClient->get<DataSource>());
								}
								catch (...) {
									Log::GetInstance().debug("VDVDataSupply : Exception reading stop point code of " + lexical_cast<string>(ps_test->getKey()));
								}
							}
							Log::GetInstance().debug("VDVDataSupply : HaltID " + haltID);
							Log::GetInstance().debug("VDVDataSupply : Betriebstag " + to_iso_extended_string((sp.getOriginDateTime() - diff_from_utc).date()));
							try {
								Log::GetInstance().debug("VDVDataSupply : LinienID " + line.getACodeBySource(*_vdvClient->get<DataSource>()));
							}
							catch (...) {
								Log::GetInstance().debug("VDVDataSupply : Exception reading line code of " + lexical_cast<string>(line.getKey()));
							}
							Log::GetInstance().debug("VDVDataSupply : LinienText " + line.getShortName());
							Log::GetInstance().debug("VDVDataSupply : RichtungsID " + _vdvClient->getDirectionID(jp));
							string networkName = network.getName();
							trim(networkName);
							Log::GetInstance().debug("VDVDataSupply : Betreiber " + networkName);
							bool inStopArea = false;
							if(arrivalDateTime.is_not_a_date_time())
							{
								if(!plannedArrivalDateTime.is_not_a_date_time())
								{
									time_duration timeToStop = plannedArrivalDateTime - now;
									if (timeToStop < time_duration(0,0,30))
									{
										inStopArea = true;
										if(!plannedDepartureDateTime.is_not_a_date_time())
										{
											time_duration timeToLeave = plannedDepartureDateTime - now;
											if (timeToLeave > time_duration(0,0,-30))
											{
												inStopArea = false;
											}
										}
									}
								}
							}
							else
							{
								if(!arrivalDateTime.is_not_a_date_time())
								{
									time_duration timeToStop = arrivalDateTime - now;
									if (timeToStop < time_duration(0,0,30))
									{
										inStopArea = true;
										if(!departureDateTime.is_not_a_date_time())
										{
											time_duration timeToLeave = departureDateTime - now;
											if (timeToLeave > time_duration(0,0,-30))
											{
												inStopArea = false;
											}
										}
									}
								}
							}
							result <<
								"\">" <<
								"<AZBID>" << it.second->getStopArea()->getACodeBySource(*_vdvClient->get<DataSource>()) << "</AZBID>" <<
								"<FahrtID>" <<
								"<FahrtBezeichner>" << serviceNumber << "</FahrtBezeichner>" <<
								"<Betriebstag>" << to_iso_extended_string((sp.getOriginDateTime() - diff_from_utc).date()) << "</Betriebstag>" <<
								"</FahrtID>" <<
								"<HstSeqZaehler>1</HstSeqZaehler>" <<
								"<LinienID>" << line.getACodeBySource(*_vdvClient->get<DataSource>())  << "</LinienID>" <<
								"<LinienText>" << line.getShortName() << "</LinienText>" <<
								"<RichtungsID>" << _vdvClient->getDirectionID(jp) << "</RichtungsID>" <<
								"<RichtungsText>" << direction << "</RichtungsText>" <<
								"<VonRichtungsText>" << provenance << "</VonRichtungsText>" <<
								"<ZielHst>" << direction << "</ZielHst>" <<
								"<AufAZB>" << (inStopArea ? "true" : "false") << "</AufAZB>" <<
								"<FahrtStatus>" << (isRealTime ? "Ist" : "Soll") << "</FahrtStatus>"
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
							result << "<HaltID>" << haltID << "</HaltID>";
							result << "<FahrtInfo><Betreiber>" << networkName << "</Betreiber></FahrtInfo>";
							result << "</AZBFahrplanlage>";
							Log::GetInstance().debug("VDVDataSupply : End of AZBFahrplanlage");
						}

						// Deletions
						Log::GetInstance().debug("VDVDataSupply : finshed addings, starting deletions of AboID " + it.second->getId());
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
							// Check that network is OK
							bool networkOK(false);
							BOOST_FOREACH(const TransportNetworks::Type::value_type& transportNetwork, _vdvClient->get<TransportNetworks>())
							{
								if (network.getKey() == transportNetwork->getKey())
								{
									networkOK = true;
									break;
								}
							}
							if (!networkOK)
							{
								continue;
							}
							string networkId = "";
							try {
								networkId = network.getACodeBySource(
									*_vdvClient->get<DataSource>()
								);
							}
							catch (...) {
								Log::GetInstance().debug("VDVDataSupply : Exception reading network code of " + lexical_cast<string>(network.getKey()));
							}
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
							string direction;
							if(jp.getDirectionObj())
							{
								direction = jp.getDirectionObj()->get<DisplayedText>();
							}
							else if(!jp.getDirection().empty())
							{
								direction = jp.getDirection();
							}
							trim(direction);
							if(direction.empty())
							{
								direction = jp.getDestination()->getConnectionPlace()->getName26();
							}
							if(direction.empty())
							{
								direction = jp.getDestination()->getConnectionPlace()->getName();
							}
							direction = iconv.convert(direction);

							// Provenance
							string provenance = jp.getOrigin()->getConnectionPlace()->getName26();
							if(provenance.empty())
							{
								provenance = jp.getOrigin()->getConnectionPlace()->getName();
							}
							provenance = iconv.convert(provenance);
						
							// XML generation
							result <<
								"<AZBFahrtLoeschen Zst=\"";
							ToXsdDateTime(result, now);
							result <<
								"\">" <<
								"<AZBID>" << it.second->getStopArea()->getACodeBySource(*_vdvClient->get<DataSource>()) << "</AZBID>" <<
								"<FahrtID>" <<
								"<FahrtBezeichner>" << serviceNumber << "</FahrtBezeichner>" <<
								"<Betriebstag>" << to_iso_extended_string((sp.getOriginDateTime() - diff_from_utc).date()) << "</Betriebstag>" <<
								"</FahrtID>" <<
								"<HstSeqZaehler>1</HstSeqZaehler>" <<
								"<LinienID>" << line.getACodeBySource(*_vdvClient->get<DataSource>())  << "</LinienID>" <<
								"<LinienText>" << line.getShortName() << "</LinienText>" <<
								"<RichtungsID>" << _vdvClient->getDirectionID(jp) << "</RichtungsID>" <<
								"<RichtungsText>" << direction << "</RichtungsText>" <<
								"<VonRichtungsText>" << provenance << "</VonRichtungsText>" <<
								"<Ursache></Ursache>" <<
								"</AZBFahrtLoeschen>"
							;
						}
					}
					catch (...)
					{
						Log::GetInstance().debug("VDVDataSupply : Exception");
					}
					Log::GetInstance().debug("VDVDataSupply : finshed deletions of AboID " + it.second->getId());
					result << "</AZBNachricht>";

					it.second->declareSending();
			}	}
			result << "</DatenAbrufenAntwort>";

			// Output the result (TODO cancel it if the service is called through the CMS)
			string strResult(result.str());
			map.insert(DATA_RESULT, strResult);
			
			// Trace
			_vdvClient->trace("DatenAbrufenAntwort", strResult);
			
			stream << strResult;

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
