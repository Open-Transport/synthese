
/** CallStatisticsTableSync class header.
	@file CallStatisticsTableSync.h

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

#ifndef SYNTHESE_resa_CallStatisticsTableSync_h__
#define SYNTHESE_resa_CallStatisticsTableSync_h__

#include "ResaDBLog.h"

#include <map>
#include <string>

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace resa
	{
		/** CallStatisticsTableSync class.
			@ingroup m51
		*/
		class CallStatisticsTableSync
		{
		public:
			typedef std::map<
				ResaDBLog::_EntryType,
				std::map<
					std::string,
					std::size_t
			>	> CallCountSearchResult;

			enum Step
			{
				HOUR_STEP,
				DAY_STEP,
				WEEK_DAY_STEP,
				WEEK_STEP,
				MONTH_STEP,
				YEAR_STEP
			};

			static CallCountSearchResult CountCalls(
				boost::gregorian::date_period period,
				Step step,
				boost::logic::tribool autoresa = boost::logic::indeterminate
			);

			static std::string GetGroupBy(Step step);
		};
	}
}

#endif // SYNTHESE_resa_CallStatisticsTableSync_h__
