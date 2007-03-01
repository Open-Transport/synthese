
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

namespace synthese
{
	using namespace util;
	using namespace security;
	using namespace db;
	using namespace time;
	using namespace accounts;

	namespace vinci
	{
		VinciContract::VinciContract(uid id)
			: Registrable<uid, VinciContract>(id)
			, _user(NULL)
			, _site(NULL)
			, _date(TIME_UNKNOWN)
		{}

		void VinciContract::setUserId(uid id)
		{
			_userId = id;
			if (_user != NULL)
				delete _user;
		}

		uid VinciContract::getUserId() const
		{
			return _userId;
		}

		User* VinciContract::getUser()
		{
			if (_user == NULL)
				_user = UserTableSync::get(_userId);
			return _user;
		}

		VinciContract::~VinciContract()
		{
			delete _user;
			delete _site;
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

		VinciSite* VinciContract::getSite()
		{
			if (_site == NULL)
				_site = VinciSiteTableSync::get(_siteId);
			return _site;
		}

		const std::string& VinciContract::getPassport() const
		{
			return _passport;
		}

		void VinciContract::setPassport( const std::string& text )
		{
			_passport = text;
		}

		VinciBike* VinciContract::getCurrentBike() const
		{
			TransactionPart* tp = getCurrentRentTransactionPart();
			if (tp == NULL)
				return NULL;
			VinciBike* bike = NULL;
			if (tp->getTradedObjectId() != "")
				bike = VinciBikeTableSync::get(Conversion::ToLongLong(tp->getTradedObjectId()));
			delete tp;
			return bike;
		}

		uid VinciContract::getSiteId() const
		{
			return _siteId;
		}

		TransactionPart* VinciContract::getCurrentRentTransactionPart() const
		{
			vector<TransactionPart*> rents = TransactionPartTableSync::search(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE), UserTableSync::get(_userId), -1, 1);
			return rents.empty() ? NULL : rents.front();
		}

		VinciAntivol* VinciContract::getCurrentLock() const
		{
			TransactionPart* tp = getCurrentRentTransactionPart();
			if (tp == NULL)
				return NULL;
			Transaction* t = TransactionTableSync::get(tp->getTransactionId());
			delete tp;
			tp = t->getPart(VinciBikeRentalModule::getFreeLockRentServiceAccount());
			delete t;
			VinciAntivol* lock = VinciAntivolTableSync::get(Conversion::ToLongLong(tp->getTradedObjectId()));
			delete tp;
			return lock;
		}

		accounts::TransactionPart* VinciContract::getCurrentGuaranteeTransactionPart() const
		{
			vector<TransactionPart*> gua = TransactionPartTableSync::search(VinciBikeRentalModule::getAccount(VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE), UserTableSync::get(_userId), -1, 1);
			return gua.empty() ? NULL : gua.front();
		}
	}
}

