
/** DBLogEntryTableSync class header.
	@file DBLogEntryTableSync.h

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

#ifndef SYNTHESE_DBLogEntryTableSync_H__
#define SYNTHESE_DBLogEntryTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace dblog
	{
		class DBLog;

		/** 45 DBLog table synchronizer.
			@ingroup m13LS refLS
		*/
		class DBLogEntryTableSync : public db::SQLiteTableSyncTemplate<DBLogEntry>
		{
		public:
			static const std::string CONTENT_SEPARATOR;
			static const std::string COL_LOG_KEY;
			static const std::string COL_DATE;
			static const std::string COL_USER_ID;
			static const std::string COL_LEVEL;
			static const std::string COL_CONTENT;
			static const std::string COL_OBJECT_ID;

			DBLogEntryTableSync();


			/** DBLog search.
				(other search parameters)
				@param first First DBLog object to answer
				@param number Number of DBLog objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<DBLogEntry*> Founded DBLog objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<DBLogEntry> > search(
				const std::string& logKey
				, const time::DateTime& startDate
				, const time::DateTime& endDate
				, const boost::shared_ptr<const security::User> user
				, DBLogEntry::Level level
				, uid id
				, const std::string& text
				, int first = 0
				, int number = 0
				, bool orderByDate = true
				, bool orderByUser = false
				, bool orderByLevel = false
				, bool raisingOrder = true
				);


		protected:

			/** Action to do on DBLog creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows, bool isFirstSync = false);

			/** Action to do on DBLog creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on DBLog deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};
	}
}

#endif // SYNTHESE_DBLogEntryTableSync_H__
