
/** AlarmObjectLinkTableSync class header.
	@file AlarmObjectLinkTableSync.h

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

#ifndef SYNTHESE_AlarmObjectLinkTableSync_H__
#define SYNTHESE_AlarmObjectLinkTableSync_H__


#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "01_util/Conversion.h"
#include "01_util/Registrable.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"
#include "02_db/SQLiteTableSyncTemplate.h"

#include "17_messages/AlarmObjectLink.h"
#include "17_messages/MessagesModule.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;

		/**  table synchronizer.
			@ingroup m17
		*/
		class AlarmObjectLinkTableSync : public db::SQLiteTableSyncTemplate<AlarmObjectLink<Registrable<uid, void> > >
		{
		public:
			static const std::string COL_RECIPIENT_KEY;
			static const std::string COL_OBJECT_ID;
			static const std::string COL_ALARM_ID;

			AlarmObjectLinkTableSync();
			~AlarmObjectLinkTableSync();


			/**  search.
				(other search parameters)
				@param first First  object to answer
				@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<*> Founded  objects.
				@author Hugues Romain
				@date 2006
			*/
			template<class T>
			static std::vector< AlarmObjectLink<T> > search(
				const Alarm* alarm,
				const std::string& recipientKey,
				int first = 0, int number = 0);


		protected:

			/** Action to do on  creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on  creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on  deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

		template<class T>
			std::vector< AlarmObjectLink<T> > AlarmObjectLinkTableSync::search(const Alarm* alarm, const std::string& recipientKey, int first /*= 0*/, int number /*= 0*/ )
		{
			const db::SQLiteQueueThreadExec* sqlite = db::DBModule::GetSQLite();
			std::stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				<< AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << util::Conversion::ToString(alarm->getKey())
				<< " AND " << AlarmObjectLinkTableSync::COL_RECIPIENT_KEY << "=" << util::Conversion::ToSQLiteString(recipientKey);
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				db::SQLiteResult result = sqlite->execQuery(query.str());
				std::vector< AlarmObjectLink<T> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					AlarmObjectLink<T> object;
					object.setKey(util::Conversion::ToLongLong(result.getColumn(i, db::TABLE_COL_ID)));
					object.setObject(db::SQLiteTableSyncTemplate<T>::get(util::Conversion::ToLongLong(result.getColumn(i, COL_OBJECT_ID))));
					object.setAlarm(messages::MessagesModule::getAlarms().get(util::Conversion::ToLongLong(result.getColumn(i, COL_ALARM_ID))));
					objects.push_back(object);
				}
				return objects;
			}
			catch(db::SQLiteException& e)
			{
				throw util::Exception(e.getMessage());
			}
		}

	}
}

#endif // SYNTHESE_AlarmObjectLinkTableSync_H__
