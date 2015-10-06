
/** TransactionTableSync class header.
	@file TransactionTableSync.h

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

#ifndef SYNTHESE_TransactionTableSync_H__
#define SYNTHESE_TransactionTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "DBNoSyncTableSyncTemplate.hpp"

namespace synthese
{
	namespace accounts
	{
		class Transaction;
		class TransactionPart;
		class Account;

		/** Transaction table synchronizer.
			@ingroup m57LS refLS
		*/

		class TransactionTableSync : public db::DBNoSyncTableSyncTemplate<TransactionTableSync,Transaction>
		{
		public:
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_DOCUMENT_ID;
			static const std::string TABLE_COL_START_DATE_TIME;
			static const std::string TABLE_COL_END_DATE_TIME;
			static const std::string TABLE_COL_LEFT_USER_ID;
			static const std::string TABLE_COL_PLACE_ID;
			static const std::string TABLE_COL_COMMENT;

			TransactionTableSync();

			/** TransactionPart search.
			@param first First user to answer
			@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			@return vector<Account*> Founded accounts.
			@author Hugues Romain
			@date 2006
			*/
			static std::vector<boost::shared_ptr<Transaction> > searchTransactions(db::DB* db

				, int first = 0, int number = 0);

			static boost::shared_ptr<TransactionPart> getPart(
				boost::shared_ptr<const Transaction> transaction, boost::shared_ptr<const Account> account
			);
		};

	}
}

#endif // SYNTHESE_TransactionTableSync_H__

