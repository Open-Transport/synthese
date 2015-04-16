
/** PlaceAlias class implementation.
	@file PlaceAlias.cpp

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

#include "PlaceAlias.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string Registry<PlaceAlias>::KEY("PlaceAlias");
		template<> const string FactorableTemplate<NamedPlace, PlaceAlias>::FACTORY_KEY("PlaceAlias");
	}

	namespace geography
	{
		PlaceAlias::PlaceAlias(
			RegistryKeyType id
		):	Registrable(id),
			IncludingPlace<NamedPlace>()
		{}



		PlaceAlias::~PlaceAlias ()
		{
		}



		const NamedPlace*
		PlaceAlias::getAliasedPlace() const
		{
			assert(!getIncludedPlaces().empty());

			return *getIncludedPlaces().begin();
		}



		const string PlaceAlias::getOfficialName() const
		{
			return getAliasedPlace()->getOfficialName ();
		}



		void PlaceAlias::setAliasedPlace( const NamedPlace* place )
		{
			assert(place);

			clearIncludedPlaces();
			addIncludedPlace(*place);
		}



		std::string PlaceAlias::getNameForAllPlacesMatcher(std::string text) const
		{
			return getAliasedPlace()->getNameForAllPlacesMatcher(getName());
		}
	}
}
