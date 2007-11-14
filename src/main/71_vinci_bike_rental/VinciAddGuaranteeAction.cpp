
/** VinciAddGuaranteeAction class implementation.
	@file VinciAddGuaranteeAction.cpp

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

#include "01_util/Conversion.h"

#include "04_time/DateTime.h"
#include "04_time/TimeParseException.h"

#include "12_security/User.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciAddGuaranteeAction.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace accounts;
	using namespace time;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciAddGuaranteeAction>::FACTORY_KEY("vinciaddguarantee");
	}
	
	namespace vinci
	{
		const string VinciAddGuaranteeAction::PARAMETER_AMOUNT = Action_PARAMETER_PREFIX + "am";
		const string VinciAddGuaranteeAction::PARAMETER_CONTRACT_ID = Action_PARAMETER_PREFIX + "ci";
		const string VinciAddGuaranteeAction::PARAMETER_ACCOUNT_ID = Action_PARAMETER_PREFIX + "ac";
		const string VinciAddGuaranteeAction::PARAMETER_DATE = Action_PARAMETER_PREFIX + "da";



		VinciAddGuaranteeAction::VinciAddGuaranteeAction()
			: FactorableTemplate<Action,VinciAddGuaranteeAction>()
			, _date(TIME_CURRENT)
		{

		}

		ParametersMap VinciAddGuaranteeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_AMOUNT, _amount);
			if (_contract.get())
				map.insert(PARAMETER_CONTRACT_ID, _contract->getKey());
			if (_account.get())
				map.insert(PARAMETER_ACCOUNT_ID, _account->getKey());
			map.insert(PARAMETER_DATE, _date);
			return map;
		}

		void VinciAddGuaranteeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_amount = map.getDouble(PARAMETER_AMOUNT, false, FACTORY_KEY);

				uid id = map.getUid(PARAMETER_ACCOUNT_ID, false, FACTORY_KEY);
				_account = AccountTableSync::Get(id);

				id = map.getUid(PARAMETER_CONTRACT_ID, false, FACTORY_KEY);
				_contract = VinciContractTableSync::Get(id);

				_date = map.getDateTime(PARAMETER_DATE, false, FACTORY_KEY);
				if (_date.isUnknown())
					_date = DateTime(TIME_CURRENT);
			}
			catch (Account::ObjectNotFoundException& e)
			{
				throw ActionException("Specified account not found " + e.getMessage());
			}
			catch(VinciContract::ObjectNotFoundException& e)
			{
				throw ActionException("Specified contract not found " + e.getMessage());
			}
			catch (TimeParseException& e)
			{
				throw ActionException("La date saisie n'est pas correcte");
			}
		}

		void VinciAddGuaranteeAction::run()
		{
			DateTime unknownDate(TIME_UNKNOWN);

			// Transaction
			shared_ptr<Transaction> transaction(new Transaction);
			transaction->setStartDateTime(_date);
			transaction->setEndDateTime(unknownDate);
			transaction->setLeftUserId(_contract->getUserId());
			TransactionTableSync::save(transaction.get());

			// Part 1 : customer
			shared_ptr<TransactionPart> transactionPart(new TransactionPart);
			shared_ptr<Account> account = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE);
			transactionPart->setTransactionId(transaction->getKey());
			transactionPart->setAccountId(account->getKey());
			transactionPart->setLeftCurrencyAmount(_amount);
			transactionPart->setRightCurrencyAmount(_amount);
			TransactionPartTableSync::save(transactionPart.get());
			
			// Part 2 : 
			shared_ptr<TransactionPart> changeTransactionPart(new TransactionPart);
			changeTransactionPart->setTransactionId(transaction->getKey());
			changeTransactionPart->setAccountId(_account->getKey());
			changeTransactionPart->setLeftCurrencyAmount(-_amount);
			changeTransactionPart->setRightCurrencyAmount(-_amount);
			TransactionPartTableSync::save(changeTransactionPart.get());
		}
	}
}
