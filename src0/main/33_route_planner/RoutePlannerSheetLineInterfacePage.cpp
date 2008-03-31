
/** RoutePlannerSheetLineInterfacePage class implementation.
	@file RoutePlannerSheetLineInterfacePage.cpp

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

#include "33_route_planner/RoutePlannerSheetLineInterfacePage.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	template<> const std::string util::FactorableTemplate<InterfacePage,routeplanner::RoutePlannerSheetLineInterfacePage>::FACTORY_KEY("schedule_sheet_row");

	
	namespace routeplanner
	{
		void RoutePlannerSheetLineInterfacePage::display( 
		    std::ostream& stream, 
		    const std::string& text, 
		    bool alternateColor
			, bool isOrigin
			, bool isDestination
			, VariablesMap& variables,
		    const env::ConnectionPlace* place, 
		    const server::Request* request /*= NULL */
		) const {
			ParametersVector pv;
			pv.push_back(text);
			pv.push_back(Conversion::ToString( alternateColor ));
			pv.push_back(Conversion::ToString(isOrigin));
			pv.push_back(Conversion::ToString(isDestination));

			InterfacePage::display( stream, pv, variables, place, request );
		}
	}
}
