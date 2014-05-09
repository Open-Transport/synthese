
//////////////////////////////////////////////////////////////////////////////////////////
///	ExportsService class implementation.
///	@file ExportsService.cpp
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

#include "ExportsService.hpp"

#include "Export.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,impex::ExportsService>::FACTORY_KEY = "exports";
	
	namespace impex
	{
		const string ExportsService::TAG_EXPORT = "export";
		


		ParametersMap ExportsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void ExportsService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap ExportsService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			const Export::Registry& registry(Env::GetOfficialEnv().getRegistry<Export>());
			recursive_mutex::scoped_lock lock(registry.getMutex());
			BOOST_FOREACH(const Export::Registry::value_type& it, registry)
			{
				boost::shared_ptr<ParametersMap> exportPM(new ParametersMap);
				it.second->toParametersMap(*exportPM, true);
				map.insert(TAG_EXPORT, exportPM);
			}

			return map;
		}
		
		
		
		bool ExportsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ExportsService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}