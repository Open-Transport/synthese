
//////////////////////////////////////////////////////////////////////////////////////////
///	VDVStatusService class implementation.
///	@file VDVStatusService.cpp
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

#include "VDVStatusService.hpp"

#include "Request.h"
#include "RequestException.h"
#include "ServerModule.h"
#include "XmlToolkit.h"

#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace util::XmlToolkit;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVStatusService>::FACTORY_KEY = "VDVStatus";
	
	namespace data_exchange
	{
		const string VDVStatusService::DATA_RESULT = "result";
		


		ParametersMap VDVStatusService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVStatusService::_setFromParametersMap(const ParametersMap& map)
		{
			// TODO search the partner code and reply ok only if the partner is registered
		}

		ParametersMap VDVStatusService::run(
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
			ptime serverStartingTime(ServerModule::GetStartingTime());
			serverStartingTime -= diff_from_utc;
			
			// XML
			stringstream result;
			result << 
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" <<
				"<vdv453:StatusAntwort xmlns:vdv453=\"vdv453ger\">" <<
				"<Status Zst=\"";
			ToXsdDateTime(result, now);
			result <<
				"\" Ergebnis=\"ok\" />" <<
				"<DatenBereit>1</DatenBereit>" <<
				"<StartDienstZst>";
			ToXsdDateTime(result, serverStartingTime);
			result <<
				"</StartDienstZst>" <<
				"</vdv453:StatusAntwort>"
			;
			map.insert(DATA_RESULT, result.str());

			// Output the result (TODO cancel it if the service is called through the CMS)
			stream << result.str();

			// Map return
			return map;
		}
		
		
		
		bool VDVStatusService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VDVStatusService::getOutputMimeType() const
		{
			return "text/xml";
		}
}	}
