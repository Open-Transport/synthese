
/** DeparturesTableRoutePlanningRowInterfacePage class header.
	@file DeparturesTableRoutePlanningRowInterfacePage.h

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

#ifndef SYNTHESE_DeparturesTableRoutePlanningRowInterfacePage_H__
#define SYNTHESE_DeparturesTableRoutePlanningRowInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace departurestable
	{
		/** Departure table route planning row Interface Page.
			@code departurestablerouteplanningrow @endcode

			Display parameters :
				- 0 : Rank of the row in the departure table
				- 1 : With transfer
				- 2 : Display track number
				- 3 : Display service number
				- 4 : Blinks

			First line :
				- 5 : Track number 1
				- 6 : Service number 1
				- 7 : Time 1 : if empty : no solution
				- 8 : Line text 1
				- 9 : Line image 1
				- 10 : Line style 1
				- 27 : Rolling stock id
			Second line :
				- 11 : Track number 2
				- 12 : Service number 2
				- 13: Time 2 : if empty : no 2nd step
				- 14 : Line text 2
				- 15 : Line image 2
				- 16 : Line style 2
				- 28 : Rolling stock id
			Destination :
				- 17 : City name
				- 18 : City is different than display screen place
				- 19 : Place name
				- 20 : Place name (13 chars)
				- 21 : Place name (26 chars)
			Transfer place :
				- 22 : City name
				- 23 : City is different than display screen place
				- 24 : Place name
				- 25 : Place name (13 chars)
				- 26 : Place name (26 chars)

			Object :
				- Must be a graph::Journey object
		*/
		class DeparturesTableRoutePlanningRowInterfacePage :
			public util::FactorableTemplate<interfaces::InterfacePage, DeparturesTableRoutePlanningRowInterfacePage>
		{
		public:
			static const std::string DATA_RANK;
			static const std::string DATA_WITH_TRANSFER;
			static const std::string DATA_DISPLAY_TRACK;
			static const std::string DATA_DISPLAY_SERVICE_NUMBER;
			static const std::string DATA_BLINKS;
			static const std::string DATA_FIRST_TRACK;
			static const std::string DATA_FIRST_SERVICE;
			static const std::string DATA_FIRST_TIME;
			static const std::string DATA_FIRST_LINE_TEXT;
			static const std::string DATA_FIRST_LINE_IMAGE;
			static const std::string DATA_FIRST_LINE_STYLE;
			static const std::string DATA_SECOND_TRACK;
			static const std::string DATA_SECOND_SERVICE;
			static const std::string DATA_SECOND_TIME;
			static const std::string DATA_SECOND_LINE_TEXT;
			static const std::string DATA_SECOND_LINE_IMAGE;
			static const std::string DATA_SECOND_LINE_STYLE;
			static const std::string DATA_DESTINATION_CITY_NAME;
			static const std::string DATA_DESTINATION_CITY_IS_DIFFERENT;
			static const std::string DATA_DESTINATION_PLACE_NAME;
			static const std::string DATA_DESTINATION_PLACE_NAME13;
			static const std::string DATA_DESTINATION_PLACE_NAME26;
			static const std::string DATA_TRANSFER_CITY_NAME;
			static const std::string DATA_TRANSFER_CITY_IS_DIFFERENT;
			static const std::string DATA_TRANSFER_PLACE_NAME;
			static const std::string DATA_TRANSFER_PLACE_NAME13;
			static const std::string DATA_TRANSFER_PLACE_NAME26;

			DeparturesTableRoutePlanningRowInterfacePage();

			/** Display of the admin page.
			*/
			void display( std::ostream& stream
				, interfaces::VariablesMap& vars
				, int rowId
				, bool displayQuaiNumber
				, bool displayServiceNumber,
				int blinkingDelay,
				bool withTransfer,
				const pt::StopArea& origin,
				const RoutePlanningRow& row
				, const server::Request* request = NULL
			) const;

		};
	}
}

#endif // SYNTHESE_DepartureTableRowInterfacePage_H__
