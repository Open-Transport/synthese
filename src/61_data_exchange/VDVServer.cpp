
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

	CLASS_DEFINITION(VDVServer, "t097_vdv_servers", 97)
	FIELD_DEFINITION_OF_OBJECT(VDVServer, "vdv_server_id", "vdv_server_ids")

	FIELD_DEFINITION_OF_TYPE(data_exchange::ServerAddress, "address", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(data_exchange::ServerPort, "port", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ServerControlCentreCode, "server_control_centre_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ClientControlCentreCode, "client_control_centre_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ServiceCode, "service_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TracePath, "trace_path", SQL_TEXT)
	
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
					FIELD_DEFAULT_CONSTRUCTOR(ServiceCode),
					FIELD_DEFAULT_CONSTRUCTOR(DataSource),
					FIELD_DEFAULT_CONSTRUCTOR(TracePath)
			)	),
			_startServiceTimeStamp(not_a_date_time),
			_online(false)
		{
		}



		std::string VDVServer::_getURL( const std::string& request ) const
		{
			return "/" + get<ClientControlCentreCode>() + "/" + get<ServiceCode>() + "/" + request + ".xml";
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
				"<Sender=\"" << get<ClientControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(statusAnfrage, now);
			statusAnfrage <<
				"\"/>";

			bool reloadNeeded(!_online);
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
				)	);
				if(startServiceTime != _startServiceTimeStamp)
				{
					reloadNeeded = true;
					_startServiceTimeStamp = startServiceTime;
				}
			}
			catch(...)
			{
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


				// TODO Read the result
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
