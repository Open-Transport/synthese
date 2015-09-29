
/** UserBookmarksService class implementation.
	@file UserBookmarksService.cpp
	@author Hugues Romain
	@date 2011

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

#include "UserBookmarksService.hpp"

#include "Profile.h"
#include "RequestException.h"
#include "Request.h"
#include "Session.h"
#include "TransportWebsiteRight.h"
#include "User.h"
#include "UserFavoriteJourney.h"
#include "UserFavoriteJourneyTableSync.h"
#include "Webpage.h"
#include "UserTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function, pt_website::UserBookmarksService>::FACTORY_KEY("UserBookmarksService");

	namespace pt_website
	{
		const string UserBookmarksService::PARAMETER_USER_ID("user_id");
		const string UserBookmarksService::PARAMETER_ITEM_DISPLAY_TEMPLATE_ID("item_display_template_id");

		const string UserBookmarksService::DATA_RANK("rank");
		const string UserBookmarksService::DATA_ORIGIN_CITY_NAME("origin_city_name");
		const string UserBookmarksService::DATA_ORIGIN_PLACE_NAME("origin_place_name");
		const string UserBookmarksService::DATA_DESTINATION_CITY_NAME("destination_city_name");
		const string UserBookmarksService::DATA_DESTINATION_PLACE_NAME("destination_place_name");

		ParametersMap UserBookmarksService::_getParametersMap() const
		{
			ParametersMap map;
			if(_user.get())
			{
				map.insert(PARAMETER_USER_ID, _user->getKey());
			}
			if(_itemDisplayTemplate.get())
			{
				map.insert(PARAMETER_ITEM_DISPLAY_TEMPLATE_ID, _itemDisplayTemplate->getKey());
			}
			return map;
		}



		void UserBookmarksService::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getOptional<RegistryKeyType>(PARAMETER_USER_ID)) try
			{
				_user = UserTableSync::Get(map.get<RegistryKeyType>(PARAMETER_USER_ID), *_env);
			}
			catch(ObjectNotFoundException<User>&)
			{
				throw RequestException("No such user");
			}
		}



		util::ParametersMap UserBookmarksService::run(
			std::ostream& stream,
			const Request& request
		) const {

			const User* user(_user.get());

			if(!user && request.getSession())
			{
				user = request.getSession()->getUser().get();
			}

			if(!user)
			{
				throw RequestException("No user");
			}

			UserFavoriteJourneyTableSync::SearchResult favorites(
				UserFavoriteJourneyTableSync::Search(*_env, _user.get())
			);
			BOOST_FOREACH(const boost::shared_ptr<UserFavoriteJourney>& fav, favorites)
			{
				ParametersMap pm(getTemplateParameters());

				pm.insert(Request::PARAMETER_OBJECT_ID, fav->getKey());
				pm.insert(DATA_RANK, fav->getRank().get_value_or(0));
				pm.insert(DATA_ORIGIN_CITY_NAME, fav->getOriginCityName());
				pm.insert(DATA_DESTINATION_PLACE_NAME, fav->getOriginPlaceName());
				pm.insert(DATA_DESTINATION_CITY_NAME, fav->getDestinationCityName());
				pm.insert(DATA_DESTINATION_PLACE_NAME, fav->getDestinationPlaceName());

				_itemDisplayTemplate->display(stream, request, pm);
			}

			return util::ParametersMap();
		}



		bool UserBookmarksService::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(READ);
		}



		std::string UserBookmarksService::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
