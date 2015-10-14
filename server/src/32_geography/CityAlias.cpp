
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

	CLASS_DEFINITION(CityAlias, "t065_city_aliases", 65)
	FIELD_DEFINITION_OF_OBJECT(CityAlias, "city_alias_id", "city_aliases_ids")

	FIELD_DEFINITION_OF_TYPE(AliasedCity, "aliased_city_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Visible, "visible", SQL_BOOLEAN)

	namespace geography
	{
		CityAlias::CityAlias( util::RegistryKeyType id)
			: Registrable(id),
			  Object<CityAlias, CityAliasSchema>(
				  Schema(
					  FIELD_VALUE_CONSTRUCTOR(Key, id),
					  FIELD_DEFAULT_CONSTRUCTOR(AliasedCity),
					  FIELD_DEFAULT_CONSTRUCTOR(Name),
					  FIELD_DEFAULT_CONSTRUCTOR(Code),
					  FIELD_DEFAULT_CONSTRUCTOR(Visible)
			  )	)
		{
		}

		const City*
		CityAlias::getCity() const
		{
			return (get<AliasedCity>() ? get<AliasedCity>().get_ptr() : NULL);
		}

		bool CityAlias::allowUpdate(const server::Session* session) const
		{
			return true;
		}

		bool CityAlias::allowCreate(const server::Session* session) const
		{
			return true;
		}

		bool CityAlias::allowDelete(const server::Session* session) const
		{
			return true;
		}

}	}
