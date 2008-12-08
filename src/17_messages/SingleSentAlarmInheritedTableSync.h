
/** SingleSentAlarmInheritedTableSync class header.
	@file SingleSentAlarmInheritedTableSync.h

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

#ifndef SYNTHESE_messages_SingleSentAlarmInheritedTableSync_h__
#define SYNTHESE_messages_SingleSentAlarmInheritedTableSync_h__

#include "02_db/SQLiteInheritedRegistryTableSync.h"

#include "AlarmTableSync.h"
#include "SingleSentAlarm.h"

namespace synthese
{
	namespace messages
	{
		/** SingleSentAlarmInheritedTableSync class.
			@ingroup m17ILS refILS
		*/
		class SingleSentAlarmInheritedTableSync
			: public db::SQLiteInheritedRegistryTableSync<AlarmTableSync, SingleSentAlarmInheritedTableSync, SingleSentAlarm>
		{
		public:
			/** Constructor.
			*/
			SingleSentAlarmInheritedTableSync();

			/** searchSingleSent.
				@param startDate
				@param endDate
				@param first
				@param number
				@param orderByDate
				@param orderByLevel
				@param orderByStatus
				@param orderByConflict
				@param raisingOrder
				@return std::vector<boost::shared_ptr<SingleSentAlarm> >
				@author Hugues Romain
				@date 2007
			
			Target query :
			@code
SELECT
	al1.*
	, (SELECT COUNT(object_id) FROM t040_alarm_object_links AS aol3 WHERE aol3.alarm_id=al1.id)
	, (SELECT MAX(al2.level) FROM t040_alarm_object_links AS aol1 INNER JOIN t040_alarm_object_links AS aol2 ON aol1.object_id=aol2.object_id AND aol1.alarm_id != aol2.alarm_id INNER JOIN t003_alarms AS al2 ON al2.id = aol2.alarm_id WHERE aol1.alarm_id=al1.id AND  (al2.period_start IS NULL OR al1.period_end IS NULL OR al2.period_start < al1.period_end) AND (al2.period_end IS NULL OR al1.period_start IS NULL OR al2.period_end > al1.period_start)) AS conflict_level
FROM
	t003_alarms AS al1
WHERE
	is_template=0
			@endcode
			*/
			static void Search(
				util::Env& env,
				time::DateTime startDate
				, time::DateTime endDate
				, AlarmConflict conflict
				, AlarmLevel level
				, int first = 0
				, int number = 0
				, bool orderByDate = true
				, bool orderByLevel = false
				, bool orderByStatus = false
				, bool orderByConflict = false
				, bool raisingOrder = false,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_messages_SingleSentAlarmInheritedTableSync_h__
