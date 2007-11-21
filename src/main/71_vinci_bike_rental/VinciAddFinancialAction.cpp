
/** VinciAddFinancialAction class implementation.
	@file VinciAddFinancialAction.cpp
	@author Hugues Romain
	@date 2007

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

#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/Account.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "04_time/DateTime.h"

#include "VinciAddFinancialAction.h"
#include "VinciBikeRentalModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace time;
	using namespace accounts;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciAddFinancialAction>::FACTORY_KEY("add_financial");
	}

	namespace vinci
	{
		const string VinciAddFinancialAction::PARAMETER_AMOUNT = Action_PARAMETER_PREFIX + "am";
		const string VinciAddFinancialAction::PARAMETER_USER = Action_PARAMETER_PREFIX + "us";

		
		
		VinciAddFinancialAction::VinciAddFinancialAction()
			: util::FactorableTemplate<Action, VinciAddFinancialAction>()
		{
		}
		
		
		
		ParametersMap VinciAddFinancialAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_USER, _user->getKey());
			return map;
		}
		
		
		
		void VinciAddFinancialAction::_setFromParametersMap(const ParametersMap& map)
		{
			_amount = map.getDouble(PARAMETER_AMOUNT, true, FACTORY_KEY);
			uid id = map.getUid(PARAMETER_USER, true, FACTORY_KEY);
			try
			{
				_user = UserTableSync::Get(id);
			}
			catch (User::ObjectNotFoundException e)
			{
				throw ActionException("User not found");
			}
		}
		
		
		
		void VinciAddFinancialAction::run()
		{
			DateTime now(TIME_CURRENT);

			shared_ptr<Transaction> ft(new Transaction);
			ft->setStartDateTime(now);
			ft->setEndDateTime(now);
			ft->setLeftUserId(_user->getKey());
			ft->setName("Reglement");
			TransactionTableSync::save(ft.get());

			// Part 1 : customer
			shared_ptr<TransactionPart> ftp(new TransactionPart);
			ftp->setTransactionId(ft->getKey());
			ftp->setAmount(_amount);
			ftp->setAccountId(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE)->getKey());
			TransactionPartTableSync::save(ftp.get());

			// Part 2 : cash
			shared_ptr<TransactionPart> ftp2(new TransactionPart);
			ftp2->setTransactionId(ft->getKey());
			ftp2->setAccountId(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CHANGE_CASH_ACCOUNT_CODE)->getKey());
			ftp2->setAmount(-_amount);
			TransactionPartTableSync::save(ftp2.get());
		}

		void VinciAddFinancialAction::setUser( boost::shared_ptr<const security::User> user )
		{
			_user = user;
		}
	}
}
