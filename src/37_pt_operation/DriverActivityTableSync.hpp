
/** DriverActivityTableSync class header.
	@file DriverActivityTableSync.h

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

#ifndef SYNTHESE_DriverActivityTableSync_H__
#define SYNTHESE_DriverActivityTableSync_H__

#include "DBRegistryTableSyncTemplate.hpp"
#include "DriverActivity.hpp"

#include <string>
#include <iostream>

namespace synthese
{
	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		/// 37.10 Table : Driver allocation
		///	@ingroup m37LS refLS
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Table name : t085_driver_allocations
		class DriverActivityTableSync:
			public db::DBRegistryTableSyncTemplate<DriverActivityTableSync, DriverActivity>
		{
		public:
			/** Driver allocation search.
				@author Hugues Romain
				@date 2012
			*/
			static SearchResult Search(
				util::Env& env,
				size_t first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByTime = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif
