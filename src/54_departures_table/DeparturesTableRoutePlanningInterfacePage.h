
/** DeparturesTableRoutePlanningInterfacePage class header.
	@file DeparturesTableRoutePlanningInterfacePage.h

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

#ifndef SYNTHESE_DeparturesTableRoutePlanningInterfacePage_H__
#define SYNTHESE_DeparturesTableRoutePlanningInterfacePage_H__

#include <string>

#include "InterfacePage.h"
#include "FactorableTemplate.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace departurestable
	{
		/** Departures table.
			
			@code departurestable P1 P2 P3 @endcode
			
			Available data :
				- 0 : Title of the screen
				- 1 : Wiring code
				- 2 : 1|0 : Service number display
				- 3 : display the track number
				- 4 : with transfer
				- 5 : id of the origin place
				- 6 : name of the origin place
				- 7 : blinking_delay : blinking delay
				
			Object :
				- result of table generation

			@ingroup m54Pages refPages
		*/
		class DeparturesTableRoutePlanningInterfacePage :
			public util::FactorableTemplate<interfaces::InterfacePage, DeparturesTableRoutePlanningInterfacePage>
		{
		public:
			static const std::string DATA_TITLE;
			static const std::string DATA_WIRING_CODE;
			static const std::string DATA_DISPLAY_SERVICE_NUMBER;
			static const std::string DATA_DISPLAY_TRACK_NUMBER;
			static const std::string DATA_ORIGIN_ID;
			static const std::string DATA_ORIGIN_NAME;
			static const std::string DATA_WITH_TRANSFER;
			static const std::string DATA_BLINKING_DELAY;

			DeparturesTableRoutePlanningInterfacePage();


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
				, bool displayTrackNumber,
				bool withTransfer
				, int blinkingDelay
				, const env::PublicTransportStopZoneConnectionPlace& place
				, const RoutePlanningListWithAlarm& rows
				, const server::Request* request = NULL
			) const;

		};
	}
}

#endif // SYNTHESE_DeparturesTableInterfacePage_H__
