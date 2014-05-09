
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
	FIELD_DEFINITION_OF_TYPE(PlannedDataSourceID, "planned_datasource_id", SQL_INTEGER)
	
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
					FIELD_DEFAULT_CONSTRUCTOR(PlannedDataSourceID),
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

			date today(day_clock::local_day());

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

				boost::shared_ptr<impex::DataSource> plannedDataSource = DataSourceTableSync::GetEditable(get<PlannedDataSourceID>(), Env::GetOfficialEnv());
				if (!plannedDataSource.get())
				{
					// The planned does not exist : log so that the user knows he has to configure it
					Log::GetInstance().warn("La source des donn�es plannifi�es associ�e � la connexion VDV n'a pas �t� trouv�e");
					return;
				}

				// Read the results :
				// 1. Get the services linked to the datasource
				// 2. Read the XML data and update the data

				// 1 : clean the old references to the current source
				ImportableTableSync::ObjectBySource<ScheduledServiceTableSync> sourcedServices(*(get<DataSource>()), Env::GetOfficialEnv());
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<ScheduledServiceTableSync>::Map::value_type& itService, sourcedServices.getMap())
				{
					BOOST_FOREACH(const ImportableTableSync::ObjectBySource<ScheduledServiceTableSync>::Map::mapped_type::value_type& obj, itService.second)
					{
						obj->removeSourceLinks(*(get<DataSource>()));
					}
				}
				
				XMLResults datenAbrufenAntwortResults;
				XMLNode datenAbrufenAntwortNode = XMLNode::parseString(result.c_str(), "DatenAbrufenAntwort", &datenAbrufenAntwortResults);
				if (datenAbrufenAntwortResults.error != eXMLErrorNone ||
					datenAbrufenAntwortNode.isEmpty()
				){
					Log::GetInstance().warn("R�ception d'un DatenAbrufenAntwort vide ou mal form�");
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
						Log::GetInstance().warn("R�ception d'un DatenAbrufenAntwort contenant un AZBNachtricht ne correspondant � aucun abonnement");
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
							Log::GetInstance().warn("R�ception d'un DatenAbrufenAntwort contenant un AZBNachtricht avec un AZBID ne correspondant � l'abonnement");
							break;
						}
						
						// Searching if service exists by FahrtBezeichner code
						string serviceCode = AZBFahrplanlageNode.getChildNode("FahrtID").getChildNode("FahrtBezeichner").getText();
						vector<string> vectServiceCode;
						split(vectServiceCode, serviceCode, is_any_of("-"));
						if (vectServiceCode.size() != 3)
						{
							Log::GetInstance().warn("R�ception d'un DatenAbrufenAntwort contenant un service avec un code diff�rentde XXX-XXXX-XXXX");
							continue;
						}
						// Service code is on 5 characters in the planned datasource
						while (vectServiceCode[1].size() < 5)
							vectServiceCode[1] = "0" + vectServiceCode[1];
						

						ScheduledService* service(
							plannedDataSource->getObjectByCode<ScheduledService>(vectServiceCode[1])
						);

						const CommercialLine& line(
							*static_cast<CommercialLine*>(service->getPath()->getPathGroup())
						);
						vector<ScheduledService*> services;
						BOOST_FOREACH(Path* route, line.getPaths())
						{
							// Avoid junctions
							if(!dynamic_cast<JourneyPattern*>(route))
							{
								continue;
							}
							JourneyPattern* jp(static_cast<JourneyPattern*>(route));
							boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
								*jp->sharedServicesMutex
							);
							BOOST_FOREACH(Service* tservice, jp->getServices())
							{
								ScheduledService* curService(dynamic_cast<ScheduledService*>(tservice));
								if(!curService) continue;
								if (curService->getACodeBySource(*plannedDataSource) == vectServiceCode[1])
								{
									// Add the service to vect
									services.push_back(curService);
								}
							}
						}
						
						Log::GetInstance().debug("On a trouve : " + lexical_cast<string>(services.size()) + " services candidats");
						int numTheoricalActivatedServices(0);
						BOOST_FOREACH(ScheduledService* sservice, services)
						{
							if (sservice->isActive(today))
							{
								numTheoricalActivatedServices++;
								service = sservice;
							}
						}
						
						if (numTheoricalActivatedServices != 1)
						{
							Log::GetInstance().debug(lexical_cast<string>(numTheoricalActivatedServices) + " services candidats sont th�oriquement activ�s aujourd'hui");
						}
						else
						{
							Log::GetInstance().debug("un seul service candidat est th�oriquement activ� aujourd'hui");
						}
						
						if (service)
						{
							// UPDATE RT OF THE SERVICE
							SchedulesBasedService::Schedules departureSchedules(
								service->getDepartureSchedules(true, true)
							);
							SchedulesBasedService::Schedules arrivalSchedules(
								service->getArrivalSchedules(true, true)
							);
							// Search the rank of the stop of the subscription
							size_t rank(0);
							JourneyPattern* route(static_cast<JourneyPattern*>(service->getPath()));
							for (size_t cptRank(0);cptRank<route->getScheduledStopsNumber();cptRank++)
							{
								const StopArea* lineStopArea(
									dynamic_cast<const StopPoint*>(&*route->getLineStop(cptRank, true)->get<LineNode>())->getConnectionPlace()
								);
								if (lineStopArea &&
									lineStopArea->getKey() == currentSubscription->get<StopArea>()->getKey())
								{
									rank = cptRank;
									break;
								}
							}
							ptime rtArrivalDate(not_a_date_time);
							int numAnkunftszeitAZBPrognose = AZBFahrplanlageNode.nChildNode("AnkunftszeitAZBPrognose");
							if (numAnkunftszeitAZBPrognose > 0)
							{
								XMLNode nodeRTArrivalTime = AZBFahrplanlageNode.getChildNode("AnkunftszeitAZBPrognose");
								rtArrivalDate = XmlToolkit::GetXsdDateTime(
									nodeRTArrivalTime.getText()) + diff_from_utc; //We are supposed to receive the UTC time
							}
							int numAbfahrtszeitAZBPrognose = AZBFahrplanlageNode.nChildNode("AbfahrtszeitAZBPrognose");
							ptime rtDepartureDate(not_a_date_time);
							if (numAbfahrtszeitAZBPrognose > 0)
							{
								XMLNode nodeRTDepartureTime = AZBFahrplanlageNode.getChildNode("AbfahrtszeitAZBPrognose");
								rtDepartureDate = XmlToolkit::GetXsdDateTime(
									nodeRTDepartureTime.getText()) + diff_from_utc; //We are supposed to receive the UTC time
							}
							time_duration rtDepartureTime = rtDepartureDate.is_not_a_date_time() ? not_a_date_time : rtDepartureDate.time_of_day();
							time_duration rtArrivalTime = rtArrivalDate.is_not_a_date_time()? not_a_date_time : rtArrivalDate.time_of_day();
							if (!rtDepartureTime.is_not_a_date_time())
							{
								// No hysteresis control (TO-DO ?)
								departureSchedules[rank] = rtDepartureTime;
							}
							if (!rtArrivalTime.is_not_a_date_time())
							{
								// No hysteresis control (TO-DO ?)
								arrivalSchedules[rank] = rtArrivalTime;
							}
							
							// Link the service to the RT datasource
							Importable::DataSourceLinks links(service->getDataSourceLinks());
							links.erase(&*(get<DataSource>()));
							links.insert(make_pair(&*(get<DataSource>()), serviceCode));
							service->setDataSourceLinksWithoutRegistration(links);

							// Set the service active
							service->setActive(today);

							// Update RT
							service->setRealTimeSchedules(departureSchedules, arrivalSchedules);
							
							// Save the updated service
							ScheduledServiceTableSync::Save(service, transaction);
							
							continue;
						}
						
						// The service is not found in the theorical data, it has to be created
						Log::GetInstance().warn("R�ception d'un DatenAbrufenAntwort contenant un service non connu (" + vectServiceCode[1] + "), cr�ation du service non cod�e");
						// TO-DO ? : code the creation of the service (why not in a different network to easily detect errors in HAFAS or special events received ?)
					}
				}
				
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
