
/** PublicPlaceEntrance class implementation.
	@file PublicPlaceEntrance.cpp

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

#include "PublicPlaceEntrance.hpp"

#include "RoadModule.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace road;
	using namespace util;

	CLASS_DEFINITION(PublicPlaceEntrance, "t084_public_place_entrances", 84)
	FIELD_DEFINITION_OF_OBJECT(PublicPlaceEntrance, "public_place_entrance_id", "public_place_entrance_ids")

	namespace road
	{
		PublicPlaceEntrance::PublicPlaceEntrance(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<PublicPlaceEntrance, PublicPlaceEntranceSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(PublicPlace),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(AddressField),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks)
			)	)
		{}



		void PublicPlaceEntrance::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Public place link
			optional<PublicPlace&> publicPlace(
				get<PublicPlace>()
			);
			if(publicPlace)
			{
				publicPlace->addEntrance(*this);
			}
		}



		void PublicPlaceEntrance::unlink()
		{
			// Public place link
			optional<PublicPlace&> publicPlace(
				get<PublicPlace>()
			);
			if(publicPlace)
			{
				publicPlace->removeEntrance(*this);
			}
		}
}	}
