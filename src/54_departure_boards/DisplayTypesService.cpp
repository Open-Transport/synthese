
//////////////////////////////////////////////////////////////////////////////////////////
///	DisplayTypesService class implementation.
///	@file DisplayTypesService.cpp
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

#include "DisplayTypesService.hpp"

#include "DisplayType.h"
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
	const string FactorableTemplate<Function,departure_boards::DisplayTypesService>::FACTORY_KEY = "display_types";
	
	namespace departure_boards
	{
		const string DisplayTypesService::TAG_DISPLAY_TYPE = "display_type";
		


		ParametersMap DisplayTypesService::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(PARAMETER_PAGE, _page->getFactoryKey());
			return map;
		}



		void DisplayTypesService::_setFromParametersMap(const ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);
		}

		ParametersMap DisplayTypesService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			BOOST_FOREACH(const DisplayType::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<DisplayType>())
			{
				shared_ptr<ParametersMap> displayTypePM;
				it.second->toParametersMap(*displayTypePM);
				map.insert(TAG_DISPLAY_TYPE, displayTypePM);
			}
			return map;
		}
		
		
		
		bool DisplayTypesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string DisplayTypesService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
