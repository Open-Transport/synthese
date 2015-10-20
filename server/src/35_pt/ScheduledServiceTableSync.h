
/** ScheduledServiceTableSync class header.
	@file ScheduledServiceTableSync.h

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

#ifndef SYNTHESE_ScheduledServiceTableSync_H__
#define SYNTHESE_ScheduledServiceTableSync_H__

#include "ScheduledService.h"

#include <vector>
#include <string>
#include <iostream>
#include <boost/optional.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "DBDirectTableSyncTemplate.hpp"
#include "FetcherTemplate.h"
#include "Service.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.10 Table : Scheduled service.
		///	@ingroup m35LS refLS
		///
		class ScheduledServiceTableSync:
			public db::DBDirectTableSyncTemplate<
				ScheduledServiceTableSync,
				ScheduledService,
				db::FullSynchronizationPolicy
			>,
			public db::FetcherTemplate<SchedulesBasedService, ScheduledServiceTableSync>,
			public db::FetcherTemplate<calendar::Calendar, ScheduledServiceTableSync>
		{
		public:
			/** ScheduledService search.
				@param line JourneyPattern which the service must belong to
				@param commercialLine Commercial line which the service must belong to
				@param dataSource Filter on the data source of the route
				@param first First ScheduledService object to answer
				@param number Number of ScheduledService objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@param orderByOriginTime order chronologically by origin time
				@param raisingOrder true = ascending order, false = descending order
				@return Found ScheduledService objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> lineId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> commercialLineId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> dataSource = boost::optional<util::RegistryKeyType>(),
				boost::optional<std::string> serviceNumber = boost::optional<std::string>(),
				bool hideOldServices = false,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByOriginTime = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			virtual bool allowList( const server::Session* session ) const;
		};
}	}

#endif // SYNTHESE_ScheduledServiceTableSync_H__
