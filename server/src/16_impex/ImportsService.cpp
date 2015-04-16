
//////////////////////////////////////////////////////////////////////////////////////////
///	ImportsService class implementation.
///	@file ImportsService.cpp
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

#include "ImportsService.hpp"

#include "Import.hpp"
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
	const string FactorableTemplate<Function,impex::ImportsService>::FACTORY_KEY = "imports";
	
	namespace impex
	{
		const string ImportsService::TAG_IMPORT = "import";
		


		ParametersMap ImportsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void ImportsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Import filter
			RegistryKeyType importId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(importId) try
			{
				_importFilter = Env::GetOfficialEnv().get<Import>(
					importId
				);
			}
			catch(ObjectNotFoundException<Import>&)
			{
				throw RequestException("No such import");
			}
		}



		ParametersMap ImportsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;

			if(_importFilter.get())
			{
				boost::shared_ptr<ParametersMap> importPM(new ParametersMap);
				_importFilter->toParametersMap(*importPM, true);
				map.insert(TAG_IMPORT, importPM);
			}
			else
			{
				BOOST_FOREACH(const Registry<Import>::value_type& it, Env::GetOfficialEnv().getRegistry<Import>())
				{
					boost::shared_ptr<ParametersMap> importPM(new ParametersMap);
					it.second->toParametersMap(*importPM, true);
					map.insert(TAG_IMPORT, importPM);
				}
			}
		
			return map;
		}
		
		
		
		bool ImportsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ImportsService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
