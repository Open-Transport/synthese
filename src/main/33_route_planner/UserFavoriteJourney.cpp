
/** UserFavoriteJourney class implementation.
	@file UserFavoriteJourney.cpp

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

#include "UserFavoriteJourney.h"

namespace synthese
{
	using namespace security;
	using namespace env;

	namespace routeplanner
	{


		void UserFavoriteJourney::setRank( int value )
		{
			_rank = value;
		}

		void UserFavoriteJourney::setOriginCityName( const std::string& value )
		{
			_originCityName = value;
		}

		void UserFavoriteJourney::setOriginPlaceName( const std::string& value )
		{
			_originPlaceName = value;
		}

		void UserFavoriteJourney::setDestinationCityName( const std::string& value )
		{
			_destinationCityName = value;
		}

		void UserFavoriteJourney::setDestinationPlaceName( const std::string& value )
		{
			_destinationPlaceName = value;
		}

		void UserFavoriteJourney::setAccessParameters( const AccessParameters& value )
		{
			_accessParameters = value;
		}

		int UserFavoriteJourney::getRank() const
		{
			return _rank;
		}

		const std::string& UserFavoriteJourney::getOriginCityName() const
		{
			return _originCityName;
		}

		const std::string& UserFavoriteJourney::getOriginPlaceName() const
		{
			return _originPlaceName;
		}

		const std::string& UserFavoriteJourney::getDestinationCityName() const
		{
			return _destinationCityName;
		}

		const std::string& UserFavoriteJourney::getDestinationPlaceName() const
		{
			return _destinationPlaceName;
		}

		const AccessParameters& UserFavoriteJourney::getAccessParameters() const
		{
			return _accessParameters;
		}

		void UserFavoriteJourney::setUser( const User* value )
		{
			_user = value;
		}

		const User* UserFavoriteJourney::getUser() const
		{
			return _user;
		}

		UserFavoriteJourney::UserFavoriteJourney()
			: util::Registrable<uid,UserFavoriteJourney>()
			, _rank(UNKNOWN_VALUE)
			, _user(NULL)
		{

		}
		
	}
}
