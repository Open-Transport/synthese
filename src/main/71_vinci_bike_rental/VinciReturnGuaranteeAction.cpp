

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
#include "71_vinci_bike_rental/VinciReturnGuaranteeAction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace accounts;
	using namespace time;
	
	namespace vinci
	{
		const string VinciReturnGuaranteeAction::PARAMETER_GUARANTEE_ID = Action::PARAMETER_PREFIX + "gu";


		Request::ParametersMap VinciReturnGuaranteeAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_GUARANTEE_ID, Conversion::ToString(_guarantee->getKey())));
			return map;
		}

		void VinciReturnGuaranteeAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_GUARANTEE_ID);
			if (it != map.end())
			{
				_guarantee = TransactionTableSync::get(ServerModule::getSQLiteThread(), Conversion::ToLongLong(it->second));
				map.erase(it);
			}
		}

		void VinciReturnGuaranteeAction::run()
		{
	/*		DateTime now;
			Account* account = VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE);
			
			// Transaction
			_guarantee->setEndDateTime(now);
			TransactionTableSync::save(ServerModule::getSQLiteThread(), transaction);

			// Old amount
			vector<TransactionPart*> tps = TransactionPartTableSync::search(_guarantee);
			TransactionPart* tp = *(tps.begin());

			// Transaction
			Transaction* transaction = new Transaction;
			transaction->setStartDateTime(now);
			transaction->setEndDateTime(unknownDate);
			transaction->setLeftUserId(_contract->getUserId());
			TransactionTableSync::save(ServerModule::getSQLiteThread(), transaction);

			// Part 1 : customer
			TransactionPart* transactionPart = new TransactionPart;
			transactionPart->setTransactionId(transaction->getKey());
			transactionPart->setAccountId(account->getKey());
			transactionPart->setLeftCurrencyAmount(-tp->getAmount());
			transactionPart->setRightCurrencyAmount(-tp->getAmount());
			TransactionPartTableSync::save(ServerModule::getSQLiteThread(), transactionPart);
			
			// Part 2 : 
			TransactionPart* changeTransactionPart = new TransactionPart;
			changeTransactionPart->setTransactionId(transaction->getKey());
			changeTransactionPart->setAccountId(_account->getKey());
			changeTransactionPart->setLeftCurrencyAmount(tp->getAmount());
			changeTransactionPart->setRightCurrencyAmount(tp->getAmount());
			TransactionPartTableSync::save(ServerModule::getSQLiteThread(), changeTransactionPart);

			delete account;
			delete transactionPart;
			delete changeTransactionPart;
			delete transaction;
*/		}

		VinciReturnGuaranteeAction::~VinciReturnGuaranteeAction()
		{
			delete _guarantee;
		}

		VinciReturnGuaranteeAction::VinciReturnGuaranteeAction()
			: _guarantee(NULL)
		{ }
	}
}
