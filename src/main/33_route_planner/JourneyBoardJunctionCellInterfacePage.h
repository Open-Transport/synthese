
/** JourneyBoardJunctionCellInterfacePage class header.
	@file JourneyBoardJunctionCellInterfacePage.h

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

#ifndef SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__
#define SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
		class Road;
	}

	namespace messages
	{
		class SentAlarm;
	}

	namespace routeplanner
	{
		/** Journey board cell for use of a junction.
			@code journey_board_junction_cell @endcode
			@ingroup m53Pages refPages

			Parameters :
				- 0 : Reached place name
				- 1 : Alarm
				- 2 : Alarm
				- 3 : Odd or even row in the journey board
				- 4 : Road name
		*/
		class JourneyBoardJunctionCellInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage,JourneyBoardJunctionCellInterfacePage>
		{
		public:
			/** Display.
				@param stream Stream to display on
				@param place Reached place
				@param alarm Alarm to display for the road use
				@param color Odd or even row in the journey board
				@param road Used road
				@param request Source request
			*/
			void display(
				std::ostream& stream
				, const env::ConnectionPlace* place
				, const messages::SentAlarm* alarm
				, bool color
				, const env::Road* road
				, const server::Request* request = NULL
			) const;

		};
	}
}

#endif // SYNTHESE_JourneyBoardJunctionCellInterfacePage_H__
