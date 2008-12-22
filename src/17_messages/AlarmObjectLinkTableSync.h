
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

#include "Conversion.h"
#include "Registrable.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "SQLiteDirectTableSyncTemplate.h"

#include "AlarmObjectLink.h"
#include "Alarm.h"
#include "MessagesModule.h"

namespace synthese
{
	namespace messages
	{
		/** Alarm object links table synchronizer.
			@ingroup m17LS refLS
			
			Only the links concerning the sent alarms are loaded in ram.
		*/
		class AlarmObjectLinkTableSync : public db::SQLiteDirectTableSyncTemplate<AlarmObjectLinkTableSync,AlarmObjectLink>
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
			template<class K, class T>
			static std::vector<boost::shared_ptr<T> > search(
				util::Env& env,
				const Alarm* alarm,
				const std::string& recipientKey,
				int first = 0,
				int number = 0,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			/** Search of alarm object links for a specified alarm in all the recipient types.
				@param alarm Alarm to the object must be liked with
				@param first First  object to answer
				@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<*> Founded  objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				const Alarm* alarm,
				int first = 0,
				int number = 0,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			/** Remove a link between an alarm and an object specified by their id.
				@param alarmId ID of the alarm
				@param objectId ID of the object. UNKNOWN_VALUE = Removes all links of the alarm
				@author Hugues Romain
				@date 2007				
			*/
			static void Remove(uid alarmId, uid objectId = UNKNOWN_VALUE);


			/** Action to do on  creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows, bool isFirstSync = false);

			/** Action to do on  creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

			/** Action to do on  deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

		};

		template<class K, class T>
		std::vector< boost::shared_ptr<T> > AlarmObjectLinkTableSync::search(
			util::Env& env,
			const Alarm* alarm,
			const std::string& recipientKey,
			int first /*= 0*/,
			int number /*= 0*/,
			util::LinkLevel linkLevel
		){
			std::stringstream query;
			query
				<< " SELECT "
					<< AlarmObjectLinkTableSync::COL_OBJECT_ID
				<< " FROM " << TABLE.NAME
				<< " WHERE " 
					<< AlarmObjectLinkTableSync::COL_ALARM_ID << "=" << util::Conversion::ToString(alarm->getKey())
					<< " AND " << AlarmObjectLinkTableSync::COL_RECIPIENT_KEY << "=" << util::Conversion::ToSQLiteString(recipientKey);
			if (number > 0)
			    query << " LIMIT " << util::Conversion::ToString(number + 1);
			if (first > 0)
			    query << " OFFSET " << util::Conversion::ToString(first);

			try
			{
				db::SQLiteResultSPtr rows = db::DBModule::GetSQLite()->execQuery(query.str());
				std::vector< boost::shared_ptr<T> > objects;
				while (rows->next ())
				{
				    objects.push_back(
						db::SQLiteDirectTableSyncTemplate<K,T>::GetEditable(
						   rows->getLongLong (COL_OBJECT_ID),
						   env,
						   linkLevel
					)	);
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
