
#ifndef SYNTHESE_AccountTableSync_H__
#define SYNTHESE_AccountTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace accounts
	{
		class Account;

		/** Account SQLite table synchronizer.
			@ingroup m57
		*/

		class AccountTableSync : public db::SQLiteTableSyncTemplate<Account>
		{
		public:
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_LEFT_USER_ID;
			static const std::string TABLE_COL_LEFT_NUMBER;
			static const std::string TABLE_COL_LEFT_CLASS_NUMBER;
			static const std::string TABLE_COL_LEFT_CURRENCY_ID;
			static const std::string TABLE_COL_RIGHT_USER_ID;
			static const std::string TABLE_COL_RIGHT_NUMBER;
			static const std::string TABLE_COL_RIGHT_CLASS_NUMBER;
			static const std::string TABLE_COL_RIGHT_CURRENCY_ID;

			AccountTableSync();
			~AccountTableSync ();

			/** Account search.
				@param sqlite SQLite thread
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Account*> Founded accounts. 
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<Account*> searchAccounts(const db::SQLiteQueueThreadExec* sqlite
				, uid rightUserId, const std::string& rightClassNumber, uid leftUserId, const std::string& leftClassNumber
				, const std::string name=""
				, int first = 0, int number = 0);
			

		protected:

			/** Action to do on user creation.
			No action because the users are not permanently loaded in ram.
			*/
			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

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

#endif // SYNTHESE_AccountTableSync_H__

