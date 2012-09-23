
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
#include "JourneyPattern.hpp"
#include "Path.h"
#include "Request.h"
#include "RequestException.h"
#include "ServerConstants.h"
#include "Service.h"
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
		}

		ParametersMap VDVDataSupplyService::run(
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
			
			// XML
			stringstream result;
			result <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:DatenAbrufenAntwort xmlns:vdv453=\"vdv453ger\">" <<
				"<Bestaetigung Zst=\"";
			ToXsdDateTime(result, now);
			result <<
				"\" Ergebnis=\"" <<
				((!_errorNumber.empty() && _errorNumber != "0") ?
				"notok" : "ok") <<
				"\" Fehlernummer=\"" <<
				(_errorNumber.empty() ? "0" : _errorNumber) <<
				"\">"
			;
			if(!_errorText.empty())
			{
				result << "<Fehlertext>" << _errorText << "</FehlerText>";
			}
			result << "</Bestaetigung>";

			if(_vdvClient)
			{
				BOOST_FOREACH(const VDVClient::Subscriptions::value_type& it, _vdvClient->getSubscriptions())
				{
					BOOST_FOREACH(const ArrivalDepartureList::value_type& dep, it.second->getLastResult())
					{
						// Local variables
						const ServicePointer& sp(dep.first);
						const CommercialLine& line(
							*static_cast<CommercialLine*>(sp.getService()->getPath()->getPathGroup())
						);
						const JourneyPattern& jp(
							*static_cast<const JourneyPattern*>(sp.getService()->getPath())
						);
						ptime departureDateTime(sp.getDepartureDateTime());
						departureDateTime -= diff_from_utc;

						// XML generation
						result <<
							"<AZBNachricht AboID=\"" << it.second->getId() << "\">" <<
							"<AZBFahrtLoeschen Zst=\"";
						ToXsdDateTime(result, departureDateTime);
						result <<
							"\">" <<
							"<AZBID>" << it.second->getStopArea()->getACodeBySource(*_vdvClient->get<DataSourcePointer>()) << "</AZBID>" <<
							"<FahrtID>" <<
							"<FahrtBezeichner>" << sp.getService()->getServiceNumber() << "</FahrtBezeichner>" <<
							"<Betriebstag>" << to_simple_string(sp.getOriginDateTime().date()) << "</Betriebstag>" << 
							"</FahrtID>" <<
							"<HstSeqZaehler></HstSeqZaehler>" << // ?
							"<LinienID>" << line.getACodeBySource(*_vdvClient->get<DataSourcePointer>())  << "</LinienID>" <<
							"<LinienText>" << line.getShortName() << "</LinienText>" <<
							"<RichtungsID>" << jp.getACodeBySource(*_vdvClient->get<DataSourcePointer>()) << "</RichtungsID>" <<
							"<RichtungsText>" << jp.getDirection() << "</RichtungsText>" <<
							"<VonRichtungsText></VonRichtungsText>" << //?
							"<AbmeldeID></AbmeldeID>" << //?
							"</AZBFahrtLoeschen>" <<
							"</AZBNachricht>"
						;
			}	}	}

			// Output the result (TODO cancel it if the service is called through the CMS)
			map.insert(DATA_RESULT, result.str());
			stream << result.str();

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
