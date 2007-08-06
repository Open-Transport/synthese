
/** PlaceAlias class implementation.
	@file PlaceAlias.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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


namespace synthese
{
	namespace env
	{


		PlaceAlias::PlaceAlias (uid id,
					std::string name,
					const Place* aliasedPlace,
					const City* city)
		: synthese::util::Registrable<uid,PlaceAlias> (id)
		, IncludingPlace (name, city)
		{
			addIncludedPlace (aliasedPlace);
		}



		PlaceAlias::~PlaceAlias ()
		{

		}



		const Place* 
		PlaceAlias::getAliasedPlace () const
		{
			return _includedPlaces[0];
		}


		const std::string& 
		PlaceAlias::getOfficialName () const
		{
			return getAliasedPlace ()->getOfficialName ();
		}

		void PlaceAlias::setAliasedPlace( const Place* place )
		{
			_includedPlaces.clear();
			addIncludedPlace(place);
		}

		uid PlaceAlias::getId() const
		{
			return getKey();
		}








	}
}



