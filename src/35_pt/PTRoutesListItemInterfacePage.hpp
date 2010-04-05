
/** PTRoutesListItemInterfacePage class header.
	@file PTRoutesListItemInterfacePage.hpp
	@author Hugues Romain
	@date 2010

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

#ifndef SYNTHESE_PTRoutesListItemInterfacePage_H__
#define SYNTHESE_PTRoutesListItemInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	
	namespace pt
	{
		class Line;
	}

	namespace pt
	{
		/** PTRoutesListItemInterfacePage Interface Page Class.
			@ingroup m35Pages refPages
			@author Hugues Romain
			@date 2010

			@code ptroutes_list_item @endcode

			Parameters :
				- 0 : Route ID
				- 1 : Route Name
				- 2 : Route length
				- 3 : Route stops number
				- 4 : Route direction
				- 5 : Origin city name
				- 6 : Origin stop name
				- 7 : Destination city name
				- 8 : Destination stop name
				- 9 : Rank in the list
				- 10 : Rank is odd

			Object : Line
		*/
		class PTRoutesListItemInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, PTRoutesListItemInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param object The route to display
				@param rank The rank of the item in the list
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const pt::Line& object,
				std::size_t rank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
			
			PTRoutesListItemInterfacePage();
		};
	}
}

#endif // SYNTHESE_PTRoutesListItemInterfacePage_H__
