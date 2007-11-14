
/** RentABikeAction class implementation.
	@file RentABikeAction.cpp

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
#include "30_server/ParametersMap.h"

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

	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::RentABikeAction>::FACTORY_KEY("rentabike");
	}

	namespace vinci
	{
		const std::string RentABikeAction::PARAMETER_RATE_ID = Action_PARAMETER_PREFIX + "ri";
		const std::string RentABikeAction::PARAMETER_BIKE_ID = Action_PARAMETER_PREFIX + "bi";
		const std::string RentABikeAction::PARAMETER_CONTRACT_ID = Action_PARAMETER_PREFIX + "ci";
		const std::string RentABikeAction::PARAMETER_LOCK_ID = Action_PARAMETER_PREFIX + "li";
		const std::string RentABikeAction::PARAMETER_DATE = Action_PARAMETER_PREFIX + "da";



		RentABikeAction::RentABikeAction()
			: FactorableTemplate<Action, RentABikeAction>()
			, _date(TIME_CURRENT)
		{

		}


		/** Conversion from attributes to generic parameter maps.
		*/
		ParametersMap RentABikeAction::getParametersMap() const
		{
			ParametersMap map;
			if (_rate)
				map.insert(PARAMETER_RATE_ID, _rate->getKey());
			if (_contract)
				map.insert(PARAMETER_CONTRACT_ID, _contract->getKey());
			if (_bike)
				map.insert(PARAMETER_BIKE_ID, _bike->getKey());
			map.insert(PARAMETER_LOCK_ID, _lock ? Conversion::ToString(_lock->getKey()) : string());
			return map;
		}

		/** Conversion from generic parameters map to attributes.
		Removes the used parameters from the map.
		*/
		void RentABikeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Rate
				uid id = map.getUid(PARAMETER_RATE_ID, true, FACTORY_KEY);
				try
				{
					_rate = VinciRateTableSync::Get(id);
				}
				catch (VinciRate::ObjectNotFoundException& e)
				{
					throw ActionException("Specified rate not found");
				}

				// Bike
				id = map.getUid(PARAMETER_BIKE_ID, true, FACTORY_KEY);
				if (id == UNKNOWN_VALUE)
					throw ActionException("Le num�ro de v�lo doit �tre saisi");
				vector<shared_ptr<VinciBike> > bikes = VinciBikeTableSync::search(map.getString(PARAMETER_BIKE_ID, true, FACTORY_KEY), "");
				if (bikes.empty())
					throw ActionException("V�lo introuvable");
				vector<shared_ptr<VinciBike> >::iterator itb = bikes.begin();
				_bike = *itb;

				// Contract
				id = map.getUid(PARAMETER_CONTRACT_ID, true, FACTORY_KEY);
				try
				{
					_contract = VinciContractTableSync::Get(id);
				}
				catch(VinciContract::ObjectNotFoundException& e)
				{
					throw ActionException("Specified contract not found");
				}

				// Lock
				id = map.getUid(PARAMETER_LOCK_ID, true, FACTORY_KEY);
				if (id == UNKNOWN_VALUE)
					throw ActionException("L'antivol doit �tre saisi.");
				_lockMarkedNumber = map.getString(PARAMETER_LOCK_ID, true, FACTORY_KEY);
				vector<shared_ptr<VinciAntivol> > locks = VinciAntivolTableSync::search(_lockMarkedNumber);
				if (!locks.empty())
					_lock = locks.front();

				// Date
				_date = map.getDateTime(PARAMETER_DATE, false, FACTORY_KEY);

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
