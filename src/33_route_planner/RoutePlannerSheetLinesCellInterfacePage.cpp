
/** RoutePlannerSheetLinesCellInterfacePage class implementation.
	@file RoutePlannerSheetLinesCellInterfacePage.cpp

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

#include "33_route_planner/RoutePlannerSheetLinesCellInterfacePage.h"

#include "30_server/Request.h"


namespace synthese
{
	using namespace interfaces;

	template<> const std::string util::FactorableTemplate<InterfacePage,routeplanner::RoutePlannerSheetLinesCellInterfacePage>::FACTORY_KEY("schedule_sheet_lines_cell");

	namespace routeplanner
	{
		void RoutePlannerSheetLinesCellInterfacePage::display( 
		    std::ostream& stream, 
		    size_t columnNumber, 
			VariablesMap& variables,
		    const env::Journey* object, 
		    const server::Request* request /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back( synthese::util::Conversion::ToString( columnNumber ) );
			
			InterfacePage::display( stream, pv, variables, (const void*) object, request );
		}



		RoutePlannerSheetLinesCellInterfacePage::RoutePlannerSheetLinesCellInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
