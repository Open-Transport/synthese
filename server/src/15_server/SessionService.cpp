
//////////////////////////////////////////////////////////////////////////////////////////
///	SessionService class implementation.
///	@file SessionService.cpp
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

#include "SessionService.hpp"

#include "RequestException.h"
#include "Request.h"
#include "Session.h"
#include "SessionException.h"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,server::SessionService>::FACTORY_KEY = "session";
	
	namespace server
	{
		const string SessionService::TAG_SESSION = "session";
		


		ParametersMap SessionService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void SessionService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap SessionService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			if(	request.getSession())
			{
				boost::shared_ptr<ParametersMap> sessionPM(new ParametersMap);
				Session& session(*request.getSession());
				session.toParametersMap(*sessionPM);
				map.insert(TAG_SESSION, sessionPM);
			}

			return map;
		}
		
		
		
		bool SessionService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string SessionService::getOutputMimeType() const
		{
			return getOutputMimeTypeFromOutputFormat();
		}
}	}
