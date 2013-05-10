
/** DeparturesTableInterfacePage class header.
	@file DeparturesTableInterfacePage.h

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

#ifndef SYNTHESE_DeparturesTableInterfacePage_H__
#define SYNTHESE_DeparturesTableInterfacePage_H__

#include <string>

#include "InterfacePage.h"
#include "FactorableTemplate.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace departure_boards
	{
		/** Departures table.

			@code departure_boards P1 P2 P3 @endcode

			Available data :
				- 0 : Title of the screen
				- 1 : Wiring code
				- 2 : 1|0 : Service number display
				- 3 : 1|0 : display the track number
				- 4 : number of intermediates stops to display
				- 5 : 1|0 : display the team identifier
				- 6 : text : name of the stop
				- 7 : blinking_delay : blinking delay
				- 8 : 1|0 : Clock display

			Object :
				- result of table generation

			@ingroup m54Pages refPages
		*/
		class DeparturesTableInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage, DeparturesTableInterfacePage>
		{
		public:
			static const std::string DATA_TITLE;
			static const std::string DATA_WIRING_CODE;
			static const std::string DATA_DISPLAY_SERVICE_NUMBER;
			static const std::string DATA_DISPLAY_TRACK_NUMBER;
			static const std::string DATA_DISPLAY_TEAM;
			static const std::string DATA_STOP_NAME;
			static const std::string DATA_INTERMEDIATE_STOPS_NUMBER;
			static const std::string DATA_BLINKING_DELAY;
			static const std::string DATA_DISPLAY_CLOCK;

			DeparturesTableInterfacePage();

			/** Display of the departures table from a departures list result object (standard and preselection methods).

				@param title Title to display in the generated code
				@param wiringCode Wiring code to display in the generated code
				@param displayServiceNumber The service number must be displayed in this departure table
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& vars
				, const std::string& title
				, int wiringCode
				, bool displayServiceNumber
				, bool displayTrackNumber
				, bool displayTeam,
				boost::optional<size_t> intermediatesStopsToDisplay,
				int blinkingDelay,
				bool displayClock
				, const pt::StopArea* place
				, const ArrivalDepartureList& rows
				, const server::Request* request = NULL
			) const;


			/** Display of the departures table from route planning result object (route planning method).

				@param title Title to display in the generated code
				@param wiringCode Wiring code to display in the generated code
				@param displayServiceNumber The service number must be displayed in this departure table
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& vars
				, const std::string& title
				, int wiringCode
				, bool displayServiceNumber
				, bool displayTrackNumber
				, int blinkingDelay,
				bool displayClock
				, const pt::StopArea* place
				, const RoutePlanningList& rows
				, const server::Request* request = NULL
			) const;

		};
	}
}

#endif // SYNTHESE_DeparturesTableInterfacePage_H__
