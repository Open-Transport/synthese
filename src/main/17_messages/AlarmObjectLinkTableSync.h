
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

		/** Alarm object links table synchronizer.
			@ingroup m17
			
			Only the links concerning the sent alarms are loaded in ram.
		*/
		class AlarmObjectLinkTableSync : public db::SQLiteTableSyncTemplate<AlarmObjectLink>
		{
		public:
			static const std::string COL_RECIPIENT_KEY;
			static const std::string COL_OBJECT_ID;
			static const std::string COL_ALARM_ID;

			AlarmObjectLinkTableSync();

			/** Search of alarm object links for a specified alarm in a specified recipient type.
				@param alarm Alarm to the object must be liked with
				@param recipientKey Key of the recipient to search (REMOVE THIS PARAMETER WHEN THE FACTORY KEY WILL BE STATIC)
				@param first First  object to answer
				@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<*> Founded  objects.
				@author Hugues Romain
				@date 2006
			*/
			template<class T>
			static std::vector<boost::shared_ptr<T> > search(
				const Alarm* alarm,
				const std::string& recipientKey,
				int first = 0, int number = 0);

			/** Search of alarm object links for a specified alarm in all the recipient types.
				@param alarm Alarm to the object must be liked with
				@param first First  object to answer
				@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<*> Founded  objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<AlarmObjectLink> > search(
				const Alarm* alarm,
				int first = 0, int number = 0);

			/** Remove a link between an alarm and an object specified by their id.
				@param alarmId ID of the alarm
				@param objectId ID of the object
				@author Hugues Romain
				@date 2007				
			*/
			static void remove(uid alarmId, uid objectId);

		protected:

			/** Action to do on  creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows, bool isFirstSync = false);

			/** Action to do on  creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on  deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

		template<class T>
		std::vector< boost::shared_ptr<T> > AlarmObjectLinkTableSync::search(const Alarm* alarm, const std::string& recipientKey, int first /*= 0*/, int number /*= 0*/ )
		{
			std::stringstream query;
			query
				<< " SELECT "
					<< AlarmObjectLinkTableSync::COL_OBJECT_ID
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
					<< AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << util::Conversion::ToString(alarm->getId())
					<< " AND " << AlarmObjectLinkTableSync::COL_RECIPIENT_KEY << "=" << util::Conversion::ToSQLiteString(recipientKey);
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				db::SQLiteResult result = db::DBModule::GetSQLite()->execQuery(query.str());
				std::vector< boost::shared_ptr<T> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					objects.push_back(db::SQLiteTableSyncTemplate<T>::get(util::Conversion::ToLongLong(result.getColumn(i, COL_OBJECT_ID))));
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
