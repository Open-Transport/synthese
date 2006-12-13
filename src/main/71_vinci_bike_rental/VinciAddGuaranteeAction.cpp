
#include "01_util/Conversion.h"

#include "04_time/DateTime.h"

#include "12_security/User.h"

#include "30_server/ServerModule.h"

#include "57_accounting/Account.h"
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
	
	namespace vinci
	{
		const string VinciAddGuaranteeAction::PARAMETER_AMOUNT = Action::PARAMETER_PREFIX + "am";
		const string VinciAddGuaranteeAction::PARAMETER_CONTRACT_ID = Action::PARAMETER_PREFIX + "ci";


		Request::ParametersMap VinciAddGuaranteeAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_AMOUNT, Conversion::ToString(_amount)));
			map.insert(make_pair(PARAMETER_CONTRACT_ID, Conversion::ToString(_contract->getKey())));
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

			it = map.find(PARAMETER_CONTRACT_ID);
			if (it != map.end())
			{
				_contract = VinciContractTableSync::get(ServerModule::getSQLiteThread(), Conversion::ToLongLong(it->second));
				map.erase(it);
			}

		}

		void VinciAddGuaranteeAction::run()
		{
			time::DateTime now;

			// Transaction
			Transaction* transaction = new Transaction;
			transaction->setStartDateTime(now);
			transaction->setLeftUserId(_contract->getUserId());
			TransactionTableSync::save(ServerModule::getSQLiteThread(), transaction);

			// Part 1 : customer
			TransactionPart* transactionPart = new TransactionPart;
			transactionPart->setTransactionId(transaction->getKey());
			transactionPart->setAccountId(VinciBikeRentalModule::getGuaranteeAccount()->getKey());
			transactionPart->setLeftCurrencyAmount(_amount);
			transactionPart->setRightCurrencyAmount(_amount);
			TransactionPartTableSync::save(ServerModule::getSQLiteThread(), transactionPart);

			// Part 2 : 
			TransactionPart* changeTransactionPart = new TransactionPart;
			changeTransactionPart->setTransactionId(transaction->getKey());
			changeTransactionPart->setAccountId(VinciBikeRentalModule::getCheckGuaranteeAccount()->getKey());
			changeTransactionPart->setLeftCurrencyAmount(-_amount);
			changeTransactionPart->setRightCurrencyAmount(-_amount);
			TransactionPartTableSync::save(ServerModule::getSQLiteThread(), changeTransactionPart);

		}

		VinciAddGuaranteeAction::~VinciAddGuaranteeAction()
		{
			delete _contract;
		}

		VinciAddGuaranteeAction::VinciAddGuaranteeAction()
			: _contract(NULL)
		{ }
	}
}