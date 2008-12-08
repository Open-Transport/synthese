
/** RoutePlannerSheetColumnInterfacePage class implementation.
	@file RoutePlannerSheetColumnInterfacePage.cpp

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

#include "33_route_planner/RoutePlannerSheetColumnInterfacePage.h"

#include "30_server/Request.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace time;
	using namespace util;

	template<> const string util::FactorableTemplate<InterfacePage,routeplanner::RoutePlannerSheetColumnInterfacePage>::FACTORY_KEY("schedule_sheet_column");

	namespace routeplanner
	{
		void RoutePlannerSheetColumnInterfacePage::display(
			ostream& stream
			, bool isItFirstLine
			, bool isItLastLine
			, size_t columnNumber
			, bool isItFootLine 
			, const Hour& firstArrivalTime
			, const Hour& lastArrivalTime 
			, bool isItContinuousService
			, bool isFirstWriting
			, bool isLastWriting
			, bool isFirstFoot
			, const server::Request* request /*= NULL */
		) const	{
			ParametersVector pv;
			pv.push_back( Conversion::ToString( isItFirstLine ) );
			pv.push_back( Conversion::ToString( isItLastLine ) );
			pv.push_back( Conversion::ToString( columnNumber ) );
			pv.push_back( Conversion::ToString( isItFootLine ) );
			pv.push_back( firstArrivalTime.isUnknown() ? string() : firstArrivalTime.toString());
			pv.push_back( firstArrivalTime.isUnknown() ? string() : lastArrivalTime.toString() );
			pv.push_back( Conversion::ToString( isItContinuousService ) );
			pv.push_back( Conversion::ToString( isFirstWriting ) );
			pv.push_back( Conversion::ToString( isLastWriting ) );
			pv.push_back( Conversion::ToString( isFirstFoot ) );

			VariablesMap vm;
			InterfacePage::display( stream, pv, vm, NULL, request );
			
		}



		RoutePlannerSheetColumnInterfacePage::RoutePlannerSheetColumnInterfacePage()
			: Registrable()
		{

		}
	}
}
