
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
#include "VDVServerSubscription.hpp"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/thread.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
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
	FIELD_DEFINITION_OF_TYPE(DataSourcePointer, "data_source_id", SQL_INTEGER)
	
	namespace data_exchange
	{
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
					FIELD_DEFAULT_CONSTRUCTOR(DataSourcePointer)
			)	),
			_online(false),
			_startServiceTimeStamp(not_a_date_time)
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
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;
			_online = false;
			
			BasicClient c(
				get<ServerAddress>(),
				get<ServerPort>()
			);

			stringstream statusAnfrage;
			statusAnfrage <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:StatusAnfrage Sender=\"" << get<ClientControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(statusAnfrage, now);
			statusAnfrage <<
				"\" xmlns:vdv453=\"vdv453ger\" />";

			stringstream out;
			c.post(
				out,
				_getURL("status"),
				statusAnfrage.str()
			);

			string statusAntwortStr(out.str());
			XMLResults results;
			XMLNode allNode = XMLNode::parseString(statusAntwortStr.c_str(), "vdv453:StatusAntwort", &results);
			if (results.error != eXMLErrorNone)
			{
				_online = false;
				return;
			}
			
			XMLNode statusNode = allNode.getChildNode("Status");
			string ergebinsAttr(statusNode.getAttribute("Ergebnis"));
			if(ergebinsAttr != "ok")
			{
				_online = false;
				return;
			}

			// TODO Check the StartDienstZst attribute

			if(_online) // TODO Check if the subscriptions have changed
			{
				return;
			}

			this_thread::sleep(seconds(1));

			// Clean subscriptions
			now = second_clock::local_time() - diff_from_utc;
			stringstream cleanRequest;
			cleanRequest <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:AboAnfrage Sender=\"" << get<ClientControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(cleanRequest, now);
			cleanRequest <<
				"\" xmlns:vdv453=\"vdv453ger\">" <<
				"<AboLoeschenAlle>true</AboLoeschenAlle>" <<
				"</vdv453:AboAnfrage>"
			;

			this_thread::sleep(seconds(1));

			// Send subscription request
			now = second_clock::local_time() - diff_from_utc;

			stringstream aboAnfrage;
			aboAnfrage <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:AboAnfrage Sender=\"" << get<ClientControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(aboAnfrage, now);
			aboAnfrage << "\" xmlns:vdv453=\"vdv453ger\">";

			BOOST_FOREACH(VDVServerSubscription* subscription, _subscriptions)
			{
				if(!subscription->get<StopAreaPointer>())
				{
					continue;
				}

				ptime expirationTime(now + subscription->get<SubscriptionDuration>());
				aboAnfrage << 
					"<AboAZB AboID=\"" << subscription->get<Name>() << " VerfallZst=\"";
				ToXsdDateTime(aboAnfrage, expirationTime);
				aboAnfrage <<
					"\">" <<
					"<AZBID>" << (
						get<DataSourcePointer>() ?
						subscription->get<StopAreaPointer>()->getACodeBySource(*get<DataSourcePointer>()) :
						lexical_cast<string>(subscription->get<StopAreaPointer>()->getKey())
					) << "</AZBID>" <<
					//"<LinienID>" << "</LinienID>" <<
					//"<RichtungsID>ZVV105B</RichtungsID>
					"<Vorschauzeit>" << (subscription->get<TimeSpan>().total_seconds() / 60)  << "</Vorschauzeit>" <<
					"<Hysterese>60</Hysterese>" <<
					"</AboAZB>"
				;
			}

			aboAnfrage << "</vdv453:AboAnfrage>";

			stringstream aboAntwort;
			c.post(
				aboAntwort,
				_getURL("aboverwalten"),
				aboAnfrage.str()
			);

			string aboAntwortStr(aboAntwort.str());
			XMLResults aboAntwortResults;
			XMLNode aboAntwortNode = XMLNode::parseString(statusAntwortStr.c_str(), "vdv453:AboAntwort", &aboAntwortResults);
			if (aboAntwortResults.error != eXMLErrorNone ||
				aboAntwortNode.isEmpty()
			){
				_online = false;
				return;
			}
			XMLNode bestaetingungNode = aboAntwortNode.getChildNode("Bestaetigung");
			ergebinsAttr = bestaetingungNode.getAttribute("Ergebnis");
			if(ergebinsAttr != "ok")
			{
				_online = false;
				return;
			}

			_online = true;
		}



		void VDVServer::updateSYNTHESEFromServer() const
		{
			this_thread::sleep(boost::posix_time::seconds(5));

			// Local variables
			ptime now(second_clock::local_time());
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;

			// The request
			stringstream request;
			request <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:DatenAbrufenAnfrage xmlns:vdv453=\"vdv453ger\" Sender=\"" <<
				get<ClientControlCentreCode>() <<
				"\" Zst=\"";
			ToXsdDateTime(request, now);
			request <<
				"<DatensatzAlle>0</DatensatzAlle>"
				"</vdv453:DatenAbrufenAnfrage>"
			;

			// Sending the request
			stringstream result;
			BasicClient c(
				get<ServerAddress>(),
				get<ServerPort>()
			);
			c.post(
				result,
				_getURL("datenabrufen"),
				request.str()
			);

			// TODO Read the result

		}
}	}
