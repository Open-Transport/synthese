
/** DeparturesTableInterfacePage class header.
	@file DeparturesTableInterfacePage.h

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

#ifndef SYNTHESE_DeparturesTableInterfacePage_H__
#define SYNTHESE_DeparturesTableInterfacePage_H__

#include <string>

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

#include "34_departures_table/Types.h"

namespace synthese
{
	namespace departurestable
	{
		/** Departures table.
			
			@code departurestable P1 P2 P3 @endcode
			
			Parameters :
				- 0 : Title of the screen
				- 1 : Wiring code
				- 2 : 1|0 : Service number display
				- 3 : display the track number
				- 4 : number of intermediates stops to display
				
			Object :
				- result of table generation

			@ingroup m34Pages refPages
		*/
		class DeparturesTableInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage, DeparturesTableInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.

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
				, int intermediatesStopsToDisplay
				, const ArrivalDepartureListWithAlarm& rows
				, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_DeparturesTableInterfacePage_H__
