
/** VinciContract class implementation.
	@file VinciContract.cpp

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

#include <vector>

#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "57_accounting/TransactionPart.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/TransactionTableSync.h"
#include "57_accounting/Transaction.h"

#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/VinciRate.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciAntivolTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace util;
	using namespace security;
	using namespace time;
	using namespace accounts;

	namespace vinci
	{
		VinciContract::VinciContract(uid id)
			: Registrable<uid, VinciContract>(id)
			, _date(TIME_UNKNOWN)
		{}

		void VinciContract::setUserId(uid id)
		{
			_userId = id;
		}

		uid VinciContract::getUserId() const
		{
			return _userId;
		}

		shared_ptr<User> VinciContract::getUser() const
		{
			try
			{
				return UserTableSync::get(_userId);
			}
			catch (...)
			{
				return shared_ptr<User>();
			}
		}

		void VinciContract::setSiteId( uid id )
		{
			_siteId = id;
		}

		void VinciContract::setDate( const time::DateTime& date )
		{
			_date = date;
		}

		const time::DateTime& VinciContract::getDate() const
		{
			return _date;
		}

		shared_ptr<VinciSite> VinciContract::getSite() const
		{
			try
			{
				return VinciSiteTableSync::get(_siteId);
			}
			catch(...)
			{
				return shared_ptr<VinciSite>();
			}
		}

		const std::string& VinciContract::getPassport() const
		{
			return _passport;
		}

		void VinciContract::setPassport( const std::string& text )
		{
			_passport = text;
		}

		shared_ptr<VinciBike> VinciContract::getCurrentBike() const
		{
			try
			{
				shared_ptr<TransactionPart> tp = getCurrentRentTransactionPart();
				if (!tp.get())
					return shared_ptr<VinciBike>();
				if (tp->getTradedObjectId() != "")
					return VinciBikeTableSync::get(Conversion::ToLongLong(tp->getTradedObjectId()));
				return shared_ptr<VinciBike>();
			}
			catch(...)
			{
				return shared_ptr<VinciBike>();
			}
		}

		uid VinciContract::getSiteId() const
		{
			return _siteId;
		}

		shared_ptr<TransactionPart> VinciContract::getCurrentRentTransactionPart() const
		{
			vector<shared_ptr<TransactionPart> > rents = TransactionPartTableSync::search(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE), UserTableSync::get(_userId), false, 0, 1);
			return rents.empty() ? shared_ptr<TransactionPart>() : rents.front();
		}

		shared_ptr<VinciAntivol> VinciContract::getCurrentLock() const
		{
			try
			{
				shared_ptr<TransactionPart> tp = getCurrentRentTransactionPart();
				if (!tp.get())
					return shared_ptr<VinciAntivol>();
				shared_ptr<Transaction> t = TransactionTableSync::get(tp->getTransactionId());
				tp = t->getPart(VinciBikeRentalModule::getFreeLockRentServiceAccount());
				return VinciAntivolTableSync::get(Conversion::ToLongLong(tp->getTradedObjectId()));
			}
			catch (...)
			{
				return shared_ptr<VinciAntivol>();
			}
		}

		shared_ptr<TransactionPart> VinciContract::getCurrentGuaranteeTransactionPart() const
		{
			vector<shared_ptr<TransactionPart> > gua = TransactionPartTableSync::search(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE), UserTableSync::get(_userId), false, -1, 1);
			return gua.empty() ? shared_ptr<TransactionPart>() : gua.front();
		}
	}
}

