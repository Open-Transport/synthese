
//////////////////////////////////////////////////////////////////////////////////////////
///	NoOpService class implementation.
///	@file NoOpService.cpp
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

#include "NoOpService.hpp"

#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,cms::NoOpService>::FACTORY_KEY = "no_op";
	
	namespace cms
	{
		ParametersMap NoOpService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void NoOpService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap NoOpService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			return map;
		}
		
		
		
		bool NoOpService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string NoOpService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
