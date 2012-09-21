
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
					FIELD_DEFAULT_CONSTRUCTOR(ControlCentreCode),
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



		VDVClient::UpdatedItems VDVClient::checkUpdate() const
		{
			UpdatedItems updatedItems;

			for(Subscriptions::const_iterator it(_subscriptions.begin()); it != _subscriptions.end(); ++it)
			{
				bool changed(it->second->checkUpdate());
				if(!changed)
				{
					continue;
				}
				updatedItems.push_back(it);
			}

			return updatedItems;
		}

	
		void VDVClient::sendUpdateSignal() const
		{
			ptime now(second_clock::local_time());
			stringstream data;
			data <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:DatenBereitAnfrage Sender=\"synthese\" Zst=\"";
			ToXsdDateTime(data, now);
			data << "\" xmlns:vdv453=\"vdv453ger\" />";

			BasicClient client(get<ReplyAddress>(), get<ReplyPort>());
			stringstream out;
			client.post(out, _getURL("datenbereit"), data.str());
		}



		void VDVClient::sendUpdate(const UpdatedItems& items) const
		{
			ptime now(second_clock::local_time());
			stringstream data;

			data <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:DatenAbrufenAntwort xmlns:vdv453=\"vdv453ger\">" <<
				"<Bestaetigung Zst=\"";
			ToXsdDateTime(data, now);
			data <<
				"\" Ergebnis=\"ok\" Fehlernummer=\"0\">" <<
				"<Fehlertext />" <<
				"</Bestaetigung>";

			BOOST_FOREACH(Subscriptions::const_iterator it, items)
			{
				BOOST_FOREACH(const ArrivalDepartureList::value_type& dep, it->second->getLastResult())
				{
					const ServicePointer& sp(dep.first);
					const CommercialLine& line(
						*static_cast<CommercialLine*>(sp.getService()->getPath()->getPathGroup())
					);
					const JourneyPattern& jp(
						*static_cast<const JourneyPattern*>(sp.getService()->getPath())
					);
					data <<
						"<AZBNachricht AboID=\"" << it->second->getId() << "\">" <<
						"<AZBFahrtLoeschen Zst=\"";
					ToXsdDateTime(data, sp.getDepartureDateTime());
					data <<
						"\">" <<
						"<AZBID>" << it->second->getStopArea()->getACodeBySource(*get<DataSourcePointer>()) << "</AZBID>" <<
						"<FahrtID>" <<
						"<FahrtBezeichner>" << sp.getService()->getServiceNumber() << "</FahrtBezeichner>" <<
						"<Betriebstag>" << to_simple_string(sp.getOriginDateTime().date()) << "</Betriebstag>" << 
						"</FahrtID>" <<
						"<HstSeqZaehler></HstSeqZaehler>" << // ?
						"<LinienID>" << line.getACodeBySource(*get<DataSourcePointer>())  << "</LinienID>" <<
						"<LinienText>" << line.getShortName() << "</LinienText>" <<
						"<RichtungsID>" << jp.getACodeBySource(*get<DataSourcePointer>()) << "</RichtungsID>" <<
						"<RichtungsText>" << jp.getDirection() << "</RichtungsText>" <<
						"<VonRichtungsText></VonRichtungsText>" << //?
						"<AbmeldeID></AbmeldeID>" << //?
						"</AZBFahrtLoeschen>" <<
						"</AZBNachricht>"
					;
			}	}

			BasicClient client(get<ReplyAddress>(), get<ReplyPort>());
			stringstream out;
			client.post(out, _getURL("datenabrufen"), data.str());
		}



		std::string VDVClient::_getURL( const std::string& request ) const
		{
			return "/" + get<ControlCentreCode>() + "/" + get<ServiceCode>() + "/" + request + ".xml";
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
			if(Env::GetOfficialEnv().get<VDVClient>(get<Key>()).get() == this)
			{
				DataExchangeModule::RemoveVDVClient(get<Name>());
			}
		}
}	}
