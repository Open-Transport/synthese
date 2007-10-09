/** AlarmTableSync class header.
	@file AlarmTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_ALARMTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ALARMTABLESYNC_H

#include "02_db/SQLiteTableSyncTemplate.h"

#include "04_time/DateTime.h"

#include "17_messages/Alarm.h"
#include "17_messages/Types.h"

#include <string>

namespace synthese
{
	namespace messages
	{
		class ScenarioSentAlarm;
		class SingleSentAlarm;
		class AlarmTemplate;
		class SentScenario;
		class ScenarioTemplate;
		class Scenario;


		/** Alarm SQLite table synchronizer.
			@ingroup m17LS refLS
			@warning (for the future sqlite node synchronizer) The alarm table sync insertion hook must always be run after the one for the scenario

			Only the sent alarms are loaded in ram.

			@note As Alarm is an abstract class, do not use the get static method. Use getAlarm instead.
		*/
		class AlarmTableSync : public db::SQLiteTableSyncTemplate<AlarmTableSync,Alarm>
		{
		private:
			static const std::string _COL_CONFLICT_LEVEL;
			static const std::string _COL_RECIPIENTS_NUMBER;

		public:
			static const std::string COL_LEVEL;
			static const std::string COL_IS_TEMPLATE;
			static const std::string COL_SHORT_MESSAGE;
			static const std::string COL_LONG_MESSAGE;
			static const std::string COL_PERIODSTART;
			static const std::string COL_PERIODEND;
			static const std::string COL_SCENARIO_ID;
			static const std::string COL_ENABLED;
			
			AlarmTableSync ();
			~AlarmTableSync ();

		protected:


			/** Action to do on alarm creation.
				
			*/
			void rowsAdded (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows, bool isFirstSync = false);

			/** Action to do on alarm update.
			
			*/
			void rowsUpdated (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

			/** Action to do on alarm deletion.
			
			*/
			void rowsRemoved (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

		public:
/*			static std::vector<boost::shared_ptr<Alarm> > search(
				const Scenario* scenario
				, int first = 0
				, int number = -1
				, bool orderByLevel = false
				, bool raisingOrder = false
				);
*/
			static std::vector<boost::shared_ptr<ScenarioSentAlarm> > searchScenarioSent(
				const SentScenario* scenario
				, int first = 0
				, int number = 0
				, bool orderByLevel = false
				, bool orderByStatus = false
				, bool orderByConflict = false
				, bool raisingOrder = false
				);
			
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
			static std::vector<boost::shared_ptr<SingleSentAlarm> > searchSingleSent(
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
				, bool raisingOrder = false
				);
			
			static std::vector<boost::shared_ptr<AlarmTemplate> > searchTemplates(
				const ScenarioTemplate* scenario
				, int first = 0
				, int number = 0
				, bool orderByLevel = false
				, bool raisingOrder = false
				);

			static boost::shared_ptr<Alarm> getAlarm(uid key);
			static boost::shared_ptr<SingleSentAlarm> getSingleSentAlarm(uid key);
		};

	}
}

#endif
