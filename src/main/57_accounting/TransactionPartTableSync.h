
#ifndef SYNTHESE_TransationPartTableSync_H__
#define SYNTHESE_TransationPartTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

#include "04_time/DateTime.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace accounts
	{
		class Account;
		class TransactionPart;

		/** Transaction part SQLite table synchronizer.
			@ingroup m57
		*/

		class TransactionPartTableSync : public db::SQLiteTableSyncTemplate<TransactionPart>
		{
		public:
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_TRANSACTION_ID;
			static const std::string TABLE_COL_LEFT_CURRENCY_AMOUNT;
			static const std::string TABLE_COL_RIGHT_CURRENCY_AMOUNT;
			static const std::string TABLE_COL_ACCOUNT_ID;
			static const std::string TABLE_COL_RATE_ID;
			static const std::string TABLE_COL_TRADED_OBJECT_ID;
			static const std::string TABLE_COL_COMMENT;

			TransactionPartTableSync();
			~TransactionPartTableSync ();


			/** TransactionPart search.
				@param sqlite SQLite thread
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Account*> Founded accounts. 
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<TransactionPart*> searchTransactionParts(const db::SQLiteThreadExec* sqlite
				, Account* account, security::User* user
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

#endif // SYNTHESE_TransationPartTableSync_H__
