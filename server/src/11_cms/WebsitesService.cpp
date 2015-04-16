
//////////////////////////////////////////////////////////////////////////////////////////
///	WebsitesService class implementation.
///	@file WebsitesService.cpp
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

#include "WebsitesService.hpp"

#include "RequestException.h"
#include "Request.h"
#include "Webpage.h"
#include "Website.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,cms::WebsitesService>::FACTORY_KEY = "websites";
	
	namespace cms
	{
		const string WebsitesService::TAG_WEBSITE = "website";
		


		ParametersMap WebsitesService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void WebsitesService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap WebsitesService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			BOOST_FOREACH(const Website::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<Website>())
			{
				boost::shared_ptr<ParametersMap> sitePM(new ParametersMap);
				it.second->toParametersMap(*sitePM, true);
				map.insert(TAG_WEBSITE, sitePM);
			}
			return map;
		}
		
		
		
		bool WebsitesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebsitesService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
