
/** DepartureTableRowInterfacePage class header.
	@file DepartureTableRowInterfacePage.h

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

#ifndef SYNTHESE_DepartureTableRowInterfacePage_H__
#define SYNTHESE_DepartureTableRowInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

#include "34_departures_table/Types.h"

namespace synthese
{
	namespace departurestable
	{
		/** Departure table row Interface Page.
			@code departurestablerow <row_id> <page_number> <message> <display_service_number> @endcode

			Parameters :
				- 0 <row_id> : Rank of the row in the departure table
				- 1 <page_number> : Number of the page to display for multiple page protocol (eg Lumiplan)
				- 2 <message> : Message to display instead of timetables
				- 3 <display_service_number> : 1|0 Indicates if the service number cell must be displayed in the row
				- 4 : Number of intermediates stops to display

			Object :
				- Must be a ArrivalDepartureRow object
		*/
		class DepartureTableRowInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage, DepartureTableRowInterfacePage>
		{
		public:
			/** Display of the admin page.
			*/
			void display( std::ostream& stream
				, interfaces::VariablesMap& vars
				, int rowId
				, int pageNumber
				, bool displayQuaiNumber
				, bool displayServiceNumber
				, int intermediatesStopsToDisplay
				, const ArrivalDepartureRow& row
				, const server::Request* request = NULL
				) const;

		};
	}
}

#endif // SYNTHESE_DepartureTableRowInterfacePage_H__
