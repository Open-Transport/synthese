
/** PlaceWithDisplayBoards class header.
	@file PlaceWithDisplayBoards.hpp

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

#ifndef SYNTHESE_departure_boards_PlaceWithDisplayBoards_hpp__
#define SYNTHESE_departure_boards_PlaceWithDisplayBoards_hpp__

#include "Registrable.h"
#include "TreeAlphabeticalOrderingPolicy.hpp"
#include "TreeRoot.hpp"

#include "NamedPlace.h"
#include "Registry.h"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreen;

		/** PlaceWithDisplayBoards class.
			@ingroup m55
		*/
		class PlaceWithDisplayBoards:
			public virtual util::Registrable,
			public tree::TreeRoot<DisplayScreen, tree::TreeAlphabeticalOrderingPolicy>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<PlaceWithDisplayBoards>	Registry;

		private:
			const geography::NamedPlace* _place;

		public:
			PlaceWithDisplayBoards(
				const geography::NamedPlace* place = NULL
			):	_place(place)
			{}

			virtual util::RegistryKeyType getKey() const { return _place ? _place->getKey() : 0; }
			void setPlace(const geography::NamedPlace* value){ _place = value; }
			const geography::NamedPlace* getPlace() const { return _place; }
		};
	}
}

#endif // SYNTHESE_departure_boards_PlaceWithDisplayBoards_hpp__

