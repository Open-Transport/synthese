
/** UserFavoriteJourney class implementation.
	@file UserFavoriteJourney.cpp

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

#include "UserFavoriteJourney.h"

#include "Profile.h"
#include "Registry.h"
#include "Session.h"
#include "TransportWebsiteRight.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace pt_website;
	using namespace security;
	using namespace util;

	CLASS_DEFINITION(UserFavoriteJourney, "t048_user_favorite_journey", 48)
	FIELD_DEFINITION_OF_OBJECT(UserFavoriteJourney, "user_favorite_journey_id", "user_favorite_journey_ids")

	FIELD_DEFINITION_OF_TYPE(FavoriteJourneyUser, "user_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(OriginCityName, "origin_city_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(OriginPlaceName, "origin_place_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DestinationCityName, "destination_city_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DestinationPlaceName, "destination_place_name", SQL_TEXT)

	namespace pt_website
	{
		void UserFavoriteJourney::setOriginCityName( const std::string& value )
		{
			set<OriginCityName>(value);
		}

		void UserFavoriteJourney::setOriginPlaceName( const std::string& value )
		{
			set<OriginPlaceName>(value);
		}

		void UserFavoriteJourney::setDestinationCityName( const std::string& value )
		{
			set<DestinationCityName>(value);
		}

		void UserFavoriteJourney::setDestinationPlaceName( const std::string& value )
		{
			set<DestinationPlaceName>(value);
		}

		void UserFavoriteJourney::setAccessParameters( const AccessParameters& value )
		{
			_accessParameters = value;
		}

		const std::string& UserFavoriteJourney::getOriginCityName() const
		{
			return get<OriginCityName>();
		}

		const std::string& UserFavoriteJourney::getOriginPlaceName() const
		{
			return get<OriginPlaceName>();
		}

		const std::string& UserFavoriteJourney::getDestinationCityName() const
		{
			return get<DestinationCityName>();
		}

		const std::string& UserFavoriteJourney::getDestinationPlaceName() const
		{
			return get<DestinationPlaceName>();
		}

		const AccessParameters& UserFavoriteJourney::getAccessParameters() const
		{
			return _accessParameters;
		}

		UserFavoriteJourney::UserFavoriteJourney(
			RegistryKeyType key
		):	Registrable(key),
			Object<UserFavoriteJourney, UserFavoriteJourneySchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(FavoriteJourneyUser),
					FIELD_DEFAULT_CONSTRUCTOR(Rank),
					FIELD_DEFAULT_CONSTRUCTOR(OriginCityName),
					FIELD_DEFAULT_CONSTRUCTOR(OriginPlaceName),
					FIELD_DEFAULT_CONSTRUCTOR(DestinationCityName),
					FIELD_DEFAULT_CONSTRUCTOR(DestinationPlaceName)
			)	)
		{

		}

		boost::optional<std::size_t> UserFavoriteJourney::getRank() const
		{
			if (get<Rank>() == 0)
				return boost::optional<std::size_t>();

			return boost::optional<std::size_t>(get<Rank>());
		}

		const security::User* UserFavoriteJourney::getUser() const
		{
			if (get<FavoriteJourneyUser>())
				return get<FavoriteJourneyUser>().get_ptr();

			return NULL;
		}

		void UserFavoriteJourney::setUser(security::User* value)
		{
			set<FavoriteJourneyUser>(value
				? boost::optional<security::User&>(*value)
				: boost::none);
		}

		bool UserFavoriteJourney::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(security::WRITE);
		}

		bool UserFavoriteJourney::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(security::WRITE);
		}

		bool UserFavoriteJourney::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(security::DELETE_RIGHT);
		}

	}
}
