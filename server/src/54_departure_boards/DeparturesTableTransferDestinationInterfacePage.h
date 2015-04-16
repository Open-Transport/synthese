
/** DeparturesTableTransferDestinationInterfacePage class header.
	@file DeparturesTableTransferDestinationInterfacePage.h
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

#ifndef SYNTHESE_DeparturesTableTransferDestinationInterfacePage_H__
#define SYNTHESE_DeparturesTableTransferDestinationInterfacePage_H__

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
		/** DeparturesTableTransferDestinationInterfacePage Interface Page Class.
			@ingroup m54Pages refPages
			@author Hugues
			@date 2009

			@code departures_table_transfer_destination @endcode

			Parameters :
				- 0 : transport mode ID
				- 1 : line ID
				- 2 : line number (short name)
				- 3 : line css class
				- 4 : line img path
				- 5 : destination ID
				- 6 : destination city name
				- 7 : destination name
				- 8 : destination 26 characters alias
				- 9 : destination 13 characters alias
				- 10 : departure time
				- 11 : arrival time
				- 12 : transfer rank in the transfer place
				- 13 : total transfer rank

		*/
		class DeparturesTableTransferDestinationInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, DeparturesTableTransferDestinationInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param serviceUse service use to draw
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const graph::ServicePointer& object,
				std::size_t localTransferRank,
				std::size_t totalTransferRank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;

			DeparturesTableTransferDestinationInterfacePage();
		};
	}
}

#endif // SYNTHESE_DeparturesTableTransferDestinationInterfacePage_H__
