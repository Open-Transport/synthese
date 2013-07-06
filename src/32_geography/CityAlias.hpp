
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

#include "IncludingPlace.h"
#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace geography
	{
		class City;

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
			public IncludingPlace<City>,
			public util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<CityAlias>	Registry;

		private:

			bool _visible;  //!< Visibility of the alias in city list lexical matcher (see class description for more informations)
			std::string _name; //!< The name of the alias
			std::string _code; //!< Unique code identifier for city within its country (france => INSEE code)

		public:
			CityAlias(util::RegistryKeyType id = 0);

			//! @name Getters
			//@{
				bool getVisible() const;
				virtual std::string getName() const;
				const std::string& getCode() const;
			//@}


			//! @name Setters
			//@{
				void setVisible(bool value);
				void setName(const std::string& value);
				void setCode(const std::string& value);
			//@}


			//! @name Services
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

			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Sets the aliased city
				/// @param value the aliased city
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				///
				/// The aliased city is stored in the IncludingPlace::_includedPlaces attribute.
				/// If non empty, it is cleaned before.
				void setCity(const City* value);
			//@}
		};
	}
}

#endif // SYNTHESE_geography_CityAlias_hpp__
