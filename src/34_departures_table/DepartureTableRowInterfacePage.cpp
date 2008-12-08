
/** DepartureTableRowInterfacePage class implementation.
	@file DepartureTableRowInterfacePage.cpp

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

#include "01_util/Conversion.h"

#include "DepartureTableRowInterfacePage.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::InterfacePage, departurestable::DepartureTableRowInterfacePage>::FACTORY_KEY("departurestablerow");
	}

	namespace departurestable
	{
		void DepartureTableRowInterfacePage::display(std::ostream& stream
			, VariablesMap& vars
			, int rowId
			, int pageNumber
            , bool displayQuai
			, bool displayServiceNumber
			, bool displayTeam
			, int intermediatesStopsToDisplay
			, const ArrivalDepartureRow& ptd
			, const server::Request* request
		) const {
			ParametersVector parameters;
			parameters.push_back(Conversion::ToString(rowId));
			parameters.push_back(Conversion::ToString(pageNumber));
			parameters.push_back(Conversion::ToString(displayQuai));
			parameters.push_back(Conversion::ToString(displayServiceNumber));
			parameters.push_back(Conversion::ToString(intermediatesStopsToDisplay));
			parameters.push_back(Conversion::ToString(displayTeam));
			
			InterfacePage::display(
				stream
				, parameters
				, vars
				, (const void*) &ptd
				, request);
		}



		DepartureTableRowInterfacePage::DepartureTableRowInterfacePage()
			: Registrable()
		{

		}
	}
}
