
/** AlarmTableSync class implementation.
	@file AlarmTableSync.cpp

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

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "04_time/DateTime.h"

#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesModule.h"

#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::time::DateTime;

namespace synthese
{
	using namespace db;
	using namespace messages;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Alarm>::TABLE_NAME = "t003_alarms";
		template<> const int SQLiteTableSyncTemplate<Alarm>::TABLE_ID = 3;
		template<> const bool SQLiteTableSyncTemplate<Alarm>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Alarm>::load(Alarm* alarm, const SQLiteResult& rows, int rowId)
		{
			alarm->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			alarm->setMessage(rows.getColumn (rowId, AlarmTableSync::TABLE_COL_MESSAGE));
			alarm->setPeriodStart(DateTime::FromSQLTimestamp (rows.getColumn (rowId, AlarmTableSync::TABLE_COL_PERIODSTART)));
			alarm->setPeriodEnd(DateTime::FromSQLTimestamp (rows.getColumn (rowId, AlarmTableSync::TABLE_COL_PERIODEND)));
			alarm->setLevel((Alarm::AlarmLevel) Conversion::ToInt (rows.getColumn (rowId, AlarmTableSync::TABLE_COL_LEVEL)));
		}

	}


	namespace messages
	{
		const std::string AlarmTableSync::TABLE_COL_MESSAGE = "message"; 
		const std::string AlarmTableSync::TABLE_COL_PERIODSTART = "period_start";
		const std::string AlarmTableSync::TABLE_COL_PERIODEND = "period_end"; 
		const std::string AlarmTableSync::TABLE_COL_LEVEL = "level";



		AlarmTableSync::AlarmTableSync ()
		: SQLiteTableSyncTemplate<Alarm>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (TABLE_COL_MESSAGE, "TEXT", true);
			addTableColumn (TABLE_COL_PERIODSTART, "TIMESTAMP", true);
			addTableColumn (TABLE_COL_PERIODEND, "TIMESTAMP", true);
			addTableColumn (TABLE_COL_LEVEL, "INTEGER", true);
		}


		AlarmTableSync::~AlarmTableSync ()
		{

		}
		    

		void AlarmTableSync::rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows(); ++rowIndex)
			{
				uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
				if (MessagesModule::getAlarms ().contains (id)) continue;

				Alarm* alarm = new Alarm (id);

				load(alarm, rows, rowIndex);

				MessagesModule::getAlarms().add (alarm);
			}

		}


		void AlarmTableSync::rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows(); ++rowIndex)
			{
				uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
				Alarm* alarm = MessagesModule::getAlarms ().get (id);
				load(alarm, rows, rowIndex);
			}
		}


		void AlarmTableSync::rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows(); ++rowIndex)
			{
				uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
				MessagesModule::getAlarms ().remove (id);	/// @todo Not so simple.
			}
		}

	}
}
