
/** ContinuousServiceTableSync class header.
	@file ContinuousServiceTableSync.h

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

#ifndef SYNTHESE_ContinuousServiceTableSync_H__
#define SYNTHESE_ContinuousServiceTableSync_H__

#include "ContinuousService.h"

#include <vector>
#include <string>
#include <iostream>

#include "SQLiteRegistryTableSyncTemplate.h"
#include "FetcherTemplate.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.10 Table : Continuous Service.
		///	@ingroup m35LS refLS
		///
		class ContinuousServiceTableSync:
			public db::SQLiteRegistryTableSyncTemplate<ContinuousServiceTableSync,ContinuousService>,
			public db::FetcherTemplate<SchedulesBasedService, ContinuousServiceTableSync>,
			public db::FetcherTemplate<graph::Service, ContinuousServiceTableSync>
		{
		public:
			static const std::string COL_SERVICENUMBER;
			static const std::string COL_SCHEDULES;
			static const std::string COL_PATHID;
			static const std::string COL_RANGE;
			static const std::string COL_MAXWAITINGTIME;
			static const std::string COL_PEDESTRIAN_USE_RULE;
			static const std::string COL_HANDICAPPED_USE_RULE;
			static const std::string COL_BIKE_USE_RULE;
			static const std::string COL_DATES;
			


			/** ContinuousService search.
				@param lineId JourneyPattern
				@param first First ContinuousService object to answer
				@param number Number of ContinuousService objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found ContinuousService objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> lineId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> commercialLineId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByDepartureTime = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
}	}

#endif // SYNTHESE_ContinuousServiceTableSync_H__
