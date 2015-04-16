
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
#include "Registry.h"

using namespace std;

namespace synthese
{
	using namespace security;
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<pt_website::UserFavoriteJourney>::KEY("UserFavoriteJourney");
	}

	namespace pt_website
	{
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

		UserFavoriteJourney::UserFavoriteJourney(
			RegistryKeyType key
		):	Registrable(key)
			, _user(NULL)
		{

		}

	}
}
