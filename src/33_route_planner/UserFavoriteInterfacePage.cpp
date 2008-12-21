
/** UserFavoriteInterfacePage class implementation.
	@file UserFavoriteInterfacePage.cpp
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

#include "UserFavoriteInterfacePage.h"

#include "33_route_planner/UserFavoriteJourney.h"


using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, routeplanner::UserFavoriteInterfacePage>::FACTORY_KEY("user_favorite_journey");
	}

	namespace routeplanner
	{

		void UserFavoriteInterfacePage::display(
			std::ostream& stream
			, const UserFavoriteJourney* fav
			, VariablesMap& variables
			, const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			pv.push_back(Conversion::ToString(fav->getKey()));
			pv.push_back(Conversion::ToString(fav->getRank()));
			pv.push_back(fav->getOriginCityName());
			pv.push_back(fav->getOriginPlaceName());
			pv.push_back(fav->getDestinationCityName());
			pv.push_back(fav->getDestinationPlaceName());

			InterfacePage::display(
				stream
				, pv
				, variables
				, static_cast<const void*>(fav)
				, request
			);
		}

		UserFavoriteInterfacePage::UserFavoriteInterfacePage()
			: Registrable(UNKNOWN_VALUE),
			util::FactorableTemplate<interfaces::InterfacePage, UserFavoriteInterfacePage>()
		{
	
		}
	}
}
