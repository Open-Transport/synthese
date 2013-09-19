
/** VDVServer class implementation.
	@file VDVServer.cpp

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

#include "VDVServer.hpp"

#include "BasicClient.h"
#include "DataExchangeModule.hpp"
#include "Env.h"
#include "VDVServerSubscription.hpp"
#include "XmlToolkit.h"
#include "ServerModule.h"
#include "ImportableTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "City.h"
#include "CityTableSync.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"
#include "DBModule.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <fstream>

using namespace boost;
using namespace boost::filesystem;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace impex;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace util::XmlToolkit;
	using namespace graph;
	using namespace geography;
	using namespace db;
	
	CLASS_DEFINITION(VDVServer, "t097_vdv_servers", 97)
	FIELD_DEFINITION_OF_OBJECT(VDVServer, "vdv_server_id", "vdv_server_ids")

	FIELD_DEFINITION_OF_TYPE(data_exchange::ServerAddress, "address", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(data_exchange::ServerPort, "port", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ServerControlCentreCode, "server_control_centre_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ClientControlCentreCode, "client_control_centre_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ServiceUrl, "service_url", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ServiceCode, "service_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TracePath, "trace_path", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TransportNetworkID, "transport_network_id", SQL_INTEGER)
	
	namespace data_exchange
	{
		const string VDVServer::TAG_SUBSCRIPTION = "subscription";



		VDVServer::VDVServer(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<VDVServer, VDVServerRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(ServerAddress),
					FIELD_DEFAULT_CONSTRUCTOR(ServerPort),
					FIELD_DEFAULT_CONSTRUCTOR(ServerControlCentreCode),
					FIELD_VALUE_CONSTRUCTOR(ClientControlCentreCode, "synthese"),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceUrl),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceCode),
					FIELD_DEFAULT_CONSTRUCTOR(DataSource),
					FIELD_DEFAULT_CONSTRUCTOR(TransportNetworkID),
					FIELD_DEFAULT_CONSTRUCTOR(TracePath)
			)	),
			_startServiceTimeStamp(not_a_date_time),
			_online(false)
		{
		}



		std::string VDVServer::_getURL( const std::string& request ) const
		{
			return "/" + get<ServiceUrl>() + "/" + get<ClientControlCentreCode>() + "/" + get<ServiceCode>() + "/" + request + ".xml";
		}



		void VDVServer::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(&env == &Env::GetOfficialEnv())
			{
				DataExchangeModule::AddVDVServer(*this);
			}
		}



		void VDVServer::unlink()
		{
			if(Env::GetOfficialEnv().contains(*this))
			{
				DataExchangeModule::RemoveVDVServer(get<Name>());
			}
		}



		void VDVServer::addSubscription(
			VDVServerSubscription* subscription
		){
			_subscriptions.insert(subscription);
		}



		void VDVServer::removeSubscription(
			VDVServerSubscription* subscription
		){
			_subscriptions.erase(subscription);
		}


		void VDVServer::connect() const
		{
			ptime now(second_clock::local_time());
			ptime localNow(now);
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;
			const string contentType = "text/xml";
			
			BasicClient c(
				get<ServerAddress>(),
				get<ServerPort>()
			);

			stringstream statusAnfrage;
			statusAnfrage <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<StatusAnfrage Sender=\"" << get<ClientControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(statusAnfrage, now);
			statusAnfrage <<
				"\"/>";

			bool reloadNeeded(!_online);
			bool updateFromServer(false);
			try
			{
				string statusAntwortStr(
					c.post(_getURL("status"), statusAnfrage.str(), contentType)
				);

				// Trace
				trace("StatusAnfrage", statusAnfrage.str());
				trace("StatusAntwort", statusAntwortStr);

				XMLResults results;
				XMLNode allNode = XMLNode::parseString(statusAntwortStr.c_str(), "StatusAntwort", &results);
				if (results.error != eXMLErrorNone)
				{
					_online = false;
					return;
				}
				
				XMLNode statusNode = allNode.getChildNode("Status");
				if (statusNode.isEmpty())
				{
					_online = false;
					return;
				}
				string ergebinsAttr(statusNode.getAttribute("Ergebnis"));
				if(ergebinsAttr != "ok")
				{
					_online = false;
					return;
				}

				// Check of the start time
				XMLNode startServiceNode = allNode.getChildNode("StartDienstZst");
				if(startServiceNode.isEmpty())
				{
					_online = false;
					return;
				}
				ptime startServiceTime(
					XmlToolkit::GetXsdDateTime(
						startServiceNode.getText()
				) + diff_from_utc); //We are supposed to receive the UTC time
				if(startServiceTime != _startServiceTimeStamp)
				{
					reloadNeeded = true;
					_startServiceTimeStamp = startServiceTime;
				}

				// Check if new data available (DatenBereit)
				XMLNode datenBereitServiceNode = allNode.getChildNode("DatenBereit");
				if(!datenBereitServiceNode.isEmpty())
				{
					string datenBereitValue(datenBereitServiceNode.getText());
					if(datenBereitValue == "true")
					{
						// New data available !
						updateFromServer = true;
					}
				}
			}
			catch(...)
			{
				Log::GetInstance().warn("Error while reading StatusAntwort");
				_online = false;
				return;
			}

			// Check if at least one subscription has changed
			if(!reloadNeeded)
			{
				BOOST_FOREACH(VDVServerSubscription* subscription, _subscriptions)
				{
					if(!subscription->get<StopArea>())
					{
						continue;
					}

					// If a subscription is offline, reload
					// Check if the subscription has expired
					if(!subscription->getOnline() ||
						subscription->getExpiration().is_not_a_date_time() ||
						subscription->getExpiration() < localNow
					){
						reloadNeeded = true;
						break;
					}
				}
			}

			if (updateFromServer)
			{
				// Run the update in a separate thread
				ServerModule::AddThread(boost::bind(&VDVServer::updateSYNTHESEFromServer, this), "VDV servers connector");
			}

			if(!reloadNeeded)
			{
				return;
			}
			
			this_thread::sleep(seconds(1));

			// Clean subscriptions
			now = second_clock::local_time() - diff_from_utc;
			stringstream cleanRequest;
			cleanRequest <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<AboAnfrage Sender=\"" << get<ClientControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(cleanRequest, now);
			cleanRequest <<
				"\">" <<
				"<AboLoeschenAlle>true</AboLoeschenAlle>" <<
				"</AboAnfrage>"
			;

			try
			{
				string cleanAntwort(
					c.post(
						_getURL("aboverwalten"),
						cleanRequest.str(),
						contentType
				)	);

				// Trace
				trace("AboAnfrage", cleanRequest.str());
				trace("AboAntwort", cleanAntwort);
			}
			catch(...)
			{
			}


			this_thread::sleep(seconds(1));

			// Send subscription request
			now = second_clock::local_time() - diff_from_utc;

			stringstream aboAnfrage;
			aboAnfrage <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<AboAnfrage Sender=\"" << get<ClientControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(aboAnfrage, now);
			aboAnfrage << "\">";

			BOOST_FOREACH(VDVServerSubscription* subscription, _subscriptions)
			{
				if(!subscription->get<StopArea>())
				{
					continue;
				}
				subscription->setOnline(false);

				// Expiration time
				ptime expirationTime(
					(localNow.time_of_day() <= time_duration(2, 30, 0)) ? localNow.date() : localNow.date() + days(1),
					time_duration(2,30, 0)
				);
				subscription->setExpiration(expirationTime);
				expirationTime -= diff_from_utc;
				aboAnfrage << 
					"<AboAZB AboID=\"" << subscription->get<Key>() << "\" VerfallZst=\"";
				ToXsdDateTime(aboAnfrage, expirationTime);
				aboAnfrage << "\">";

				aboAnfrage << 
					"<AZBID>" << (
						get<DataSource>() ?
						subscription->get<StopArea>()->getACodeBySource(*get<DataSource>()) :
						lexical_cast<string>(subscription->get<StopArea>()->getKey())
					) << "</AZBID>" <<
					//"<LinienID>" << "</LinienID>" <<
					//"<RichtungsID>ZVV105B</RichtungsID>
					"<Vorschauzeit>" << (subscription->get<TimeSpan>().total_seconds() / 60)  << "</Vorschauzeit>" <<
					"<Hysterese>60</Hysterese>" <<
					"</AboAZB>"
				;
			}

			aboAnfrage << "</AboAnfrage>";

			try
			{
				string aboAntwortStr(
					c.post(
						_getURL("aboverwalten"),
						aboAnfrage.str(),
						contentType
				)	);

				// Trace
				trace("AboAnfrage", aboAnfrage.str());
				trace("AboAntwort", aboAntwortStr);

				XMLResults aboAntwortResults;
				XMLNode aboAntwortNode = XMLNode::parseString(aboAntwortStr.c_str(), "AboAntwort", &aboAntwortResults);
				if (aboAntwortResults.error != eXMLErrorNone ||
					aboAntwortNode.isEmpty()
				){
					_online = false;
					return;
				}
				XMLNode bestaetingungNode = aboAntwortNode.getChildNode("Bestaetigung");
				string ergebinsAttr(bestaetingungNode.getAttribute("Ergebnis"));
				if(ergebinsAttr != "ok")
				{
					_online = false;
					return;
				}
			}
			catch(...)
			{
				_online = false;
				return;
			}

			_online = true;
			BOOST_FOREACH(VDVServerSubscription* subscription, _subscriptions)
			{
				if(!subscription->get<StopArea>())
				{
					continue;
				}
				subscription->setOnline(true);
			}
		}



		void VDVServer::updateSYNTHESEFromServer() const
		{
			this_thread::sleep(boost::posix_time::seconds(5));

			// Local variables
			ptime now(second_clock::local_time());
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;
			const string contentType = "text/xml";

			// The request
			stringstream request;
			request <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<DatenAbrufenAnfrage Sender=\"" <<
				get<ClientControlCentreCode>() <<
				"\" Zst=\"";
			ToXsdDateTime(request, now);
			request <<
				"\">" <<
				"<DatensatzAlle>0</DatensatzAlle>" <<
				"</DatenAbrufenAnfrage>"
			;

			// Sending the request
			string result;
			try
			{
				BasicClient c(
					get<ServerAddress>(),
					get<ServerPort>()
				);
				result = c.post(
					_getURL("datenabrufen"),
					request.str(),
					contentType
				);

				// Trace
				trace("DatenAbrufenAnfrage", request.str());
				trace("DatenAbrufenAntwort", result);

				boost::shared_ptr<pt::TransportNetwork> network = TransportNetworkTableSync::GetEditable(get<TransportNetworkID>(), Env::GetOfficialEnv());
				if (!network.get())
				{
					// The network does not exist : log so that the user knows he has to configure it
					Log::GetInstance().warn("Le réseau associé à la connexion VDV n'a pas été trouvé");
					return;
				}

				// Read the results :
				// 1. Get the services linked to the datasource
				// 2. Read the XML data and update the data
				// Services linked to the datasource
				
				ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(*(get<DataSource>()), Env::GetOfficialEnv());
				ImportableTableSync::ObjectBySource<StopPointTableSync> stops(*(get<DataSource>()), Env::GetOfficialEnv());
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(*(get<DataSource>()), Env::GetOfficialEnv());
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Map::value_type& itLine, lines.getMap())
				{
					BOOST_FOREACH(const ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Map::mapped_type::value_type& line, itLine.second)
					{
						JourneyPatternTableSync::Search(Env::GetOfficialEnv(), line->getKey());
						ScheduledServiceTableSync::Search(Env::GetOfficialEnv(), optional<RegistryKeyType>(), line->getKey());
						BOOST_FOREACH(const Path* route, line->getPaths())
						{
							LineStopTableSync::Search(Env::GetOfficialEnv(), route->getKey());
						}
				}	}
				
				// Parse the data
				XMLResults datenAbrufenAntwortResults;
				XMLNode datenAbrufenAntwortNode = XMLNode::parseString(result.c_str(), "DatenAbrufenAntwort", &datenAbrufenAntwortResults);
				if (datenAbrufenAntwortResults.error != eXMLErrorNone ||
					datenAbrufenAntwortNode.isEmpty()
				){
					Log::GetInstance().warn("Réception d'un DatenAbrufenAntwort vide ou mal formé");
					return;
				}
				
				// Loop on AZBNachricht to link with a subscription
				int numAZBNachricht = datenAbrufenAntwortNode.nChildNode("AZBNachricht");
				for (int cptAZBNachricht = 0;cptAZBNachricht<numAZBNachricht;cptAZBNachricht++)
				{
					XMLNode AZBNachrichtNode = datenAbrufenAntwortNode.getChildNode("AZBNachricht", cptAZBNachricht);
					// Get the id of the subscription to find it
					string aboId = AZBNachrichtNode.getAttribute("AboID");
					VDVServerSubscription* currentSubscription = NULL;
					BOOST_FOREACH(VDVServerSubscription* subscription, _subscriptions)
					{
						if (lexical_cast<string>(subscription->getKey()) == aboId)
						{
							currentSubscription = subscription;
							break;
						}
					}
					
					// If no corresponding subscription, log it and continue
					if (!currentSubscription)
					{
						Log::GetInstance().warn("Réception d'un DatenAbrufenAntwort contenant un AZBNachtricht ne correspondant à aucun abonnement");
						continue;
					}
					
					// Loop on AZBFahrplanlage => new service or RTupdate services
					int numAZBFahrplanlage = AZBNachrichtNode.nChildNode("AZBFahrplanlage");
					DBTransaction transaction;
					for (int cptAZBFahrplanlage = 0;cptAZBFahrplanlage<numAZBFahrplanlage;cptAZBFahrplanlage++)
					{
						XMLNode AZBFahrplanlageNode = AZBNachrichtNode.getChildNode("AZBFahrplanlage", cptAZBFahrplanlage);
						// Verify that the AZBID is the right one for the subscription
						string readAZBID = AZBFahrplanlageNode.getChildNode("AZBID").getText();
						if (readAZBID != currentSubscription->get<StopArea>()->getACodeBySource(*get<DataSource>()))
						{
							Log::GetInstance().warn("Réception d'un DatenAbrufenAntwort contenant un AZBNachtricht avec un AZBID ne correspondant à l'abonnement");
							break;
						}
						
						// Searching if commercial line exists by LinienID
						string commercialLineCode = AZBFahrplanlageNode.getChildNode("LinienID").getText();
						CommercialLine* line = NULL;
						if(lines.contains(commercialLineCode))
						{
							line = *lines.get(commercialLineCode).begin();
							if(line)
							{
								if(line->getPaths().empty())
								{
									JourneyPatternTableSync::Search(Env::GetOfficialEnv(), line->getKey());
									ScheduledServiceTableSync::Search(Env::GetOfficialEnv(), optional<RegistryKeyType>(), line->getKey());
									BOOST_FOREACH(const Path* route, line->getPaths())
									{
										LineStopTableSync::Search(Env::GetOfficialEnv(), route->getKey());
									}
								}
							}
						}
						// if commercial line does not exist, create it
						if (!line)
						{
							line = new CommercialLine(CommercialLineTableSync::getId());
							Log::GetInstance().info("Creation of the commercial line with key " + commercialLineCode);
							line->setParent(*network);
							Importable::DataSourceLinks links;
							links.insert(make_pair(&*(get<DataSource>()), commercialLineCode));
							line->setDataSourceLinksWithoutRegistration(links);
							Env::GetOfficialEnv().getEditableRegistry<CommercialLine>().add(boost::shared_ptr<CommercialLine>(line));
							lines.add(*line);
						}
						// Update the line
						string lineName = AZBFahrplanlageNode.getChildNode("LinienText").getText();
						if (!lineName.empty())
						{
							line->setName(lineName);
							line->setShortName(lineName);
						}
						CommercialLineTableSync::Save(line, transaction);
						
						// Searching if route exists by looping on the stops
						string strStops = AZBFahrplanlageNode.getChildNode("ViaHst1Lang").getText(); // code1;name1;code2;name2;code3;name3
						vector<string> vectStrStops;
						split(vectStrStops, strStops, is_any_of(";"));
						JourneyPattern::StopsWithDepartureArrivalAuthorization stopsOfRoute;
						// Searching for the first stop point by lloking after HaltID and HaltepositionsText
						string firstStopPointCode = AZBFahrplanlageNode.getChildNode("HaltID").getText();
						string firstStopPointName = "";
						int numHaltepositionsText = AZBFahrplanlageNode.nChildNode("HaltepositionsText");
						if (numHaltepositionsText > 0)
							firstStopPointName = AZBFahrplanlageNode.getChildNode("HaltepositionsText").getText();
						//Search (and create if not exists) first stop point
						{
							StopPoint* stopPoint = NULL;
							if (stops.contains(firstStopPointCode))
							{
								stopPoint = *stops.get(firstStopPointCode).begin();
							}
							else
							{
								Log::GetInstance().info("Create stopPoint " + firstStopPointName + " (" + firstStopPointCode + ")");
								stopPoint = new StopPoint(StopPointTableSync::getId());
								Importable::DataSourceLinks links;
								links.insert(make_pair(&*(get<DataSource>()), firstStopPointCode));
								stopPoint->setDataSourceLinksWithoutRegistration(links);
								stopPoint->setHub(&*(currentSubscription->get<StopArea>()));
								Env::GetOfficialEnv().getEditableRegistry<StopPoint>().add(boost::shared_ptr<StopPoint>(stopPoint));
								stops.add(*stopPoint);
							}
							if (!firstStopPointName.empty())
							{
								stopPoint->setName(firstStopPointName);
							}
							StopPointTableSync::Save(stopPoint, transaction);
							
							ImportableTableSync::ObjectBySource<StopPointTableSync>::Set linkedStops(
								stops.get(
									firstStopPointCode
							)	);
							
							JourneyPattern::StopWithDepartureArrivalAuthorization stopOfRoute(
								linkedStops
							);
							stopOfRoute._withTimes = true;
							stopsOfRoute.push_back(stopOfRoute);
						}
						
						for (unsigned int cptStops=0;cptStops<vectStrStops.size();cptStops++)
						{
							// Get name and code
							string codeStop = vectStrStops[cptStops];
							if (vectStrStops.size()<=cptStops+1)
							{
								// Stop with no name, we don't insert it
								Log::GetInstance().warn("Un parcours est incohérent (ViaHst1Lang = " + strStops + ")");
								break;
							}
							string nameStop = vectStrStops[cptStops+1];
							string cityName = vectStrStops[cptStops+1];
							cptStops++;
							
							// Search (and create if not exists) city
							CityTableSync::SearchResult cities(
								CityTableSync::Search(
									Env::GetOfficialEnv(),
									cityName
							)	);
							City* city = NULL;
							if(!cities.empty())
							{
								city = cities.begin()->get();
							}
							else
							{
								Log::GetInstance().info("Create city " + cityName);
								city = new City(
									CityTableSync::getId(),
									cityName
								);
								Env::GetOfficialEnv().getEditableRegistry<City>().add(boost::shared_ptr<City>(city));
								CityTableSync::Save(city, transaction);
							}
							
							// Search (and create if not exists) stop area
							StopArea* stopArea = NULL;
							if(stopAreas.contains(codeStop))
							{
								stopArea = *stopAreas.get(codeStop).begin();
							}
							else
							{
								Log::GetInstance().info("Create stopArea " + nameStop + " (" + codeStop + ")");
								stopArea = new StopArea(StopAreaTableSync::getId());
								Importable::DataSourceLinks links;
								links.insert(make_pair(&*(get<DataSource>()), codeStop));
								stopArea->setDataSourceLinksWithoutRegistration(links);
								stopArea->setCity(city);
								Env::GetOfficialEnv().getEditableRegistry<StopArea>().add(boost::shared_ptr<StopArea>(stopArea));
								stopAreas.add(*stopArea);
							}
							if (!nameStop.empty())
							{
								stopArea->setName(nameStop);
							}
							
							//Search (and create if not exists) stop point
							StopPoint* stopPoint = NULL;
							if (stops.contains(codeStop))
							{
								stopPoint = *stops.get(codeStop).begin();
							}
							else
							{
								Log::GetInstance().info("Create stopPoint " + nameStop + " (" + codeStop + ")");
								stopPoint = new StopPoint(StopPointTableSync::getId());
								Importable::DataSourceLinks links;
								links.insert(make_pair(&*(get<DataSource>()), codeStop));
								stopPoint->setDataSourceLinksWithoutRegistration(links);
								stopPoint->setHub(stopArea);
								Env::GetOfficialEnv().getEditableRegistry<StopPoint>().add(boost::shared_ptr<StopPoint>(stopPoint));
								stops.add(*stopPoint);
							}
							if (!nameStop.empty())
							{
								stopPoint->setName(nameStop);
							}
							
							ImportableTableSync::ObjectBySource<StopPointTableSync>::Set linkedStops(
								stops.get(
									codeStop
							)	);
							
							JourneyPattern::StopWithDepartureArrivalAuthorization stopOfRoute(
								linkedStops
							);
							if (cptStops == vectStrStops.size() - 1)
								stopOfRoute._withTimes = true;
							else
								stopOfRoute._withTimes = false;
							stopsOfRoute.push_back(stopOfRoute);
							
							StopAreaTableSync::Save(stopArea, transaction);
							StopPointTableSync::Save(stopPoint, transaction);
						}
						
						// Search for the route
						JourneyPattern* journeyPattern = NULL;
						BOOST_FOREACH(Path* route, line->getPaths())
						{
							// Avoid junctions
							if(!dynamic_cast<JourneyPattern*>(route))
							{
								continue;
							}
							JourneyPattern* jp(static_cast<JourneyPattern*>(route));
							if(!jp->hasLinkWithSource(*(get<DataSource>())))
							{
								continue;
							}
							if (jp->compareStopAreas(stopsOfRoute))
							{
								journeyPattern = jp;
							}
						}
						// Name of the route
						string origin = AZBFahrplanlageNode.getChildNode("VonRichtungsText").getText();
						string destination = AZBFahrplanlageNode.getChildNode("RichtungsText").getText();
						string nameOfTheRoute = origin + " - " + destination;
						if (!journeyPattern)
						{
							Log::GetInstance().info("Create route " + nameOfTheRoute);
							journeyPattern = new JourneyPattern(
								JourneyPatternTableSync::getId()
							);
							// Line link
							journeyPattern->setCommercialLine(line);
							line->addPath(journeyPattern);
							// Source links
							Importable::DataSourceLinks links;
							links.insert(make_pair(&*(get<DataSource>()), string()));
							journeyPattern->setDataSourceLinksWithoutRegistration(links);
							// Storage in the environment
							Env::GetOfficialEnv().getEditableRegistry<JourneyPattern>().add(boost::shared_ptr<JourneyPattern>(journeyPattern));
							// Served stops
							size_t rank(0);
							bool first(true);
							bool last(false);
							BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, stopsOfRoute)
							{
								boost::shared_ptr<DesignatedLinePhysicalStop> ls(
									new DesignatedLinePhysicalStop(
										LineStopTableSync::getId(),
										journeyPattern,
										rank,
										rank+1 < stopsOfRoute.size() && stop._departure,
										rank > 0 && stop._arrival,
										0,
										*stop._stop.begin(),
										first || last
								)	);
								journeyPattern->addEdge(*ls);
								Env::GetOfficialEnv().getEditableRegistry<LineStop>().add(ls);
								++rank;
								first = false;
								if (rank == stopsOfRoute.size()-1)
								{
									last = true;
								}
							}
						}
						journeyPattern->setName(nameOfTheRoute);
						
						JourneyPatternTableSync::Save(journeyPattern, transaction);
						BOOST_FOREACH(Edge* edge, journeyPattern->getEdges())
						{
							LineStopTableSync::Save(static_cast<LineStop*>(edge), transaction);
						}
						
						// Searching if service exists by FahrtBezeichner code
						string serviceCode = AZBFahrplanlageNode.getChildNode("FahrtID").getChildNode("FahrtBezeichner").getText();
						ScheduledService::Schedules departureSchedules;
						ScheduledService::Schedules arrivalSchedules;
						int numAnkunftszeitAZBPlan = AZBFahrplanlageNode.nChildNode("AnkunftszeitAZBPlan");
						ptime theoricalArrivalDate(not_a_date_time);
						if (numAnkunftszeitAZBPlan > 0)
						{
							XMLNode nodeTheoricalArrivalTime = AZBFahrplanlageNode.getChildNode("AnkunftszeitAZBPlan");
							theoricalArrivalDate = XmlToolkit::GetXsdDateTime(
								nodeTheoricalArrivalTime.getText()) + diff_from_utc; //We are supposed to receive the UTC time
						}
						int numAbfahrtszeitAZBPlan = AZBFahrplanlageNode.nChildNode("AbfahrtszeitAZBPlan");
						ptime theoricalDepartureDate(not_a_date_time);
						if (numAbfahrtszeitAZBPlan > 0)
						{
							XMLNode nodeTheoricalDepartureTime = AZBFahrplanlageNode.getChildNode("AbfahrtszeitAZBPlan");
							theoricalDepartureDate = XmlToolkit::GetXsdDateTime(
								nodeTheoricalDepartureTime.getText()) + diff_from_utc; //We are supposed to receive the UTC time
						}
						time_duration theoricalDepartureTime = theoricalDepartureDate.is_not_a_date_time() ? time_duration(0,0,0) : theoricalDepartureDate.time_of_day();
						time_duration theoricalArrivalTime = theoricalArrivalDate.is_not_a_date_time()? theoricalDepartureTime : theoricalArrivalDate.time_of_day();
						// round of the seconds
						theoricalDepartureTime -= seconds(theoricalDepartureTime.seconds());
						if(theoricalArrivalTime.seconds())
						{
							theoricalArrivalTime += seconds(60 - theoricalArrivalTime.seconds());
						}
						// Fake hours to have a coherent schedule for the journey pattern so that the service can be created
						time_duration theoricalDepartureTimeAtEndStop = theoricalDepartureTime + hours(1) + minutes(5);
						time_duration theoricalArrivalTimeAtEndStop = theoricalDepartureTime + hours(1);
						// storage of the times
						departureSchedules.push_back(theoricalDepartureTime);
						arrivalSchedules.push_back(theoricalArrivalTime);
						departureSchedules.push_back(theoricalDepartureTimeAtEndStop);
						arrivalSchedules.push_back(theoricalArrivalTimeAtEndStop);
						ScheduledService* service(NULL);
						{
							boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
								*journeyPattern->sharedServicesMutex
							);
							BOOST_FOREACH(Service* sservice, journeyPattern->getServices())
							{
								service = dynamic_cast<ScheduledService*>(sservice);
								if(!service)
								{
									continue;
								}
								if(service->comparePlannedSchedules(departureSchedules, arrivalSchedules))
								{
									// This the service to use
									break;
								}
								service = NULL;
							}
						}
						if (!service)
						{
							service = new ScheduledService(
								ScheduledServiceTableSync::getId(),
								string(),
								journeyPattern
							);
							// Complete schedules to create service (maybe the line ...
							bool last(false);
							bool first(true);
							size_t rank(0);
							departureSchedules.clear();
							arrivalSchedules.clear();
							departureSchedules.push_back(theoricalDepartureTime);
							arrivalSchedules.push_back(theoricalArrivalTime);
							BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, stopsOfRoute)
							{
								if (!last && !first)
								{
									departureSchedules.push_back(time_duration(0,0,0));
									arrivalSchedules.push_back(time_duration(0,0,0));
								}
								++rank;
								first = false;
								if (rank == stopsOfRoute.size()-1)
								{
									last = true;
								}
							}
							departureSchedules.push_back(theoricalDepartureTimeAtEndStop);
							arrivalSchedules.push_back(theoricalArrivalTimeAtEndStop);
							service->setDataSchedules(departureSchedules, arrivalSchedules);
							service->setPath(journeyPattern);
							service->addCodeBySource(*(get<DataSource>()), serviceCode);
							date today(day_clock::local_day());
							service->setActive(today);
							journeyPattern->addService(*service, false);
							Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().add(boost::shared_ptr<ScheduledService>(service));
							Log::GetInstance().info("Create service " + serviceCode);
						}
						else
						{
							date today(day_clock::local_day());
							service->setActive(today);
						}
						
						ScheduledServiceTableSync::Save(service, transaction);

					} // End loop on AZBFahrplanlage
					
					transaction.run();
					
					// Loop on AZBFahrtLoeschen => set services inactive
					int numAZBFahrtLoeschen = AZBNachrichtNode.nChildNode("AZBFahrtLoeschen");
					for (int cptAZBFahrtLoeschen = 0;cptAZBFahrtLoeschen<numAZBFahrtLoeschen;cptAZBFahrtLoeschen++)
					{
						XMLNode AZBFahrtLoeschenNode = AZBNachrichtNode.getChildNode("AZBFahrtLoeschen", cptAZBFahrtLoeschen);
						// Verify that the AZBID is the right one for the subscription
						string readAZBID = AZBFahrtLoeschenNode.getChildNode("AZBID").getText();
						if (readAZBID != currentSubscription->get<StopArea>()->getACodeBySource(*get<DataSource>()))
						{
							Log::GetInstance().warn("Réception d'un DatenAbrufenAntwort contenant un AZBFahrtLoeschen avec un AZBID ne correspondant pas à l'abonnement");
							break;
						}
						
						// Searching if commercial line exists by LinienID
						string commercialLineCode = AZBFahrtLoeschenNode.getChildNode("LinienID").getText();
						CommercialLine* line = NULL;
						if(lines.contains(commercialLineCode))
						{
							line = *lines.get(commercialLineCode).begin();
							if(line)
							{
								if(line->getPaths().empty())
								{
									JourneyPatternTableSync::Search(Env::GetOfficialEnv(), line->getKey());
									ScheduledServiceTableSync::Search(Env::GetOfficialEnv(), optional<RegistryKeyType>(), line->getKey());
									BOOST_FOREACH(const Path* route, line->getPaths())
									{
										LineStopTableSync::Search(Env::GetOfficialEnv(), route->getKey());
									}
								}
							}
						}
						if (!line)
						{
							// Commercial line does not exist, so service does not exist, we do nothing
							continue;
						}
						
						// Commercial line exists, we can't find route in FahrtLoeschen so we loop on the routes of the line
						// Searching for the service by FahrtBezeichner code
						string serviceCode = AZBFahrtLoeschenNode.getChildNode("FahrtID").getChildNode("FahrtBezeichner").getText();
						ScheduledService* service(NULL);
						BOOST_FOREACH(Path* route, line->getPaths())
						{
							// Avoid junctions
							if(!dynamic_cast<JourneyPattern*>(route))
							{
								continue;
							}
							JourneyPattern* jp(static_cast<JourneyPattern*>(route));
							if(!jp->hasLinkWithSource(*(get<DataSource>())))
							{
								continue;
							}
							
							// Scope to use a lock in a minimal scope
							{
								boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
									*jp->sharedServicesMutex
								);
								BOOST_FOREACH(Service* sservice, jp->getServices())
								{
									service = dynamic_cast<ScheduledService*>(sservice);
									if(!service)
									{
										continue;
									}
									if(service->getACodeBySource(*(get<DataSource>())) == serviceCode)
									{
										// This the service to use
										break;
									}
									service = NULL;
								}
								if (service)
								{
									// Service is found
									break;
								}
							}
						}
						
						if (!service)
						{
							Log::GetInstance().warn("Réception d'un DatenAbrufenAntwort contenant un AZBFahrtLoeschen pour un service inconnu");
						}
						else
						{
							date today(day_clock::local_day());
							service->setInactive(today);
							ScheduledServiceTableSync::Save(service);
						}
					
					} //END Loop on AZBFahrtLoeschen
				}// END loop on AZBNachricht
			}
			catch(...)
			{
			}
		}



		void VDVServer::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			// Subscription
			BOOST_FOREACH(const Subscriptions::value_type& subscription, _subscriptions)
			{
				boost::shared_ptr<ParametersMap> subscriptionMap(new ParametersMap);
				subscription->toParametersMap(*subscriptionMap, true);
				map.insert(prefix + TAG_SUBSCRIPTION, subscriptionMap);
			}
		}



		void VDVServer::trace(
			const std::string& tag,
			const std::string& content
		) const {

			if(	!get<TracePath>().empty()
			){
				ptime now(second_clock::local_time());
				stringstream dateDirName;
				dateDirName <<
					now.date().year() << "-" <<
					setw(2) << setfill('0') << int(now.date().month()) << "-" <<
					setw(2) << setfill('0') << now.date().day()
				;
				stringstream fileName;
				fileName <<
					setw(2) << setfill('0') << now.time_of_day().hours() << "-" <<
					setw(2) << setfill('0') << now.time_of_day().minutes() << "-" <<
					setw(2) << setfill('0') << now.time_of_day().seconds() <<
					"_" << tag << ".xml"
				;
				path p(get<TracePath>());
				p = p / dateDirName.str() / "servers" / get<Name>();
				create_directories(p);
				p = p / fileName.str();
				ofstream logFile(p.file_string().c_str());
				logFile << content;
			}
		}
}	}
