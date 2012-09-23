
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
#include "ServerModule.h"
#include "Service.h"
#include "ServicePointer.h"
#include "VDVClientSubscription.hpp"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace graph;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace util::XmlToolkit;

	CLASS_DEFINITION(VDVClient, "t099_vdv_clients", 99)
	FIELD_DEFINITION_OF_OBJECT(VDVClient, "vdv_client_id", "vdv_client_ids")

	FIELD_DEFINITION_OF_TYPE(ReplyAddress, "reply_address", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ReplyPort, "reply_port", SQL_TEXT)

	namespace data_exchange
	{
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
					FIELD_DEFAULT_CONSTRUCTOR(ServiceCode),
					FIELD_DEFAULT_CONSTRUCTOR(DataSourcePointer)
			)	)
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
			bool changed(false);

			for(Subscriptions::const_iterator it(_subscriptions.begin()); it != _subscriptions.end(); ++it)
			{
				if(it->second->checkUpdate())
				{
					return true;
				}
			}

			return false;
		}


	
		void VDVClient::sendUpdateSignal() const
		{
			// Local variables
			ptime now(second_clock::local_time());
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;

			stringstream data;
			data <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:DatenBereitAnfrage Sender=\"" << get<ServerControlCentreCode>() << "\" Zst=\"";
			ToXsdDateTime(data, now);
			data << "\" xmlns:vdv453=\"vdv453ger\" />";

			BasicClient client(get<ReplyAddress>(), get<ReplyPort>());
			stringstream out;
			client.post(out, _getURL("datenbereit"), data.str());
		}



		std::string VDVClient::_getURL( const std::string& request ) const
		{
			return "/" + get<ServerControlCentreCode>() + "/" + get<ServiceCode>() + "/" + request + ".xml";
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
}	}
