
//////////////////////////////////////////////////////////////////////////
///	DesignatedLinePhysicalStopInheritedTableSync class header.
///	@file DesignatedLinePhysicalStopInheritedTableSync.hpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_pt_DesignatedLinePhysicalStopInheritedTableSync_hpp__
#define SYNTHESE_pt_DesignatedLinePhysicalStopInheritedTableSync_hpp__

#include "DBInheritedRegistryTableSync.hpp"

#include "LineStopTableSync.h"
#include "DesignatedLinePhysicalStop.hpp"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		///	DesignatedLinePhysicalStopInheritedTableSync class.
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m35ILS refILS
		///	@author Hugues Romain
		/// @since 3.2.1
		/// @date 2011
		class DesignatedLinePhysicalStopInheritedTableSync:
			public db::DBInheritedRegistryTableSync<
				LineStopTableSync,
				DesignatedLinePhysicalStopInheritedTableSync,
				DesignatedLinePhysicalStop
			>
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Search of DesignatedLinePhysicalStop objects.
			/// @param env Environment to populate when loading objects
			/// @param startStop id of the starting vertex of the edge
			/// @param endStop id of the ending vertex of the edge
			/// @param first first element to return
			/// @param number maximal number of elements to return
			/// @param orderById order the returned elements by their id
			/// @param raisingOrder order ascendantly or not
			/// @param linkLevel automatic load of objects linked by foreign key
			///	@author Hugues Romain
			/// @since 3.2.1
			/// @date 2011
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> startStop = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> endStop = boost::optional<util::RegistryKeyType>(),
				std::size_t first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderById = false,
				bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
}	}

#endif // SYNTHESE_pt_DesignatedLinePhysicalStopInheritedTableSync_hpp__
