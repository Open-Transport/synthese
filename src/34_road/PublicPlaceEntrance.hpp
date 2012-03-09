
/** PublicPlaceEntrance class header.
	@file PublicPlaceEntrance.hpp

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

#ifndef SYNTHESE_road_PublicPlaceEntrance_hpp__
#define SYNTHESE_road_PublicPlaceEntrance_hpp__

#include "Address.h"
#include "ImportableTemplate.hpp"
#include "Named.h"
#include "Registrable.h"

#include "Registry.h"

namespace synthese
{
	namespace road
	{
		class PublicPlace;

		/** Public placee entrance.
			@ingroup m34
		*/
		class PublicPlaceEntrance:
			public Address,
			public impex::ImportableTemplate<PublicPlaceEntrance>,
			public util::Named
		{
		public:
			typedef util::Registry<PublicPlaceEntrance> Registry;

			PublicPlaceEntrance(
				util::RegistryKeyType id = 0
			);


		private:
			/// @name Data
			//@{
			//@}

			/// @name Links
			//@{
				PublicPlace* _publicPlace;
			//@}

		public:
			/// @name Getters
			//@{
				PublicPlace* getPublicPlace() const { return _publicPlace; }
			//@}

			/// @name Setters
			//@{
				void setPublicPlace(PublicPlace* value){ _publicPlace = value; }
			//@}

			/// @name Virtual services
			//@{
			//@}
		};
	}
}

#endif // SYNTHESE_road_PublicPlaceEntrance_hpp__
