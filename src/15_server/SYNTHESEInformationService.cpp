
//////////////////////////////////////////////////////////////////////////////////////////
///	SYNTHESEInformationService class implementation.
///	@file SYNTHESEInformationService.cpp
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

#include "SYNTHESEInformationService.hpp"

#include "RequestException.h"
#include "Request.h"
#include "ServerModule.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,server::SYNTHESEInformationService>::FACTORY_KEY = "synthese_information";
	
	namespace server
	{
		const string SYNTHESEInformationService::ATTR_START_TIME = "start_time";
		


		ParametersMap SYNTHESEInformationService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void SYNTHESEInformationService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap SYNTHESEInformationService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			map.insert(ATTR_START_TIME, ServerModule::GetStartingTime());
			return map;
		}
		
		
		
		bool SYNTHESEInformationService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string SYNTHESEInformationService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}