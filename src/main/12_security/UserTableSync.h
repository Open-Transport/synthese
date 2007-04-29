
/** UserTableSync class header.
	@file UserTableSync.h

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

#ifndef SYNTHESE_UserTableSync_H__
#define SYNTHESE_UserTableSync_H__

#include <boost/logic/tribool.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;
		class Profile;

		/** User SQLite table synchronizer.
			@ingroup m12LS refLS
			@todo Update the opened session on user update
		*/

		class UserTableSync : public db::SQLiteTableSyncTemplate<User>
		{
		public:
			static const std::string TABLE_COL_LOGIN;
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_SURNAME;
			static const std::string TABLE_COL_PASSWORD;
			static const std::string TABLE_COL_PROFILE_ID;
			static const std::string TABLE_COL_ADDRESS;
			static const std::string TABLE_COL_POST_CODE;
			static const std::string TABLE_COL_CITY_TEXT;
			static const std::string TABLE_COL_CITY_ID;
			static const std::string TABLE_COL_COUNTRY;
			static const std::string TABLE_COL_EMAIL;
			static const std::string TABLE_COL_PHONE;
			static const std::string COL_LOGIN_AUTHORIZED;
			static const std::string COL_BIRTH_DATE;


			UserTableSync();

			static boost::shared_ptr<User> getUserFromLogin(const std::string& login);
			static bool loginExists(const std::string& login);

			/** User search.
				@param login login to search (empty = no search on login)
				@param name name to search (empty = no search on name)
				@param profile Profile which user must belong (null = filter deactivated)
				@param emptyLogin User without login acceptation (true = user must have a login, false = user must not have a login, indeterminate = filter deactivated)
				@param first First user to answer
				@param number Number of users to answer (-1 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others users to show. Test it to know if the situation needs a "click for more" button.
				@param orderByLogin Order the results by login
				@param orderByName Order the results by name and surname
				@param orderByProfile Order the results by profile name
				@param raisingOrder True = Ascendant order, false = descendant order
				@return vector<share_ptr<User>> Founded users. 
				@warning only one of the orderBy parameters must be true, or no one. More of one true value will throw an exception.
				@throw UserTableSyncException If the query has failed (does not occurs in normal case)
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<boost::shared_ptr<User> > search(
				const std::string& login
				, const std::string name
				, boost::shared_ptr<const security::Profile> profile = boost::shared_ptr<const security::Profile>()
				, boost::logic::tribool emptyLogin = boost::logic::indeterminate
				, int first = 0
				, int number = -1
				, bool orderByLogin = true
				, bool orderByName = false
				, bool orderByProfileName = false
				, bool raisingOrder = true
				);

		protected:

			/** Action to do on user creation.
				No action because the users are not permanently loaded in ram.
			*/
			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows, bool isFirstSync = false);

			/** Action to do on user creation.
				Updates the users objects in the opened sessions.
			*/
			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on user deletion.
				Closes the sessions of the deleted user.
			*/
			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}
#endif // SYNTHESE_UserTableSync_H__

