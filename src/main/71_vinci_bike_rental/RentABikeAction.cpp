
#include "01_util/Conversion.h"

#include "04_time/DateTime.h"

#include "30_server/ServerModule.h"

#include "57_accounting/Account.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"

#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/RentABikeAction.h"
#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace time;
	using namespace accounts;

	namespace vinci
	{
		const std::string RentABikeAction::PARAMETER_RATE_ID = Action::PARAMETER_PREFIX + "ri";
		const std::string RentABikeAction::PARAMETER_BIKE_ID = Action::PARAMETER_PREFIX + "bi";
		const std::string RentABikeAction::PARAMETER_CONTRACT_ID = Action::PARAMETER_PREFIX + "ci";

		/** Conversion from attributes to generic parameter maps.
		*/
		Request::ParametersMap RentABikeAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_RATE_ID, Conversion::ToString(_rateId)));
			map.insert(make_pair(PARAMETER_CONTRACT_ID, Conversion::ToString(_contract->getKey())));
			map.insert(make_pair(PARAMETER_BIKE_ID, Conversion::ToString(_bikeId)));
			return map;
		}

		/** Conversion from generic parameters map to attributes.
		Removes the used parameters from the map.
		*/
		void RentABikeAction::setFromParametersMap(server::Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;
			
			it=map.find(PARAMETER_RATE_ID);
			if (it != map.end())
			{
				_rateId = Conversion::ToLongLong(it->second);
				map.erase(it);
			}

			it=map.find(PARAMETER_BIKE_ID);
			if (it != map.end())
			{
				_bikeId = Conversion::ToLongLong(it->second);
				map.erase(it);
			}

			it=map.find(PARAMETER_CONTRACT_ID);
			if (it != map.end())
			{
				_contract = VinciContractTableSync::get(ServerModule::getSQLiteThread(), Conversion::ToLongLong(it->second));
				map.erase(it);
			}
		}

		void RentABikeAction::run()
		{
			DateTime now;
			DateTime unknownDate(TIME_UNKNOWN);

			VinciRate* rate = VinciRateTableSync::get(ServerModule::getSQLiteThread(), _rateId);

			double amount;

			amount = rate->getStartTicketsPrice();

			// Control of bike
			vector<VinciBike*> bikes = VinciBikeTableSync::search(Conversion::ToString(_bikeId), "");
			if (bikes.size() == 0)
				return;
			vector<VinciBike*>::iterator it = bikes.begin();
			VinciBike* bike = *it;
			
			// Control of guarantees

			// Transaction
			Transaction* transaction = new Transaction;
			transaction->setStartDateTime(now);
			transaction->setEndDateTime(unknownDate);
			transaction->setLeftUserId(_contract->getUserId());
			TransactionTableSync::save(ServerModule::getSQLiteThread(), transaction);

			// Part 1 : service
			TransactionPart* transactionPart = new TransactionPart;
			transactionPart->setTransactionId(transaction->getKey());
			transactionPart->setAccountId(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE)->getKey());
			transactionPart->setLeftCurrencyAmount(amount);
			transactionPart->setRightCurrencyAmount(amount);
			transactionPart->setRateId(_rateId);
			transactionPart->setTradedObjectId(Conversion::ToString(bike->getKey()));
			TransactionPartTableSync::save(ServerModule::getSQLiteThread(), transactionPart);

			// Part 2 : customer
			TransactionPart* changeTransactionPart = new TransactionPart;
			changeTransactionPart->setTransactionId(transaction->getKey());
			changeTransactionPart->setAccountId(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE)->getKey());
			changeTransactionPart->setLeftCurrencyAmount(-amount);
			changeTransactionPart->setRightCurrencyAmount(-amount);
			TransactionPartTableSync::save(ServerModule::getSQLiteThread(), changeTransactionPart);

			// Part 3 : bike stock lower

			// Part 4 : rented bike higher

		}
	}
}
