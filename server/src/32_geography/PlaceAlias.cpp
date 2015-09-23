
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

#include "City.h"
#include "Fetcher.h"
#include "GeographyModule.h"
#include "PTModule.h"
#include "StopArea.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<NamedPlace, PlaceAlias>::FACTORY_KEY("PlaceAlias");
	}

	CLASS_DEFINITION(PlaceAlias, "t011_place_aliases", 11)
	FIELD_DEFINITION_OF_OBJECT(PlaceAlias, "place_alias_id", "place_alias_ids")

	FIELD_DEFINITION_OF_TYPE(AliasedPlaceId, "aliased_place_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ParentCity, "city_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(IsCityMainConnection, "is_city_main_connection", SQL_BOOLEAN)

	namespace geography
	{
		PlaceAlias::PlaceAlias(
			RegistryKeyType id
		):	Registrable(id),
			Object<PlaceAlias, PlaceAliasSchema> (
				Schema (
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(AliasedPlaceId),
					FIELD_DEFAULT_CONSTRUCTOR(ParentCity),
					FIELD_DEFAULT_CONSTRUCTOR(IsCityMainConnection)
			)	),
			IncludingPlace<NamedPlace>(),
			NamedPlaceTemplate<PlaceAlias>()
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

		void PlaceAlias::link(
			util::Env& env,
			bool withAlgorithmOptimizations /*= false*/
		){
			if (get<ParentCity>())
			{
				setCity(&*get<ParentCity>());
			}
			setAliasedPlace(db::Fetcher<NamedPlace>::Fetch(get<AliasedPlaceId>(), env).get());

			// Registration to city matcher
			if(getCity())
			{
				const_cast<City*>(getCity())->addPlaceToMatcher(env.getEditableSPtr(this));
				if (get<IsCityMainConnection>())
				{
					getCity()->addIncludedPlace(*this);
				}
			}

			// Registration to all places matcher
			if(	&env == &Env::GetOfficialEnv() &&
				withAlgorithmOptimizations
			){
				GeographyModule::GetGeneralAllPlacesMatcher().add(
					getFullName(),
					env.getEditableSPtr(this)
				);
			}
			
			if(	&env == &Env::GetOfficialEnv() &&
				getCity() &&
				withAlgorithmOptimizations &&
				dynamic_cast<const pt::StopArea*>(getAliasedPlace())
			){
				const pt::StopArea* stopArea = static_cast<const pt::StopArea*>(getAliasedPlace());
				pt::PTModule::GetGeneralStopsMatcher().add(
					getFullName(),
					env.getEditableSPtr(const_cast<pt::StopArea*>(stopArea))
				);
			}
		}


		void PlaceAlias::unlink()
		{
			// City matcher
			City* city(const_cast<City*>(getCity()));
			if (city != NULL)
			{
				city->removePlaceFromMatcher(*this);
			}

			if(Env::GetOfficialEnv().contains(*this))
			{
				// General all places
				GeographyModule::GetGeneralAllPlacesMatcher().remove(
					getFullName()
				);
			}

			if(Env::GetOfficialEnv().contains(*this))
			{
				// General public places
				pt::PTModule::GetGeneralStopsMatcher().remove(
					getFullName()
				);
			}
		}
	}
}
