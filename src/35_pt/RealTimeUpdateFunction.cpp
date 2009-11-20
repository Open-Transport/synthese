
/** RealTimeUpdateFunction class implementation.
	@file RealTimeUpdateFunction.cpp
	@author Hugues
	@date 2009

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "RequestException.h"
#include "Request.h"
#include "TransportNetworkRight.h"
#include "RealTimeUpdateFunction.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace env;

	template<> const string util::FactorableTemplate<Function,pt::RealTimeUpdateFunction>::FACTORY_KEY("rtu");
	
	namespace pt
	{
		/// @todo Parameter names declarations
		//const string RealTimeUpdateFunction::PARAMETER_PAGE("rub");
		
		ParametersMap RealTimeUpdateFunction::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(PARAMETER_PAGE, _page->getFactoryKey());
			return map;
		}

		void RealTimeUpdateFunction::_setFromParametersMap(const ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);
		}

		void RealTimeUpdateFunction::_run( std::ostream& stream ) const
		{
			/// @todo Fill it
		}
		
		
		
		bool RealTimeUpdateFunction::_isAuthorized() const
		{
			return _request->isAuthorized<TransportNetworkRight>(READ);
		}



		std::string RealTimeUpdateFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
