
/** WebPageTableSync class header.
	@file WebPageTableSync.h

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

#ifndef SYNTHESE_OperationUnitTableSync_H__
#define SYNTHESE_OperationUnitTableSync_H__

#include "OperationUnit.hpp"

#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		/// 37.10 Table : Operation unit.
		///	@ingroup m37LS refLS
		/// @author Hugues Romain
		/// @date 2013
		//////////////////////////////////////////////////////////////////////////
		class OperationUnitTableSync:
			public db::DBDirectTableSyncTemplate<OperationUnitTableSync, OperationUnit>
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Operation unit search.
			/// @param env Environment to use and populate by the search
			/// @param first First page to answer
			/// @param number Number pages to answer. Undefined value seems unlimited size.
			/// @param orderByRank the results are sorted by rank
			/// @param raisingOrder the results are ordered ascendantly (true) or descendantly (false)
			/// @param linkLevel level of link to apply when loading objects
			/// @return Found operation units.
			/// @author Hugues Romain
			/// @date 2013
			static SearchResult Search(
				util::Env& env,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_SiteTableSync_H__
