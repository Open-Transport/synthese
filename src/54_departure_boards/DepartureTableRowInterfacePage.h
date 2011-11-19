
/** DepartureTableRowInterfacePage class header.
	@file DepartureTableRowInterfacePage.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "InterfacePage.h"
#include "FactorableTemplate.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace departure_boards
	{
		/** Departure table row Interface Page.
			@code departurestablerow @endcode

			Available data :
				- 0 : Rank of the row in the departure table
				- 1 : Number of the page to display for multiple page protocol (eg Lumiplan)
				- 2 : Display track number
				- 3 : Display service number
				- 4 : Number of intermediates stops to display
				- 5 : Display team number
				- 6 : blinks : Departure blinks ?
				- 7 : time : Departure time
				- 8 : service_number : Service number
				- 9 : track : Track name
				- 10 : team : Team number
				- 11 : Rolling stock ID
				- 12 : theoretical departure time
				- 13 : delay duration
				- 14 : URL to the service edition screen (on the same interface if available)

			Object :
				- Must be a ArrivalDepartureRow object
		*/
		class DepartureTableRowInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage, DepartureTableRowInterfacePage>
		{
		public:
			static const std::string DATA_ROW_RANK;
			static const std::string DATA_PAGE_NUMBER;
			static const std::string DATA_DISPLAY_TRACK_NUMBER;
			static const std::string DATA_DISPLAY_SERVICE_NUMBER;
			static const std::string DATA_DISPLAY_TEAM;
			static const std::string DATA_INTERMEDIATE_STOPS_NUMBER;
			static const std::string DATA_BLINKS;
			static const std::string DATA_TIME;
			static const std::string DATA_SERVICE_NUMBER;
			static const std::string DATA_TRACK;
			static const std::string DATA_TEAM;

			DepartureTableRowInterfacePage();



			//////////////////////////////////////////////////////////////////////////
			/// Display of the admin page.
			/// @param stream Stream to write the result on
			/// @param vars Interface variables
			/// @param rowId Rank of the row in the departure table
			/// @param pageNumber Rank of the page if the display type is based on
			///		memory pages
			/// @param displayQuaiNumber true if the number of the quay must be
			///		displayed
			void display(
				std::ostream& stream,
				interfaces::VariablesMap& vars,
				int rowId,
				int pageNumber,
				bool displayQuaiNumber,
				bool displayServiceNumber,
				bool displayTeam,
				int intermediatesStopsToDisplay,
				int blinkingDelay,
				const ArrivalDepartureRow& row,
				const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_DepartureTableRowInterfacePage_H__
