
/** PublicBikeNetworkTableSync class header.
	@file PublicBikeNetworkTableSync.h

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

#ifndef SYNTHESE_public_biking_PUBLICBIKENETWORKTABLESYNC_H
#define SYNTHESE_public_biking_PUBLICBIKENETWORKTABLESYNC_H

#include <string>
#include <iostream>

#include "PublicBikeNetwork.hpp"

#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace public_biking
	{
		//////////////////////////////////////////////////////////////////////////
		/// 65.10 Table : Public bike network.
		///	@ingroup m65LS refLS
		/// @author Camille Hue
		/// @date 2015
		//////////////////////////////////////////////////////////////////////////
		class PublicBikeNetworkTableSync:
			public db::DBDirectTableSyncTemplate<
				PublicBikeNetworkTableSync,
				PublicBikeNetwork
			>
		{
		 public:
			virtual bool allowList( const server::Session* session ) const;
		};
	}
}

#endif
