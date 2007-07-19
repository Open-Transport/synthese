
/** ProfileTableSync class header.
	@file ProfileTableSync.h

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

#ifndef SYNTHESE_ProfileTableSync_H__
#define SYNTHESE_ProfileTableSync_H__

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Profile SQLite table synchronizer.

			The storage of rights is :
				- rights separated by |
				- parameters of rights separated by ,
			Eg: *,*,10,10|env,*,40,40

			@ingroup m12LS refLS
		*/
		class ProfileTableSync : public db::SQLiteTableSyncTemplate<Profile>
		{

		public:
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_PARENT_ID;
			static const std::string TABLE_COL_RIGHTS_STRING;

			static const std::string RIGHT_SEPARATOR;
			static const std::string RIGHT_VALUE_SEPARATOR;

			static std::string getRightsString(const Profile*);
			static void setRightsFromString(Profile*, const std::string& text);

			ProfileTableSync();

			/** Profile search.
				@param name name to search (empty = no search on name)
				@param right right to search
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others users to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Profile*> Founded users. 
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<boost::shared_ptr<Profile> > search(
				std::string name = std::string()
				, std::string exactName = std::string()
				, std::string right = std::string()
				, int first = 0
				, int number = UNKNOWN_VALUE
				, bool orderByName = true
				, bool raisingOrder = true
				);

			/** Profile search by parent.
			*/
			static std::vector<boost::shared_ptr<Profile> > search(
				boost::shared_ptr<const Profile> parent
				, int first = 0, int number = -1);

		protected:

			/** Action to do on user creation.
			No action because the users are not permanently loaded in ram.
			*/
			void rowsAdded (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows, bool isFirstSync = false);

			/** Action to do on user creation.
			Updates the users objects in the opened sessions.
			*/
			void rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on user deletion.
			Closes the sessions of the deleted user.
			*/
			void rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}
#endif // SYNTHESE_ProfileTableSync_H__

