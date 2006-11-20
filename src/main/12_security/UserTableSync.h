
#ifndef SYNTHESE_UserTableSync_H__
#define SYNTHESE_UserTableSync_H__


#include <string>
#include <iostream>

#include "02_db/SQLiteTableSync.h"

namespace synthese
{
	namespace security
	{
		class User;

		/** User SQLite table synchronizer.
			@ingroup m12
		*/

		class UserTableSync : public db::SQLiteTableSync
		{
		private:
			static const std::string TABLE_NAME;
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_LOGIN;
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_SURNAME;
			static const std::string TABLE_COL_PASSWORD;
			static const std::string TABLE_COL_PROFILE_ID;

		public:

			UserTableSync();
			~UserTableSync ();

			static db::SQLiteResult fetchUser(uid);
			static User* getUser(const db::SQLiteResult& rows);
			static void loadUser(const db::SQLiteResult& rows, User* user);

		protected:

			/** Action to do on user creation.
				No action because the users are not listed in ram.
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

			void rowsRemoved (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}
#endif // SYNTHESE_UserTableSync_H__
