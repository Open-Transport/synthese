
/** TransactionPartTableSync class header.
	@file TransactionPartTableSync.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DBNoSyncTableSyncTemplate.hpp"

#include "DateTime.h"

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

		/** Transaction part table synchronizer.
			@ingroup m57LS refLS
		*/
		class TransactionPartTableSync : public db::DBNoSyncTableSyncTemplate<TransactionPartTableSync,TransactionPart>
		{
		public:
			static const std::string TABLE_COL_TRANSACTION_ID;
			static const std::string TABLE_COL_LEFT_CURRENCY_AMOUNT;
			static const std::string TABLE_COL_RIGHT_CURRENCY_AMOUNT;
			static const std::string TABLE_COL_ACCOUNT_ID;
			static const std::string TABLE_COL_RATE_ID;
			static const std::string TABLE_COL_TRADED_OBJECT_ID;
			static const std::string TABLE_COL_COMMENT;
			static const std::string COL_STOCK_ID;

			TransactionPartTableSync();


			/** TransactionPart search.
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Account*> Founded accounts.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<TransactionPart> > search(
				boost::shared_ptr<const Account> account
				, boost::shared_ptr<const security::User> user
				, bool order=false
				, int first = 0, int number = -1);

			/** TransactionPart search.
				@param transaction Transaction which belong the part
				@param account Account of the part (NULL = all parts)
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Account*> Founded accounts.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<TransactionPart> > search(
				boost::shared_ptr<const Transaction> transaction
				, boost::shared_ptr<const Account> account=boost::shared_ptr<const Account>()
				, int first = 0, int number = -1);

			static std::vector<boost::shared_ptr<TransactionPart> > Search(
				std::string accountCode
				, uid rightUserId = UNKNOWN_VALUE
				, uid stockId = UNKNOWN_VALUE
				, bool orderByAccount = false
				, bool orderByDate = true
				, bool raisingOrder = true
				, int first = 0
				, int number = -1
			);

			static std::map<int, int> count(
				boost::shared_ptr<const Account> account
				, time::Date startDate, time::Date endDate, int first=0, int number=-1);

			static double sum(
				boost::shared_ptr<const Account> account
				, boost::shared_ptr<const security::User> user
			);
		};
	}
}

#endif // SYNTHESE_TransationPartTableSync_H__
