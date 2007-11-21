
/** ReturnABikeAction class implementation.
	@file ReturnABikeAction.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 

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

#include "04_time/DateTime.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/Account.h"

#include "71_vinci_bike_rental/ReturnABikeAction.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace time;
	using namespace accounts;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::ReturnABikeAction>::FACTORY_KEY("vincireturnbike");
	}
	
	namespace vinci
	{
		const string ReturnABikeAction::PARAMETER_TRANSACTION_PART_ID = Action_PARAMETER_PREFIX + "tpi";


		ParametersMap ReturnABikeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TRANSACTION_PART_ID, _transactionPart ? Conversion::ToString(_transactionPart->getKey()) : "");
			return map;
		}

		void ReturnABikeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				uid id = map.getUid(PARAMETER_TRANSACTION_PART_ID, true, FACTORY_KEY);
				_transactionPart = TransactionPartTableSync::Get(id);
			}
			catch (TransactionPart::ObjectNotFoundException& e)
			{
				throw ActionException(e.getMessage());
			}
		}

		void ReturnABikeAction::run()
		{
			DateTime now(TIME_CURRENT);
			shared_ptr<const VinciRate> rate = VinciRateTableSync::Get(_transactionPart->getRateId());
			shared_ptr<Transaction> t = TransactionTableSync::GetUpdateable(_transactionPart->getTransactionId());
			double amount(rate->getAdditionalAmountToPay(t->getStartDateTime()));

			if (amount > 0)
			{
				shared_ptr<Transaction> ft(new Transaction);
				ft->setStartDateTime(now);
				ft->setEndDateTime(now);
				ft->setLeftUserId(t->getLeftUserId());
				ft->setName("Retour vélo, pénalité de retard");
				TransactionTableSync::save(ft.get());

				// Part 1 : customer
				shared_ptr<TransactionPart> ftp(new TransactionPart);
				ftp->setTransactionId(ft->getKey());
				ftp->setAmount(-amount);
				ftp->setAccountId(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE)->getKey());
				TransactionPartTableSync::save(ftp.get());

				// Part 2 : service
				shared_ptr<TransactionPart> ftp2(new TransactionPart);
				ftp2->setTransactionId(ft->getKey());
				ftp2->setAccountId(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE)->getKey());
				ftp2->setLeftCurrencyAmount(amount);
				ftp2->setRightCurrencyAmount(amount);
				ftp2->setRateId(rate->getKey());
				TransactionPartTableSync::save(ftp2.get());
			}

			t->setEndDateTime(now);
			TransactionTableSync::save(t.get());
		}

		void ReturnABikeAction::setTransactionPart( boost::shared_ptr<accounts::TransactionPart> transactionPart )
		{
			_transactionPart = transactionPart;
		}
	}
}
