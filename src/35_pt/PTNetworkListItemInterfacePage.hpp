
/** PTNetworkListItemInterfacePage class header.
	@file PTNetworkListItemInterfacePage.hpp
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

#ifndef SYNTHESE_PTNetworkListItemInterfacePage_H__
#define SYNTHESE_PTNetworkListItemInterfacePage_H__

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
		class TransportNetwork;
	}

	namespace pt
	{
		/** PTNetworkListItemInterfacePage Interface Page Class.
			@ingroup m35Pages refPages
			@author Hugues Romain
			@date 2010

			@code transport_network_list_item @endcode

			Parameters :
				- 0 : Network id
				- 1 : Network name
				- 2 : Rank in the list
				- 3 : Rank is odd

			Object : TransportNetwork
		*/
		class PTNetworkListItemInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, PTNetworkListItemInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param object Transport network to display
				@param rank Rank of the item in the list
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const pt::TransportNetwork& object,
				std::size_t rank,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
			
			PTNetworkListItemInterfacePage();
		};
	}
}

#endif // SYNTHESE_PTNetworkListItemInterfacePage_H__
