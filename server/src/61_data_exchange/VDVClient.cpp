
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

#include "VDVClient.hpp"

#include "BasicClient.h"
#include "CommercialLine.h"
#include "DataExchangeModule.hpp"
#include "JourneyPattern.hpp"
#include "Profile.h"
#include "Session.h"
#include "ServerModule.h"
#include "Service.h"
#include "ServicePointer.h"
#include "User.h"
#include "VDVClientSubscription.hpp"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <fstream>

using namespace boost;
using namespace boost::filesystem;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace util::XmlToolkit;

	CLASS_DEFINITION(VDVClient, "t099_vdv_clients", 99)
	FIELD_DEFINITION_OF_OBJECT(VDVClient, "vdv_client_id", "vdv_client_ids")

	FIELD_DEFINITION_OF_TYPE(ReplyAddress, "reply_address", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ReplyPort, "reply_port", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TransportNetworks, "transport_network_ids", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DefaultDirection, "default_direction", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SBBMode, "sbb_mode", SQL_BOOLEAN)
	
	namespace data_exchange
	{
		const string VDVClient::TAG_SUBSCRIPTION = "subscription";
		const string VDVClient::TAG_TRANSPORT_NETWORK = "transport_network";



		VDVClient::VDVClient(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<VDVClient, VDVClientRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(ReplyAddress),
					FIELD_DEFAULT_CONSTRUCTOR(ReplyPort),
					FIELD_DEFAULT_CONSTRUCTOR(ClientControlCentreCode),
					FIELD_VALUE_CONSTRUCTOR(ServerControlCentreCode, "synthese"),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceUrl),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceCode),
					FIELD_DEFAULT_CONSTRUCTOR(DataSource),
					FIELD_DEFAULT_CONSTRUCTOR(TransportNetworks),
					FIELD_DEFAULT_CONSTRUCTOR(DefaultDirection),
					FIELD_VALUE_CONSTRUCTOR(Active, true),
					FIELD_DEFAULT_CONSTRUCTOR(TracePath),
					FIELD_VALUE_CONSTRUCTOR(SBBMode, false)
			)	),
			_lastDataReady(not_a_date_time)
		{
		}



		void VDVClient::addSubscription(
			boost::shared_ptr<VDVClientSubscription> subscription
		){
			if(!subscription.get())
			{
				return;
			}

			_subscriptions.insert(
				make_pair(
					subscription->getId(),
					subscription
			)	);
		}



		void VDVClient::removeSubscription(
			const std::string& key )
		{
			_subscriptions.erase(key);
		}



		bool VDVClient::checkUpdate() const
		{
			bool result(false);
			for(Subscriptions::const_iterator it(_subscriptions.begin()); it != _subscriptions.end(); ++it)
			{
				if(it->second->checkUpdate())
				{
					result = true;
				}
			}

			return result;
		}


	
		void VDVClient::sendUpdateSignal() const
		{
			// Local variables
			ptime now(second_clock::local_time());
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;
			const string contentType = "text/xml";

			stringstream data;
			data <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<DatenBereitAnfrage Sender=\"" << get<ServerControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(data, now);
			data << "\"></DatenBereitAnfrage>";

			BasicClient client(get<ReplyAddress>(), get<ReplyPort>());
			string out;
			try
			{
				out = client.post(_getURL("datenbereit"), data.str(), contentType);
			}
			catch(...)
			{
				Log::GetInstance().warn("Error while sending DatenBereitAnfrage");
			}

			// Trace
			trace("DatenBereitAnfrage", data.str());
			trace("DatenBereitAntwort", out);
		}



		std::string VDVClient::_getURL( const std::string& request ) const
		{
			return "/" + get<ServiceUrl>() + "/" + get<ServerControlCentreCode>() + "/" + get<ServiceCode>() + "/" + request + ".xml";
		}



		void VDVClient::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(&env == &Env::GetOfficialEnv())
			{
				DataExchangeModule::AddVDVClient(*this);
			}
		}



		void VDVClient::unlink()
		{
			if(Env::GetOfficialEnv().contains(*this))
			{
				DataExchangeModule::RemoveVDVClient(get<Name>());
			}
		}



		void VDVClient::cleanSubscriptions()
		{
			_subscriptions.clear();
		}



		void VDVClient::setLastDataReadyNow() const
		{
			ptime now(second_clock::local_time());
			_lastDataReady = now;
		}

		
		
		void VDVClient::clearLastDataReady() const
		{
			_lastDataReady = ptime(not_a_date_time);
		}



		void VDVClient::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			// Subscriptions
			BOOST_FOREACH(const Subscriptions::value_type& subscription, _subscriptions)
			{
				boost::shared_ptr<ParametersMap> subscriptionMap(new ParametersMap);
				subscription.second->toParametersMap(*subscriptionMap);
				map.insert(prefix + TAG_SUBSCRIPTION, subscriptionMap);
			}
			
			// Networks
			BOOST_FOREACH(const TransportNetworks::Type::value_type& transportNetwork, get<TransportNetworks>())
			{
				boost::shared_ptr<ParametersMap> transportNetworkPM(new ParametersMap);
				transportNetwork->toParametersMap(*transportNetworkPM, true);
				map.insert(prefix + TAG_TRANSPORT_NETWORK, transportNetworkPM);
			}

		}



		string VDVClient::getDirectionID(
			const JourneyPattern& jp
		) const {

			if( get<SBBMode>()
			){
				if(	get<DataSource>() &&
					jp.hasLinkWithSource(*get<DataSource>()) &&
					!jp.getACodeBySource(*get<DataSource>()).empty())
				{
					// The journey has a link with the Datasource so we use its code
					return jp.getACodeBySource(*get<DataSource>());
				}

				if(	jp.getCommercialLine()->hasLinkWithSource(*get<DataSource>())
				){
					stringstream result;
					result << jp.getCommercialLine()->getACodeBySource(*get<DataSource>());
					result << (jp.getWayBack() ? "B" : "A");
					return result.str();
				}
				else
				{
					return get<DefaultDirection>();
				}
			}
			else
			{
				if(	get<DataSource>() &&
					jp.hasLinkWithSource(*get<DataSource>())
				){
					return jp.getACodeBySource(*get<DataSource>());
				}
				else
				{
					return get<DefaultDirection>();
				}
			}
		}



		void VDVClient::trace(
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
					setw(2) << setfill('0') << now.time_of_day().seconds() << "-" <<
					setw(2) << setfill('0') << now.time_of_day().total_milliseconds() <<
					"_" << tag << ".xml"
				;
				path p(get<TracePath>());
				p = p / dateDirName.str() / "clients" / get<Name>();
				create_directories(p);
				p = p / fileName.str();
				ofstream logFile(p.string().c_str());
				logFile << content;
				logFile.close();
			}
		}


		bool VDVClient::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool VDVClient::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool VDVClient::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
