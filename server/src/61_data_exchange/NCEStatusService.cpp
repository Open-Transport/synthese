
//////////////////////////////////////////////////////////////////////////////////////////
///	NCEStatusService class implementation.
///	@file NCEStatusService.cpp
///	@author hromain
///	@date 2013
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

#include "NCEStatusService.hpp"

#include "IneoNCEConnection.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,data_exchange::NCEStatusService>::FACTORY_KEY = "nce_status";
	
	namespace data_exchange
	{
		const string NCEStatusService::ATTR_STATUS = "status";

		ParametersMap NCEStatusService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void NCEStatusService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap NCEStatusService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			
			boost::shared_ptr<IneoNCEConnection> theConnection(IneoNCEConnection::GetTheConnection());
			if(theConnection)
			{
				string statusStr;
				switch(theConnection->getStatus())
				{
					case IneoNCEConnection::offline:
					statusStr = "offline";
					break;

					case IneoNCEConnection::online:
					statusStr = "online";
					break;

					case IneoNCEConnection::connect:
					statusStr = "connect";
					break;
				}
				map.insert(ATTR_STATUS, statusStr);
			}

			return map;
		}
		
		
		
		bool NCEStatusService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string NCEStatusService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}