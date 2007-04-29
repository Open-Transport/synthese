
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


#include "11_interfaces/InterfacePage.h"
#include "04_time/Hour.h"

namespace synthese
{
	namespace messages
	{
		class SentAlarm;
	}

	namespace routeplanner
	{
		/** Journey board cell for stop at a place.
			@code journey_board_stop_cell @endcode
			@ingroup m33Pages refPages
		*/
		class JourneyBoardStopCellInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Display.
				@param stream Stream to write on
				@param isItArrival (0) true if the stop is used as an arrival, false else
				@param alarm (1/2) Alert (1=message, 2=level)
				@param isItTerminus (3) true if the stop is the terminus of the used line, false else
				@param stopName (4) Stop name
				@param color (5) Odd or even color
				@param firstTime (6) Fist time
				@param lastTime (7) Last time (UNKNOWN if continuous service)
				@param site Displayed site
			*/
			void display(
				std::ostream& stream
				, bool isItArrival
				, const messages::SentAlarm* alarm
				, bool isItTerminus, const std::string& stopName, bool color
				, const time::Hour& firstTime, const time::Hour& lastTime
				, const server::Request* request = NULL ) const;

		};
	}
}
#endif // SYNTHESE_JourneyBoardStopCellInterfacePage_H__

