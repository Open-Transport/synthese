
/** JourneyBoardStopCellInterfacePage class header.
	@file JourneyBoardStopCellInterfacePage.h

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

#ifndef SYNTHESE_JourneyBoardStopCellInterfacePage_H__
#define SYNTHESE_JourneyBoardStopCellInterfacePage_H__

#include "InterfacePage.h"
#include "DateTime.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace env
	{
		class PhysicalStop;
	}

	namespace messages
	{
		class SentAlarm;
	}

	namespace routeplanner
	{
		/** Journey board cell for stop at a place.
			@code journey_board_stop_cell @endcode
			@ingroup m53Pages refPages

			Parameters
				- 0 isItArrival : true if the stop is used as an arrival, false else
				- 1 : Alert message
				- 2 : Alert level
				- 3 isItTerminus : true if the stop is the terminus of the used line, false else
				- 4 : Stop name
				- 5 Odd or even color
				- 6 Fist time
				- 7 Last time (empty if continuous service)
				- 8 WGS84 longitude
				- 9 WGS84 latitude
		*/
		class JourneyBoardStopCellInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage,JourneyBoardStopCellInterfacePage>
		{
		public:
			JourneyBoardStopCellInterfacePage();

			/** Display.
				@param stream Stream to write on
				@param isItArrival (0) true if the stop is used as an arrival, false else
				@param alarm (1/2) Alert (1=message, 2=level)
				@param isItTerminus (3) true if the stop is the terminus of the used line, false else
				@param place Place to display
				@param color (5) Odd or even color
				@param time Time
				@param continuousServiceRange Continuous service range (if <= 0 then no continuous service displayed)
				@param site Displayed site
			*/
			void display(
				std::ostream& stream
				, bool isItArrival
				, const messages::SentAlarm* alarm,
				bool isItTerminus,
				const env::PhysicalStop& physicalStop,
				bool color
				, const time::DateTime& time
				, int continuousServiceRange
				, const server::Request* request = NULL
			) const;
		};
	}
}
#endif // SYNTHESE_JourneyBoardStopCellInterfacePage_H__
