
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
		

		FunctionAPI VersionService::getAPI() const
		{
			FunctionAPI api(
						"Admin",
						"Return the version of SYNTHESE as a Parameter Map or a stream",
						"Example:\n"
						"?SERVICE=version&of=json> returns the version in json.\n"
						"You can also get the value in xml or csv");
			api.addParams(Function::PARAMETER_OUTPUT_FORMAT, "One of json, xml, csv", true);
			api.addParams(Function::PARAMETER_OUTPUT_FORMAT_COMPAT, "One of json, xml, csv", true);
			return api;
		}

		ParametersMap VersionService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void VersionService::_setFromParametersMap(const ParametersMap& map)
		{
			setOutputFormatFromMap(map, "");
		}



		ParametersMap VersionService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			map.insert(ATTR_VERSION, ServerModule::VERSION);
			map.insert(ATTR_REVISION, ServerModule::REVISION);
			map.insert(ATTR_BUILD_DATE, ServerModule::BUILD_DATE);
			map.insert(ATTR_BRANCH, ServerModule::BRANCH);
			outputParametersMap(
				map,
				stream,
				"SYNTHESE-VERSION",
				""
			);
			return map;
		}
		
		
		
		bool VersionService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VersionService::getOutputMimeType() const
		{
			return getOutputMimeTypeFromOutputFormat();
		}
}	}
