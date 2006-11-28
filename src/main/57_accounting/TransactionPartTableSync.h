
#ifndef SYNTHESE_TransationPartTableSync_H__
#define SYNTHESE_TransationPartTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace accounts
	{
		class TransactionPart;

		/** Transaction part SQLite table synchronizer.
			@ingroup m57
		*/

		class TransactionPartTableSync : public db::SQLiteTableSyncTemplate<TransactionPart>
		{
		private:
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_LEFT_CURRENCY_AMOUNT;
			static const std::string TABLE_COL_RIGHT_CURRENCY_AMOUNT;
			static const std::string TABLE_COL_ACCOUNT_ID;
			static const std::string TABLE_COL_RATE_ID;
			static const std::string TABLE_COL_TRADED_OBJECT_ID;
			static const std::string TABLE_COL_COMMENT;

			static void loadTransactionPart(TransactionPart* tp, const db::SQLiteResult& rows, int rowId=0);

		public:

			TransactionPartTableSync();
			~TransactionPartTableSync ();

			//static TransactionPart* getAccount(const db::SQLiteThreadExec* sqlite, uid id);
			static void saveTransactionPart(const db::SQLiteThreadExec* sqlite, TransactionPart* tp);

			/** TransactionPart search.
				@param sqlite SQLite thread
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Account*> Founded accounts. 
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<TransactionPart*> searchTransactionParts(const db::SQLiteThreadExec* sqlite
				
				, int first = 0, int number = 0);


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
