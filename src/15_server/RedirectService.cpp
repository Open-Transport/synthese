
//////////////////////////////////////////////////////////////////////////////////////////
///	RedirectService class implementation.
///	@file RedirectService.cpp
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

#include "RedirectService.hpp"

#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,server::RedirectService>::FACTORY_KEY = "redirect";
	


	namespace server
	{
		const string RedirectService::PARAMETER_URL = "url";
		


		ParametersMap RedirectService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_URL, _url);
			return map;
		}



		void RedirectService::_setFromParametersMap(const ParametersMap& map)
		{
			_url = map.getDefault<string>(PARAMETER_URL);
		}



		ParametersMap RedirectService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// The redirection is requested to ServerModule through an exception
			throw Request::RedirectException(_url, true);

			return ParametersMap(); // Never return
		}
		
		
		
		bool RedirectService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string RedirectService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
