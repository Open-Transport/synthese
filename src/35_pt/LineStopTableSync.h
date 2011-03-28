
/** LineStopTableSync class header.
	@file LineStopTableSync.h

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

#ifndef SYNTHESE_LineStopTableSync_H__
#define SYNTHESE_LineStopTableSync_H__

#include "LineStop.h"
#include "DBInheritanceTableSyncTemplate.hpp"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.10 Table : Route stop.
		///	@ingroup m35LS refLS
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Table name : t010_line_stops
		/// Corresponding class : LineStop
		///
		/// Fields :
		///	<ul>
		///		<li>physical_stop_id</li>
		///		<li>line_id</li>
		///		<li>rank_in_path</li>
		///		<li>is_departure</li>
		///		<li>is_arrival</li>
		///		<li>metric_offset</li>
		///		<li>schedule_input</li>
		///		<li>geometry : optional. Defines the geometry of the edge. Each point is separated
		///		by coma.</li>
		///	</ul>
		class LineStopTableSync:
			public db::DBInheritanceTableSyncTemplate<LineStopTableSync,LineStop>
		{
		public:
			static const std::string COL_PHYSICALSTOPID;
			static const std::string COL_LINEID;
			static const std::string COL_RANKINPATH;
			static const std::string COL_ISDEPARTURE;
			static const std::string COL_ISARRIVAL;
			static const std::string COL_METRICOFFSET;
			static const std::string COL_SCHEDULEINPUT;
			static const std::string COL_INTERNAL_SERVICE;

			/** LineStop search.
				(other search parameters)
				@param first First LineStop object to answer
				@param number Number of LineStop objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found LineStop objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> lineId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> physicalStopId = boost::optional<util::RegistryKeyType>(),
				int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>()
				, bool orderByRank = true
				, bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			//////////////////////////////////////////////////////////////////////////
			/// Inserts a stop in a route.
			/// @param lineStop line stop to insert
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			//////////////////////////////////////////////////////////////////////////
			static void InsertStop(
				LineStop& lineStop
			);



			//////////////////////////////////////////////////////////////////////////
			/// Removes a stop from a route.
			/// @param lineStop line stop to remove
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			//////////////////////////////////////////////////////////////////////////
			static void RemoveStop(
				const LineStop& lineStop
			);
		};
	}
}

#endif // SYNTHESE_LineStopTableSync_H__
