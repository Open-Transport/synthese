
/** DeparturesTableDestinationInterfacepage class header.
	@file DeparturesTableDestinationInterfacepage.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_DeparturesTableDestinationInterfacepage_H__
#define SYNTHESE_DeparturesTableDestinationInterfacepage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace graph
	{
		class ServicePointer;
	}

	namespace departure_boards
	{
		/** DeparturesTableDestinationInterfacepage Interface Page Class.
			@ingroup m54Pages refPages
			@author Hugues
			@date 2009

			@code departures_table_destination @endcode

			This class allows the interface to design the output for a destination of a departure table row.
			The destination can be an intermediary stop or the end station.

			Parameters :
				- 0 : stop ID
				- 1 : stop city name
				- 2 : stop name
				- 3 : stop 26 characters alias
				- 4 : stop 13 characters alias
				- 5 : last displayed stop was in the same city ?
				- 6 : arrival time
				- 7 : is the end station
				- 8 : transfer destinations from this stop (should be the output of DeparturesTableTransferDestinationInterfacePage)
				- 9 : rank

		*/
		class DeparturesTableDestinationInterfacepage
			: public util::FactorableTemplate<interfaces::InterfacePage, DeparturesTableDestinationInterfacepage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param object service use
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const graph::ServicePointer& object,
				bool lastDisplayedStopWasInTheSameCity,
				bool isTheEndStation,
				const std::string& transfersString,
				std::size_t rank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;

			DeparturesTableDestinationInterfacepage();
		};
	}
}

#endif // SYNTHESE_DeparturesTableDestinationInterfacepage_H__
