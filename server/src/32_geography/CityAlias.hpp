
/** CityAlias class header.
	@file CityAlias.hpp

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

#ifndef SYNTHESE_geography_CityAlias_hpp__
#define SYNTHESE_geography_CityAlias_hpp__

#include "Object.hpp"
#include "IncludingPlace.h"
#include "PointerField.hpp"
#include "StringField.hpp"

namespace synthese
{
	namespace geography
	{
		class City;

		FIELD_BOOL(Visible)
		FIELD_POINTER(AliasedCity, City)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(AliasedCity),
			FIELD(Name),
			FIELD(Code),
			FIELD(Visible)
		> CityAliasSchema;

		//////////////////////////////////////////////////////////////////////////
		/// Alias to a city.
		///	@ingroup m32
		///
		///	If visible (_visible=true), then the alias is added in the city list lexical matcher to be used by the web sites. In this case, the object aliases the name of the city by an other.
		///	If non visible (_visible=false), then the alias is not accessible by name search. In this case the object aliases only the city code, in import purposes.
		///
		///	The aliased city is the only one included place in IncludingPlace::_includedPlaces.
		///
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		class CityAlias:
			public Object<CityAlias, CityAliasSchema>,
			public IncludingPlace<City>
		{
		public:

			CityAlias(util::RegistryKeyType id = 0);


			//! @name Getters
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets the aliased city.
				/// @return The aliased city
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				///
				/// The aliased city is read in the IncludingPlace::_includedPlaces attribute which is
				/// supposed to contain only one city.
				const City* getCity() const;

			//@}


			//! @name Setters
			//@{
			//@}


			//! @name Services
			//@{
			//@}

			//! @name Modifiers
			//@{
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;

		};
	}
}

#endif // SYNTHESE_geography_CityAlias_hpp__
