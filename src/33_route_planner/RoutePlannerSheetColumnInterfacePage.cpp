
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

#include "RoutePlannerSheetColumnInterfacePage.h"
#include "Request.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	template<> const string util::FactorableTemplate<InterfacePage,routeplanner::RoutePlannerSheetColumnInterfacePage>::FACTORY_KEY("schedule_sheet_column");

	namespace routeplanner
	{
		const string RoutePlannerSheetColumnInterfacePage::DATA_IS_FIRST_ROW("is_first_row");
		const string RoutePlannerSheetColumnInterfacePage::DATA_IS_LAST_ROW("is_last_row");
		const string RoutePlannerSheetColumnInterfacePage::DATA_COLUMN_NUMBER("column_number");
		const string RoutePlannerSheetColumnInterfacePage::DATA_IS_FOOT("is_foot");
		const string RoutePlannerSheetColumnInterfacePage::DATA_FIRST_TIME("first_time");
		const string RoutePlannerSheetColumnInterfacePage::DATA_LAST_TIME("last_time");
		const string RoutePlannerSheetColumnInterfacePage::DATA_IS_CONTINUOUS_SERVICE("is_continuous_service");
		const string RoutePlannerSheetColumnInterfacePage::DATA_IS_FIRST_WRITING("is_first_writing");
		const string RoutePlannerSheetColumnInterfacePage::DATA_IS_LAST_WRITING("is_last_writing");
		const string RoutePlannerSheetColumnInterfacePage::DATA_IS_FIRST_FOOT("is_first_foot");

		void RoutePlannerSheetColumnInterfacePage::display(
			ostream& stream
			, bool isItFirstLine
			, bool isItLastLine
			, size_t columnNumber
			, bool isItFootLine 
			, const time_duration& firstArrivalTime
			, const time_duration& lastArrivalTime 
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
			{
				stringstream s;
				if(!firstArrivalTime.is_not_a_date_time())
				{
					s << setfill('0') << setw(2) << firstArrivalTime.hours() << ":" << setfill('0') << setw(2) << firstArrivalTime.minutes();
				}
				pv.push_back(s.str());
			}
			{
				stringstream s;
				if(!lastArrivalTime.is_not_a_date_time())
				{
					s << setfill('0') << setw(2) << lastArrivalTime.hours() << ":" << setfill('0') << setw(2) << lastArrivalTime.minutes();
				}
				pv.push_back(s.str());
			}
			pv.push_back( Conversion::ToString( isItContinuousService ) );
			pv.push_back( Conversion::ToString( isFirstWriting ) );
			pv.push_back( Conversion::ToString( isLastWriting ) );
			pv.push_back( Conversion::ToString( isFirstFoot ) );

			VariablesMap vm;
			InterfacePage::_display( stream, pv, vm, NULL, request );
			
		}



		RoutePlannerSheetColumnInterfacePage::RoutePlannerSheetColumnInterfacePage()
			: Registrable(0)
		{

		}
	}
}
