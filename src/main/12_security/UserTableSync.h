
#ifndef SYNTHESE_UserTableSync_H__
#define SYNTHESE_UserTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;

		/** User SQLite table synchronizer.
			@ingroup m12
		*/

		class UserTableSync : public db::SQLiteTableSyncTemplate<User>
		{
		public:
			static const std::string TABLE_COL_ID;
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


			UserTableSync();
			~UserTableSync ();

			static User* getUser(const db::SQLiteThreadExec* sqlite, const std::string& login);

			/** User search.
				@param sqlite SQLite thread
				@param login login to search (empty = no search on login)
				@param name name to search (empty = no search on name)
				@param first First user to answer
				@param number Number of users to answer (-1 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others users to show. Test it to know if the situation needs a "click for more" button.
				@return vector<User*> Founded users. 
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<User*> searchUsers(const db::SQLiteThreadExec* sqlite
				, const std::string& login, const std::string name, uid profileId = 0
				, int first = 0, int number = -1);

		protected:

			/** Action to do on user creation.
				No action because the users are not permanently loaded in ram.
			*/
			void rowsAdded (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on user creation.
				Updates the users objects in the opened sessions.
			*/
			void rowsUpdated (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on user deletion.
				Closes the sessions of the deleted user.
			*/
			void rowsRemoved (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}
#endif // SYNTHESE_UserTableSync_H__

