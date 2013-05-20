
//////////////////////////////////////////////////////////////////////////////////////////
///	VDVDataReadyService class implementation.
///	@file VDVDataReadyService.cpp
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

#include "VDVDataReadyService.hpp"

#include "DataExchangeModule.hpp"
#include "Request.h"
#include "RequestException.h"
#include "ServerModule.h"
#include "ServerConstants.h"
#include "VDVServer.hpp"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace util::XmlToolkit;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVDataReadyService>::FACTORY_KEY = "VDVDataReady";
	
	namespace data_exchange
	{
		const string VDVDataReadyService::DATA_RESULT = "result";

		ParametersMap VDVDataReadyService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVDataReadyService::_setFromParametersMap(const ParametersMap& map)
		{
			string content(map.getDefault<string>(PARAMETER_POST_DATA));

			XMLResults results;
			XMLNode allNode = XMLNode::parseString(content.c_str(), "DatenBereitAnfrage", &results);
			if (results.error != eXMLErrorNone)
			{
				return;
			}

			if(allNode.isEmpty())
			{
				return;
			}

			string sender(allNode.getAttribute("Sender"));
			_vdvServer = &DataExchangeModule::GetVDVServer(sender);

			// Trace
			_vdvServer->trace("DatenBereitAnfrage", content);
		}



		ParametersMap VDVDataReadyService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			

			// Local variables
			ptime now(second_clock::local_time());
			typedef boost::date_time::c_local_adjustor<ptime> local_adj;
			time_duration diff_from_utc(local_adj::utc_to_local(now) - now);
			now -= diff_from_utc;

			stringstream result;
			result <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<DatenBereitAntwort>" <<
				"<Bestaetigung Zst=\"";
			ToXsdDateTime(result, now);
			result <<
				"\" Ergebnis=\"ok\" Fehlernummer=\"0\">" <<
				"<Fehlertext />" <<
				"</Bestaetigung>" <<
				"</DatenBereitAntwort>"
			;

			// Output the result (TODO cancel it if the service is called through the CMS)
			map.insert(DATA_RESULT, result.str());
			stream << result.str();

			// Trace
			_vdvServer->trace("DatenBereitAntwort", result.str());

			// Run the update in a separate thread
			ServerModule::AddThread(boost::bind(&VDVServer::updateSYNTHESEFromServer, _vdvServer), "VDV servers connector");

			// Map return
			return map;
		}
		
		
		
		bool VDVDataReadyService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VDVDataReadyService::getOutputMimeType() const
		{
			return "text/xml";
		}
}	}
