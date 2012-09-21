
//////////////////////////////////////////////////////////////////////////////////////////
///	VDVServerUpdateService class implementation.
///	@file VDVServerUpdateService.cpp
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

#include "VDVServerUpdateService.hpp"

#include "Request.h"
#include "RequestException.h"
#include "ServerConstants.h"
#include "XmlParser.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,data_exchange::VDVServerUpdateService>::FACTORY_KEY = "VDVClientUpdate";
	
	namespace data_exchange
	{
		ParametersMap VDVServerUpdateService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VDVServerUpdateService::_setFromParametersMap(const ParametersMap& map)
		{
			string content(map.getDefault<string>(PARAMETER_POST_DATA));


		}



		ParametersMap VDVServerUpdateService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			
			return map;
		}
		
		
		
		bool VDVServerUpdateService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VDVServerUpdateService::getOutputMimeType() const
		{
			return "text/xml";
		}
}	}
