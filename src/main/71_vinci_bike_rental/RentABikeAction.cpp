
/** RentABikeAction class implementation.
	@file RentABikeAction.cpp

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

#include "57_accounting/TransactionPart.h"

#include "71_vinci_bike_rental/RentABikeAction.h"
#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciAntivol.h"
#include "71_vinci_bike_rental/VinciAntivolTableSync.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"

#include "01_util/Conversion.h"

#include "04_time/DateTime.h"
#include "04_time/TimeParseException.h"

#include "30_server/ActionException.h"

#include "57_accounting/Account.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/TransactionPartTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace time;
	using namespace accounts;
	using namespace db;

	namespace vinci
	{
		const std::string RentABikeAction::PARAMETER_RATE_ID = Action_PARAMETER_PREFIX + "ri";
		const std::string RentABikeAction::PARAMETER_BIKE_ID = Action_PARAMETER_PREFIX + "bi";
		const std::string RentABikeAction::PARAMETER_CONTRACT_ID = Action_PARAMETER_PREFIX + "ci";
		const std::string RentABikeAction::PARAMETER_LOCK_ID = Action_PARAMETER_PREFIX + "li";
		const std::string RentABikeAction::PARAMETER_DATE = Action_PARAMETER_PREFIX + "da";

		/** Conversion from attributes to generic parameter maps.
		*/
		ParametersMap RentABikeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_rate)
				map.insert(make_pair(PARAMETER_RATE_ID, Conversion::ToString(_rate->getKey())));
			if (_contract)
				map.insert(make_pair(PARAMETER_CONTRACT_ID, Conversion::ToString(_contract->getKey())));
			if (_bike)
				map.insert(make_pair(PARAMETER_BIKE_ID, Conversion::ToString(_bike->getKey())));
			map.insert(make_pair(PARAMETER_LOCK_ID, _lock ? Conversion::ToString(_lock->getKey()) : ""));
			return map;
		}

		/** Conversion from generic parameters map to attributes.
		Removes the used parameters from the map.
		*/
		void RentABikeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				// Rate
				it=map.find(PARAMETER_RATE_ID);
				if (it == map.end())
					throw ActionException("Rate not specified");
				try
				{
					_rate = VinciRateTableSync::get(Conversion::ToLongLong(it->second));
				}
				catch (VinciRate::ObjectNotFoundException& e)
				{
					throw ActionException("Specified rate not found");
				}

				// Bike
				it=map.find(PARAMETER_BIKE_ID);
				if (it == map.end())
					throw ActionException("Bike not specified");
				if (it->second.empty())
					throw ActionException("Le numéro de vélo doit être saisi");
				vector<shared_ptr<VinciBike> > bikes = VinciBikeTableSync::search(it->second, "");
				if (bikes.empty())
					throw ActionException("Vélo introuvable");
				vector<shared_ptr<VinciBike> >::iterator itb = bikes.begin();
				_bike = *itb;

				// Contract
				it=map.find(PARAMETER_CONTRACT_ID);
				if (it == map.end())
					throw ActionException("Contract not specified");
				try
				{
					_contract = VinciContractTableSync::get(Conversion::ToLongLong(it->second));
				}
				catch(VinciContract::ObjectNotFoundException& e)
				{
					throw ActionException("Specified contract not found");
				}

				// Lock
				it = map.find(PARAMETER_LOCK_ID);
				if (it == map.end())
					throw ActionException("Lock not specified");
				if (it->second.empty())
					throw ActionException("L'antivol doit être saisi.");
				if (!it->second.empty())
				{
					_lockMarkedNumber = it->second;
					vector<shared_ptr<VinciAntivol> > locks = VinciAntivolTableSync::search(_lockMarkedNumber);
					if (!locks.empty())
						_lock = locks.front();
				}

				// Date
				it = map.find(PARAMETER_DATE);
				if (it != map.end())
				{
					_date = DateTime::FromString(it->second);
				}

				// TODO Control of guarantees

				_amount = _rate->getStartTicketsPrice();
			}
			catch (TimeParseException e)
			{
				throw ActionException("La date saisie n'est pas correcte");
			}
		}

		void RentABikeAction::run()
		{
			DateTime unknownDate(TIME_UNKNOWN);

			// Lock creation if necessary
			if (!_lock.get())
			{
				_lock.reset(new VinciAntivol);
				_lock->setMarkedNumber(_lockMarkedNumber);
				VinciAntivolTableSync::save(_lock.get());
			}

			// Transaction
			shared_ptr<Transaction> transaction(new Transaction);
			transaction->setStartDateTime(_date);
			transaction->setEndDateTime(unknownDate);
			transaction->setLeftUserId(_contract->getUserId());
			TransactionTableSync::save(transaction.get());

			// Part 1 : service
			shared_ptr<TransactionPart> transactionPart(new TransactionPart);
			transactionPart->setTransactionId(transaction->getKey());
			transactionPart->setAccountId(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE)->getKey());
			transactionPart->setLeftCurrencyAmount(_amount);
			transactionPart->setRightCurrencyAmount(_amount);
			transactionPart->setRateId(_rate->getKey());
			transactionPart->setTradedObjectId(Conversion::ToString(_bike->getKey()));
			TransactionPartTableSync::save(transactionPart.get());

			// Part 2 : customer
			shared_ptr<TransactionPart> changeTransactionPart(new TransactionPart);
			changeTransactionPart->setTransactionId(transaction->getKey());
			changeTransactionPart->setAccountId(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE)->getKey());
			changeTransactionPart->setLeftCurrencyAmount(-_amount);
			changeTransactionPart->setRightCurrencyAmount(-_amount);
			TransactionPartTableSync::save(changeTransactionPart.get());

			// Part 3 : bike stock lower

			// Part 4 : rented bike higher

			// Part 5 : Lock rent
			shared_ptr<TransactionPart> lockTransactionPart(new TransactionPart);
			lockTransactionPart->setTransactionId(transaction->getKey());
			lockTransactionPart->setAccountId(VinciBikeRentalModule::getFreeLockRentServiceAccount()->getKey());
			lockTransactionPart->setLeftCurrencyAmount(0);
			lockTransactionPart->setRightCurrencyAmount(0);
			lockTransactionPart->setTradedObjectId(Conversion::ToString(_lock->getKey()));
			TransactionPartTableSync::save(lockTransactionPart.get());
		}
	}
}
