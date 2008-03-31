
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

#include "04_time/DateTime.h"

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
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<InterfacePage, vinci::VinciPrintedContractInterfacePage>::FACTORY_KEY("vincicontractprint");
	}

	namespace vinci
	{

		void VinciPrintedContractInterfacePage::display(
			std::ostream& stream
			, VariablesMap& vars
			, shared_ptr<const VinciContract> contract
			, const server::Request* request /*= NULL*/
		) const	{
			shared_ptr<const User>			user = UserTableSync::Get(contract->getUserId());
			shared_ptr<VinciSite>			site = contract->getSite();
			shared_ptr<VinciBike>			bike = contract->getCurrentBike();
			shared_ptr<TransactionPart>		tp = contract->getCurrentRentTransactionPart();
			shared_ptr<const Transaction>	t;
			shared_ptr<VinciAntivol>		antivol;
			shared_ptr<const VinciRate>		rate;
			time::DateTime					endDate(TIME_UNKNOWN);
			endDate.setHour(Hour(TIME_CURRENT));

			if (tp.get())
			{
				t = TransactionTableSync::Get(tp->getTransactionId());
				antivol = contract->getCurrentLock();
				rate = VinciRateTableSync::Get(tp->getRateId());
				if (t.get())
				{
					endDate = rate->getEndDate(t->getStartDateTime());
				}
			}

			shared_ptr<TransactionPart>	guarantee = contract->getCurrentGuaranteeTransactionPart();
			shared_ptr<const Account>			guaranteeAccount;
			if (guarantee.get())
			{
				guaranteeAccount = AccountTableSync::Get(guarantee->getAccountId());
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
 
