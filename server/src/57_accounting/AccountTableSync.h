
/** AccountTableSync class header.
	@file AccountTableSync.h

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

#ifndef SYNTHESE_AccountTableSync_H__
#define SYNTHESE_AccountTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "DBNoSyncTableSyncTemplate.hpp"

namespace synthese
{
	namespace accounts
	{
		class Account;

		/** Account table synchronizer.
			@ingroup m57LS refLS
		*/

		class AccountTableSync : public db::DBNoSyncTableSyncTemplate<AccountTableSync,Account>
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
			static const std::string COL_LOCKED;
			static const std::string COL_STOCK_ACCOUNT_ID;
			static const std::string COL_UNIT_PRICE;

			AccountTableSync();

			/** Account search.
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Account*> Founded accounts.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<Account> > search(
				uid rightUserId
				, const std::string& rightClassNumber
				, uid leftUserId
				, const std::string& leftClassNumber
				, const std::string name=""
				, bool orderByName = true
				, bool raisingOrder = true
				, int first = 0
				, int number = 0
			);

			static std::string GetNextCode(std::string basisCode);

		};

	}
}

#endif // SYNTHESE_AccountTableSync_H__

