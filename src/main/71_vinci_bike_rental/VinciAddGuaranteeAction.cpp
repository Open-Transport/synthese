
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
	
	namespace vinci
	{
		const string VinciAddGuaranteeAction::PARAMETER_AMOUNT = Action_PARAMETER_PREFIX + "am";
		const string VinciAddGuaranteeAction::PARAMETER_CONTRACT_ID = Action_PARAMETER_PREFIX + "ci";
		const string VinciAddGuaranteeAction::PARAMETER_ACCOUNT_ID = Action_PARAMETER_PREFIX + "ac";
		const string VinciAddGuaranteeAction::PARAMETER_DATE = Action_PARAMETER_PREFIX + "da";


		ParametersMap VinciAddGuaranteeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_AMOUNT, Conversion::ToString(_amount)));
			if (_contract.get())
				map.insert(make_pair(PARAMETER_CONTRACT_ID, Conversion::ToString(_contract->getKey())));
			if (_account.get())
				map.insert(make_pair(PARAMETER_ACCOUNT_ID, Conversion::ToString(_account->getKey())));
			map.insert(make_pair(PARAMETER_DATE, Conversion::ToString(_date.toString())));
			return map;
		}

		void VinciAddGuaranteeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_AMOUNT);
				if (it != map.end())
				{
					_amount = Conversion::ToDouble(it->second);
				}

				it = map.find(PARAMETER_ACCOUNT_ID);
				if (it != map.end())
				{
					_account = AccountTableSync::get(Conversion::ToLongLong(it->second));
				}

				it = map.find(PARAMETER_CONTRACT_ID);
				if (it != map.end())
				{
					_contract = VinciContractTableSync::get(Conversion::ToLongLong(it->second));
				}

				it = map.find(PARAMETER_DATE);
				if (it != map.end())
				{
					_date = DateTime::FromString(it->second);
				}
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
