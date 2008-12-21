
/** ScheduledServiceTableSync class header.
	@file ScheduledServiceTableSync.h

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

#ifndef SYNTHESE_ScheduledServiceTableSync_H__
#define SYNTHESE_ScheduledServiceTableSync_H__

#include "ScheduledService.h"

#include <vector>
#include <string>
#include <iostream>

#include "Date.h"

#include "SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		/** ScheduledService table synchronizer.
			@ingroup m35LS refLS
		*/
		class ScheduledServiceTableSync : public db::SQLiteRegistryTableSyncTemplate<ScheduledServiceTableSync,ScheduledService>
		{
		public:
			static const std::string COL_SERVICENUMBER;
			static const std::string COL_SCHEDULES;
			static const std::string COL_PATHID;
			static const std::string COL_RANKINPATH;
			static const std::string COL_BIKECOMPLIANCEID;
			static const std::string COL_HANDICAPPEDCOMPLIANCEID;
			static const std::string COL_PEDESTRIANCOMPLIANCEID;
			static const std::string COL_RESERVATIONRULEID;
			static const std::string COL_TEAM;
			
			ScheduledServiceTableSync();


			/** ScheduledService search.
				@param line Line which the service must belong to
				@param commercialLine Commercial line which the service must belong to
				@param first First ScheduledService object to answer
				@param number Number of ScheduledService objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@param orderByOriginTime order chronologically by origin time
				@param raisingOrder true = ascending order, false = descending order
				@return vector<ScheduledService*> Founded ScheduledService objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				uid lineId = UNKNOWN_VALUE
				, uid commercialLineId = UNKNOWN_VALUE
				, time::Date date = time::Date(time::TIME_UNKNOWN)
				, int first = 0
				, int number = 0
				, bool orderByOriginTime = true
				, bool raisingOrder = true,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_ScheduledServiceTableSync_H__
