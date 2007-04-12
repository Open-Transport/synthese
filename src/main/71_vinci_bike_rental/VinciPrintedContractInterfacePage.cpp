
/** VinciPrintedContractInterfacePage class implementation.
	@file VinciPrintedContractInterfacePage.cpp

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

#include "02_db/DBEmptyResultException.h"

#include "04_time/Date.h"

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "57_accounting/TransactionPart.h"
#include "57_accounting/Transaction.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"

#include "VinciPrintedContractInterfacePage.h"
#include "VinciContract.h"
#include "VinciSite.h"
#include "VinciAntivol.h"
#include "VinciBike.h"
#include "VinciRate.h"
#include "VinciSiteTableSync.h"
#include "VinciRateTableSync.h"

using boost::shared_ptr;

namespace synthese
{
	using namespace interfaces;
	using namespace security;
	using namespace db;
	using namespace accounts;
	using namespace time;

	namespace vinci
	{

		void VinciPrintedContractInterfacePage::display(std::ostream& stream
			, VariablesMap& vars
			, shared_ptr<const VinciContract> contract
			, const server::Request* request /*= NULL*/) const
		{
			shared_ptr<User>			user = UserTableSync::get(contract->getUserId());
			shared_ptr<VinciSite>		site = contract->getSite();
			shared_ptr<VinciBike>		bike = contract->getCurrentBike();
			shared_ptr<TransactionPart>	tp = contract->getCurrentRentTransactionPart();
			shared_ptr<Transaction>		t;
			shared_ptr<VinciAntivol>	antivol;
			shared_ptr<VinciRate>		rate;
			DateTime					endDate(Date::UNKNOWN_DATE, Hour());

			if (!tp.get())
			{
				t = TransactionTableSync::get(tp->getTransactionId());
				antivol = contract->getCurrentLock();
				rate = VinciRateTableSync::get(tp->getRateId());
				if (t)
				{
					endDate = rate->getEndDate(t->getStartDateTime());
				}
			}

			shared_ptr<TransactionPart>	guarantee = contract->getCurrentGuaranteeTransactionPart();
			shared_ptr<Account>			guaranteeAccount;
			if (!guarantee.get())
			{
				guaranteeAccount = AccountTableSync::get(guarantee->getAccountId());
			}


			ParametersVector pv;
			pv.push_back(Conversion::ToString(contract->getKey()));
			pv.push_back(user->getName());
			pv.push_back(user->getSurname());
			pv.push_back(site ? site->getName() : "");
			pv.push_back(user->getAddress());
			pv.push_back(site ? site->getAddress() : "");
			pv.push_back(user->getBirthDate().toString());
			pv.push_back(site ? site->getPhone() : "");
			pv.push_back(user->getPhone());
			pv.push_back(contract->getPassport());
			pv.push_back(bike ? bike->getNumber() : "");	//10
			pv.push_back(t ? t->getStartDateTime().toString() : "");
			pv.push_back(antivol ? antivol->getMarkedNumber() : "");
			pv.push_back(endDate.isUnknown() ? "" : endDate.toString());
			pv.push_back(rate ? rate->getName() : "");
			pv.push_back(guarantee ? Conversion::ToString(guarantee->getLeftCurrencyAmount()) : "");
			pv.push_back(guaranteeAccount ? guaranteeAccount->getName() : "");

			InterfacePage::display(stream, pv, vars, NULL, request);
		}
	}
}
 
