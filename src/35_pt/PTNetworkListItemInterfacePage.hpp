
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

#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace transportwebsite
	{
		class WebPage;
	}
	
	namespace pt
	{
		class TransportNetwork;
	
		/** PTNetworkListItemInterfacePage Interface Page Class.
			@ingroup m35Pages refPages
			@author Hugues Romain
			@date 2010

			Parameters :
				- roid : Network id
				- name : Network name
				- rank : Rank in the list
				- rank_is_odd : Rank is odd
		*/
		class PTNetworkListItemInterfacePage
		{
		public:
			static const std::string DATA_NAME;
			static const std::string DATA_RANK;
			static const std::string DATA_RANK_IS_ODD;

			/** Network display
				@param stream Stream to write on
				@param page page to use for display
				@param request Source request
				@param object Transport network to display
				@param rank Rank of the item in the list
			*/
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const transportwebsite::WebPage> page,
				const server::Request& request,
				const pt::TransportNetwork& object,
				std::size_t rank
			);
		};
	}
}

#endif // SYNTHESE_PTNetworkListItemInterfacePage_H__
