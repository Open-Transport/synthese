
/** PlaceAlias class header.
	@file PlaceAlias.h

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

#ifndef SYNTHESE_ENV_PLACEALIAS_H
#define SYNTHESE_ENV_PLACEALIAS_H

#include <vector>

#include "Object.hpp"

#include "IncludingPlace.h"
#include "NamedPlaceTemplate.h"
#include "StringField.hpp"

#ifdef _MSC_VER
#pragma warning( disable : 4250 )
#endif

namespace synthese
{
	namespace geography
	{
		FIELD_ID(AliasedPlaceId)
		FIELD_POINTER(ParentCity, geography::City)
		FIELD_BOOL(IsCityMainConnection)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(AliasedPlaceId),
			FIELD(ParentCity),
			FIELD(IsCityMainConnection)
		> PlaceAliasSchema;

		/** Place alias.

			@ingroup m32
		*/
		class PlaceAlias:
			public virtual Object<PlaceAlias, PlaceAliasSchema>,
			public IncludingPlace<NamedPlace>,
			public NamedPlaceTemplate<PlaceAlias>
		{
		public:
			PlaceAlias(
				util::RegistryKeyType id = 0
			);



			virtual ~PlaceAlias ();


			//! @name Getters/Setters
			//@{
				/** Gets official name of this place.

					@return aliased place official name.
				 */
				const std::string getOfficialName () const;


				/** Gets aliased place.
				 */
				const NamedPlace* getAliasedPlace () const;


				void setAliasedPlace(const NamedPlace* place);
			//@}


			virtual std::string getNameForAllPlacesMatcher(
				std::string text = std::string()
			) const;

			virtual std::string getName() const { return get<Name>(); }

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();
		};
	}
}

#endif
