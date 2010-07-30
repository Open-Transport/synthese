
/** UserFavoriteInterfacePage class header.
	@file UserFavoriteInterfacePage.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_UserFavoriteInterfacePage_H__
#define SYNTHESE_UserFavoriteInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace pt_website
	{
		class UserFavoriteJourney;

		/** UserFavoriteInterfacePage Interface Page Class.
			@ingroup m56Pages refPages
			@author Hugues Romain
			@date 2007

			@code user_favorite_journey @endcode

			Parameters :
				- 0 : ID
				- 1 : Rank
				- 2 : Origin city name
				- 3 : Origin place name
				- 4 : Destination city name
				- 5 : Destination place name

			Object : UserFavoriteJourney
		*/
		class UserFavoriteInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, UserFavoriteInterfacePage>
		{
		public:
			static const std::string DATA_ID;
			static const std::string DATA_RANK;
			static const std::string DATA_ORIGIN_CITY_NAME;
			static const std::string DATA_ORIGIN_PLACE_NAME;
			static const std::string DATA_DESTINATION_CITY_NAME;
			static const std::string DATA_DESTINATION_PLACE_NAME;

			UserFavoriteInterfacePage();

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...	
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream
				, const UserFavoriteJourney* fav
				, interfaces::VariablesMap& variables
				, const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_UserFavoriteInterfacePage_H__
