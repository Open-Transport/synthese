
/** Resa StatisticsTableSync class header.
	@file ResaStatisticsTableSync.h

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

#ifndef SYNTHESE_resa_ResaStatisticsTableSync_h__
#define SYNTHESE_resa_ResaStatisticsTableSync_h__

#include "Registry.h"

#include <map>
#include <string>

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace db
	{
		class DB;
	}

	namespace resa
	{
		/** Reservation StatisticsTableSync class.
			@ingroup m51
		*/
		class ResaStatisticsTableSync
		{
		public:
			typedef std::map<
				std::string,
				std::map<
					std::string,
					std::size_t
			>	> ResaCountSearchResult;

			enum Step
			{
				NO_STEP = 0,
				SERVICE_STEP,
				HOUR_STEP,
				DATE_STEP,
				WEEK_DAY_STEP,
				WEEK_STEP,
				MONTH_STEP,
				YEAR_STEP,
				DEPARTURE_STOP_STEP,
				DEPARTURE_CITY_STEP,
				ARRIVAL_STOP_STEP,
				ARRIVAL_CITY_STEP,
				RESERVATION_DELAY_10_MIN_STEP,
				RESERVATION_DELAY_30_MIN_STEP,
				RESERVATION_DELAY_60_MIN_STEP
			};

			static ResaCountSearchResult CountCalls(
				boost::gregorian::date_period period,
				Step rowStep,
				Step colStep,
				boost::optional<util::RegistryKeyType> lineFilter,
				boost::logic::tribool cancelledFilter = false
			);

		private:

			static std::string GetSQLColumnOrGroupBy(synthese::db::DB* db, Step step, bool column);
		};
	}
}

#endif // SYNTHESE_resa_CallStatisticsTableSync_h__
