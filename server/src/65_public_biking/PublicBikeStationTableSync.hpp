
/** PublicBikeStationTableSync class header.
	@file PublicBikeStationTableSync.hpp

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

#ifndef SYNTHESE_public_biking_PUBLICBIKESTATIONTABLESYNC_H
#define SYNTHESE_public_biking_PUBLICBIKESTATIONTABLESYNC_H

#include "PublicBikeStation.hpp"

#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"
#include "FetcherTemplate.h"

namespace synthese
{
	namespace public_biking
	{
		class PublicBikeStationTableSync:
			public db::DBDirectTableSyncTemplate<
				PublicBikeStationTableSync,
				PublicBikeStation>,
			public db::FetcherTemplate<graph::Vertex, PublicBikeStationTableSync>,
			public db::FetcherTemplate<geography::NamedPlace, PublicBikeStationTableSync>
		{
		public:
			virtual bool allowList( const server::Session* session ) const;

			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> networkId = boost::optional<util::RegistryKeyType>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};

	}
}

#endif
