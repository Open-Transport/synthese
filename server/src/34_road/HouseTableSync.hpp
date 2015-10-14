
/** HouseTableSync class header.
	@file HouseTableSync.hpp
	@author Gaël Sauvanet

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

#ifndef SYNTHESE_HouseTableSync_H__
#define SYNTHESE_HouseTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "House.hpp"
#include "FetcherTemplate.h"
#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace road
	{
		/** 34.10 House table synchronizer.
			@ingroup m34LS refLS
			@author Gaël Sauvanet
			@date 2011

			Table number : 78
			Corresponding class : House

			</dl>
		*/
		class HouseTableSync:
				public db::DBDirectTableSyncTemplate<
					HouseTableSync,
					House
				>
		{
		public:
			HouseTableSync() {}
			~HouseTableSync() {}

			virtual bool allowList( const server::Session* session ) const;

		};
	}
}

#endif // SYNTHESE_AddressTableSync_H__
