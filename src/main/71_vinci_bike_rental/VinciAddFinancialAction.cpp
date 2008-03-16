
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
#include "57_accounting/AccountTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/Request.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "04_time/DateTime.h"

#include "VinciAddFinancialAction.h"
#include "VinciBikeRentalModule.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace time;
	using namespace accounts;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciAddFinancialAction>::FACTORY_KEY("add_financial");
	}

	namespace vinci
	{
		const string VinciAddFinancialAction::PARAMETER_AMOUNT = Action_PARAMETER_PREFIX + "am";
		const string VinciAddFinancialAction::PARAMETER_USER = Action_PARAMETER_PREFIX + "us";
		const string VinciAddFinancialAction::PARAMETER_ACCOUNT = Action_PARAMETER_PREFIX + "ac";
		const string VinciAddFinancialAction::PARAMETER_PIECES = Action_PARAMETER_PREFIX + "pi";
		
		
		VinciAddFinancialAction::VinciAddFinancialAction()
			: util::FactorableTemplate<Action, VinciAddFinancialAction>()
		{
		}
		
		
		
		ParametersMap VinciAddFinancialAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_USER, _user.get() ? _user->getKey() : 0);
			return map;
		}
		
		
		
		void VinciAddFinancialAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id = map.getUid(PARAMETER_USER, true, FACTORY_KEY);
			if (id != 0)
			{
				try
				{
					_user = UserTableSync::Get(id);
				}
				catch (User::ObjectNotFoundException e)
				{
					throw ActionException("User not found");
				}
			}

			id = map.getUid(PARAMETER_ACCOUNT, true, FACTORY_KEY);
			try
			{
				_account = AccountTableSync::Get(id);
				if (_account->getStockAccountId() > 0)
					_stockAccount = AccountTableSync::Get(_account->getStockAccountId());
			}
			catch (Account::ObjectNotFoundException e)
			{
				throw ActionException("Account not found");
			}

			_pieces = map.getInt(PARAMETER_PIECES, false, FACTORY_KEY);
			_amount = (_pieces != UNKNOWN_VALUE) 
				? -_pieces * _account->getUnitPrice()
				: map.getDouble(PARAMETER_AMOUNT, true, FACTORY_KEY);

			_name = (_pieces != UNKNOWN_VALUE)
				? "Vente " + Conversion::ToString(_pieces) + " x " + _account->getName()
				: "Reglement " + _account->getName();

			_stockId = VinciBikeRentalModule::GetSessionSite(_request->getSession());
		}
		
		
		
		void VinciAddFinancialAction::run()
		{
			DateTime now(TIME_CURRENT);

			shared_ptr<Transaction> ft(new Transaction);
			ft->setStartDateTime(now);
			ft->setEndDateTime(now);
			ft->setLeftUserId(_user.get() ? _user->getKey() : 0);
			ft->setName(_name);
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
			ftp2->setAccountId(_account->getKey());
			ftp2->setAmount(-_amount);
			TransactionPartTableSync::save(ftp2.get());

			if (_pieces != UNKNOWN_VALUE && _stockAccount.get())
			{
				// Part 3 : stock
				TransactionPart	ftp3;
				ftp3.setTransactionId(ft->getKey());
				ftp3.setAccountId(_stockAccount->getKey());
				ftp3.setAmount(-_pieces);
				ftp3.setStockId(_stockId);
				TransactionPartTableSync::save(&ftp3);

				// Part 4 : charge
				TransactionPart ftp4;
				ftp4.setTransactionId(ft->getKey());
				ftp4.setAccountId(VinciBikeRentalModule::getStockChargeAccount()->getKey());
				ftp4.setAmount(_pieces);
				TransactionPartTableSync::save(&ftp4);
			}
		}

		void VinciAddFinancialAction::setUser( boost::shared_ptr<const security::User> user )
		{
			_user = user;
		}
	}
}
