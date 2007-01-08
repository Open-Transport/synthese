
#include "01_util/Conversion.h"

#include "04_time/DateTime.h"

#include "12_security/User.h"

#include "30_server/ServerModule.h"

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

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace accounts;
	using namespace time;
	
	namespace vinci
	{
		const string VinciAddGuaranteeAction::PARAMETER_AMOUNT = Action::PARAMETER_PREFIX + "am";
		const string VinciAddGuaranteeAction::PARAMETER_CONTRACT_ID = Action::PARAMETER_PREFIX + "ci";
		const string VinciAddGuaranteeAction::PARAMETER_ACCOUNT_ID = Action::PARAMETER_PREFIX + "ac";


		Request::ParametersMap VinciAddGuaranteeAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_AMOUNT, Conversion::ToString(_amount)));
			map.insert(make_pair(PARAMETER_CONTRACT_ID, Conversion::ToString(_contract->getKey())));
			map.insert(make_pair(PARAMETER_ACCOUNT_ID, Conversion::ToString(_account->getKey())));
			return map;
		}

		void VinciAddGuaranteeAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_AMOUNT);
			if (it != map.end())
			{
				_amount = Conversion::ToDouble(it->second);
				map.erase(it);
			}

			it = map.find(PARAMETER_ACCOUNT_ID);
			if (it != map.end())
			{
				_account = AccountTableSync::get(ServerModule::getSQLiteThread(), Conversion::ToLongLong(it->second));
				map.erase(it);
			}

			it = map.find(PARAMETER_CONTRACT_ID);
			if (it != map.end())
			{
				_contract = VinciContractTableSync::get(ServerModule::getSQLiteThread(), Conversion::ToLongLong(it->second));
				map.erase(it);
			}

		}

		void VinciAddGuaranteeAction::run()
		{
			DateTime now;
			DateTime unknownDate(TIME_UNKNOWN);

			// Transaction
			Transaction* transaction = new Transaction;
			transaction->setStartDateTime(now);
			transaction->setEndDateTime(unknownDate);
			transaction->setLeftUserId(_contract->getUserId());
			TransactionTableSync::save(ServerModule::getSQLiteThread(), transaction);

			// Part 1 : customer
			TransactionPart* transactionPart = new TransactionPart;
			Account* account = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE);
			transactionPart->setTransactionId(transaction->getKey());
			transactionPart->setAccountId(account->getKey());
			transactionPart->setLeftCurrencyAmount(_amount);
			transactionPart->setRightCurrencyAmount(_amount);
			TransactionPartTableSync::save(ServerModule::getSQLiteThread(), transactionPart);
			
			// Part 2 : 
			TransactionPart* changeTransactionPart = new TransactionPart;
			changeTransactionPart->setTransactionId(transaction->getKey());
			changeTransactionPart->setAccountId(_account->getKey());
			changeTransactionPart->setLeftCurrencyAmount(-_amount);
			changeTransactionPart->setRightCurrencyAmount(-_amount);
			TransactionPartTableSync::save(ServerModule::getSQLiteThread(), changeTransactionPart);

			delete account;
			delete transactionPart;
			delete changeTransactionPart;
			delete transaction;
		}

		VinciAddGuaranteeAction::~VinciAddGuaranteeAction()
		{
			delete _contract;
			delete _account;
		}

		VinciAddGuaranteeAction::VinciAddGuaranteeAction()
			: _contract(NULL), _account(NULL)
		{ }
	}
}
