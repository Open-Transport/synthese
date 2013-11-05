
//////////////////////////////////////////////////////////////////////////////////////////
///	VersionService class implementation.
///	@file VersionService.cpp
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

#include "VersionService.hpp"

#include "Request.h"
#include "ServerModule.h"

#include <boost/algorithm/string.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,server::VersionService>::FACTORY_KEY = "version";
	
	namespace server
	{
		const string VersionService::ATTR_VERSION = "version";
		const string VersionService::ATTR_REVISION = "revision";
		const string VersionService::ATTR_BUILD_DATE = "build_date";
		const string VersionService::ATTR_BRANCH = "branch";
		


		ParametersMap VersionService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VersionService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap VersionService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			map.insert(ATTR_VERSION, ServerModule::VERSION);
			map.insert(ATTR_REVISION, ServerModule::REVISION);
			map.insert(ATTR_BUILD_DATE, ServerModule::BUILD_DATE);
			std::vector<std::string> urlVector;
			boost::algorithm::split(urlVector, ServerModule::SYNTHESE_URL, boost::is_any_of("/"));
			if (urlVector.size() > 0)
			{
				if (urlVector.at(urlVector.size()-1) == "trunk")
					map.insert(ATTR_BRANCH, urlVector.at(urlVector.size()-1));
				else if (urlVector.size() > 1)
					map.insert(ATTR_BRANCH, urlVector.at(urlVector.size()-2) + "/" + urlVector.at(urlVector.size()-1));
			}
			else
				map.insert(ATTR_BRANCH, "");
			return map;
		}
		
		
		
		bool VersionService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VersionService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
