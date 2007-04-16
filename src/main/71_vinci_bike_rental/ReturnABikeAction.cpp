
/** ReturnABikeAction class implementation.
	@file ReturnABikeAction.cpp

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

#include "02_db/DBEmptyResultException.h"

#include "04_time/DateTime.h"

#include "30_server/ActionException.h"

#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"

#include "71_vinci_bike_rental/ReturnABikeAction.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace time;
	using namespace accounts;
	
	namespace vinci
	{
		const string ReturnABikeAction::PARAMETER_TRANSACTION_PART_ID = Action_PARAMETER_PREFIX + "tpi";


		ParametersMap ReturnABikeAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_TRANSACTION_PART_ID, _transactionPart ? Conversion::ToString(_transactionPart->getKey()) : ""));
			return map;
		}

		void ReturnABikeAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_TRANSACTION_PART_ID);
				if (it == map.end())
					throw ActionException("Transaction not specified");

				_transactionPart = TransactionPartTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch (DBEmptyResultException<TransactionPart> e)
			{
				throw ActionException("Specified transaction part not found ");
			}
		}

		void ReturnABikeAction::run()
		{
			DateTime now;
			shared_ptr<VinciRate> rate = VinciRateTableSync::get(_transactionPart->getRateId());

			// Create an other transaction if the customer has to pay for the transaction

			shared_ptr<Transaction> t = TransactionTableSync::get(_transactionPart->getTransactionId());
			t->setEndDateTime(now);
			TransactionTableSync::save(t.get());
		}

		void ReturnABikeAction::setTransactionPart( boost::shared_ptr<accounts::TransactionPart> transactionPart )
		{
			_transactionPart = transactionPart;
		}
	}
}
