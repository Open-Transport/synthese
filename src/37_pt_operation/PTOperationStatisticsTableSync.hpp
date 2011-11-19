
/** PTOperationStatisticsTableSync class header.
	@file PTOperationStatisticsTableSync.hpp

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

#ifndef SYNTHESE_pt_operation_PTOperationStatisticsTableSync_hpp__
#define SYNTHESE_pt_operation_PTOperationStatisticsTableSync_hpp__

#include <boost/date_time/gregorian/gregorian_types.hpp>

#include <map>
#include <string>

namespace synthese
{
	namespace db
	{
		class DB;
	}

	namespace pt_operation
	{
		/** PTOperationStatisticsTableSync class.
			@ingroup m37
		*/
		class PTOperationStatisticsTableSync
		{
		public:
			typedef std::map<
				std::string,
				std::map<
					std::string,
					int
			>	> CountSearchResult;

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
				LINE_STEP,
				VEHICLE_STEP
			};

			enum What
			{
				KM
			};

			static CountSearchResult Count(
				boost::gregorian::date_period period,
				Step rowStep,
				Step colStep,
				What what
			);

		private:

			static std::string GetSQLColumnOrGroupBy(synthese::db::DB* db, Step step, bool column);
			static std::string GetSQLWhat(What what);
		};
	}
}

#endif // SYNTHESE_pt_operation_PTOperationStatisticsTableSync_hpp__
