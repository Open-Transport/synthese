
/** TransactionPartTableSync class header.
	@file TransactionPartTableSync.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#ifndef SYNTHESE_TransationPartTableSync_H__
#define SYNTHESE_TransationPartTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

#include "04_time/DateTime.h"

namespace synthese
{
	namespace time
	{
		class Date;
	}

	namespace security
	{
		class User;
	}

	namespace accounts
	{
		class Account;
		class TransactionPart;
		class Transaction;

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
			static std::vector<TransactionPart*> search(
				Account* account, security::User* user
				, int first = 0, int number = -1);

			/** TransactionPart search.
				@param sqlite SQLite thread
				@param transaction Transaction which belong the part
				@param account Account of the part (NULL = all parts)
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Account*> Founded accounts. 
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<TransactionPart*> search(
				const Transaction* transaction, const Account* account=NULL
				, int first = 0, int number = -1);

			static std::map<int, int> TransactionPartTableSync::count(Account* account, time::Date startDate, time::Date endDate, int first=0, int number=-1);

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

#endif // SYNTHESE_TransationPartTableSync_H__
