
/** CityAlias class implementation.
	@file CityAlias.cpp

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

#include "CityAlias.hpp"
#include "City.h"

using namespace std;

namespace synthese
{
	using namespace geography;
	using namespace util;

	namespace util
	{
		template<> const string Registry<geography::CityAlias>::KEY("CityAlias");
	}



	namespace geography
	{
		CityAlias::CityAlias( util::RegistryKeyType id)
			: Registrable(id)
		{}



		bool CityAlias::getVisible() const
		{
			return _visible;
		}



		std::string CityAlias::getName() const
		{
			return _name;
		}



		const std::string& CityAlias::getCode() const
		{
			return _code;
		}



		void CityAlias::setVisible( bool value )
		{
			_visible = value;
		}



		void CityAlias::setName( const std::string& value )
		{
			_name = value;
		}



		void CityAlias::setCode( const std::string& value )
		{
			_code = value;
		}



		const City* CityAlias::getCity() const
		{
			assert(getIncludedPlaces().size() == 1);
			assert(dynamic_cast<const City*>(*getIncludedPlaces().begin()));

			return dynamic_cast<const City*>(*getIncludedPlaces().begin());
		}



		void CityAlias::setCity( const City* value )
		{
			assert(value);

			clearIncludedPlaces();
			addIncludedPlace(*value);
		}
}	}
