
/** VinciBikeRentalModule class implementation.
	@file VinciBikeRentalModule.cpp

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

#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/Profile.h"
#include "12_security/GlobalRight.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Currency.h"
#include "57_accounting/CurrencyTableSync.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace security;
	using namespace accounts;
	using namespace util;

	namespace vinci
	{
		boost::shared_ptr<Account> VinciBikeRentalModule::_freeLockRent;
		shared_ptr<User> VinciBikeRentalModule::_vinciUser;
		shared_ptr<Profile> VinciBikeRentalModule::_adminProfile;
		shared_ptr<Profile> VinciBikeRentalModule::_operatorProfile;
		shared_ptr<Profile> VinciBikeRentalModule::_vinciProfile;
		shared_ptr<Profile> VinciBikeRentalModule::_vinciCustomerProfile;
		shared_ptr<Currency> VinciBikeRentalModule::_euroCurrency;

		const string VinciBikeRentalModule::CSS_LIMITED_HEIGHT = "limitedheight";

		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE = "4111";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE = "4117";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE = "4119";
		const std::string VinciBikeRentalModule::VINCI_AVAILABLE_BIKES_STOCKS_ACCOUNT_CODE = "37101";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE = "70831";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE = "70832";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE = "70833";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_DELAYED_PAYMENTS_ACCOUNT_CODE = "763";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_UNRETURNED_BIKE_ACCOUNT_CODE = "707";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CHECKS_ACCOUNT_CODE = "5112";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE = "5331";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE = "5332";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CREDIT_CARD_ACCOUNT_CODE = "5121";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CASH_ACCOUNT_CODE = "532";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE = "59";

		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_USER = "Vinci";
		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_PROFILE = "VinciAccounts";

		const std::string VinciBikeRentalModule::VINCI_ADMIN_PROFILE = "VinciAdministrator";
		const std::string VinciBikeRentalModule::VINCI_OPERATOR_PROFILE = "VinciOperator";

		// Currencies
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO_NAME = "Euro";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO = "EUR";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_BIKE_NAME = "velo(s)";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_BIKE = "_bike";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING_NAME = "validation(s)";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING = "_punching";

		// Rights
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_PROFILE = "VinciCustomerWithoutAccess";
		
		void VinciBikeRentalModule::initialize()
		{
			// Profile for bike rental administrator user
			vector<shared_ptr<Profile> > profiles = ProfileTableSync::search(VINCI_ADMIN_PROFILE);
			if (profiles.size() == 0)
				_adminProfile.reset(new Profile);
			else
				_adminProfile = profiles.front();
			_adminProfile->setName(VINCI_ADMIN_PROFILE);
			shared_ptr<Right> r = Factory<Right>::create<GlobalRight>();
			r->setPrivateLevel(Right::DELETE);
			r->setPublicLevel(Right::DELETE);
			_adminProfile->cleanRights();
			_adminProfile->addRight(r);
			ProfileTableSync::save(_adminProfile.get ());

			// Profile for bike rental operator user
			profiles = ProfileTableSync::search(VINCI_OPERATOR_PROFILE);
			if (profiles.size() == 0)
				_operatorProfile.reset(new Profile);
			else
				_operatorProfile = profiles.front();
			_operatorProfile->setName(VINCI_OPERATOR_PROFILE);
			r = Factory<Right>::create<GlobalRight>();
			r->setPrivateLevel(Right::USE);
			r->setPublicLevel(Right::USE);
			_operatorProfile->cleanRights();
			_operatorProfile->addRight(r);
			ProfileTableSync::save(_operatorProfile.get ());

			// Profile for virtual owner user
			profiles = ProfileTableSync::search(VINCI_ACCOUNTING_PROFILE);
			if (profiles.size() == 0)
				_vinciProfile.reset(new Profile);
			else
				_vinciProfile = profiles.front();
			_vinciProfile->setName(VINCI_ACCOUNTING_PROFILE);
			r = Factory<Right>::create<GlobalRight>();
			r->setPrivateLevel(Right::FORBIDDEN);
			r->setPublicLevel(Right::FORBIDDEN);
			_vinciProfile->cleanRights();
			_vinciProfile->addRight(r);
			ProfileTableSync::save(_vinciProfile.get ());
		
			// Virtual owner user
			vector<shared_ptr<User> > users = UserTableSync::search(VINCI_ACCOUNTING_USER, VINCI_ACCOUNTING_USER, _vinciProfile);
			if (users.size() == 0)
			{
				_vinciUser.reset(new User);
				_vinciUser->setName(VINCI_ACCOUNTING_USER);
				_vinciUser->setLogin(VINCI_ACCOUNTING_USER);
				_vinciUser->setProfile(_vinciProfile);
				UserTableSync::save(_vinciUser.get ());
			}
			else
			{
				_vinciUser = users.front();
			}

			// Required currencies
			vector<shared_ptr<Currency> > currencies = CurrencyTableSync::search(VINCI_CURRENCY_EURO_NAME, VINCI_CURRENCY_EURO);
			if (currencies.size() == 0)
			{
				_euroCurrency.reset(new Currency);
				_euroCurrency->setName(VINCI_CURRENCY_EURO_NAME);
				_euroCurrency->setSymbol(VINCI_CURRENCY_EURO);
				CurrencyTableSync::save(_euroCurrency.get ());
			}
			else
				_euroCurrency = currencies.front();

			shared_ptr<Currency> bikeCurrency;
			currencies = CurrencyTableSync::search(VINCI_CURRENCY_BIKE_NAME, VINCI_CURRENCY_BIKE);
			if (currencies.size() == 0)
			{
				bikeCurrency.reset(new Currency);
				bikeCurrency->setName(VINCI_CURRENCY_BIKE_NAME);
				bikeCurrency->setSymbol(VINCI_CURRENCY_BIKE);
				CurrencyTableSync::save(bikeCurrency.get ());
			}
			else
				bikeCurrency = currencies.front();

			shared_ptr<Currency> ticketCurrency;
			currencies = CurrencyTableSync::search(VINCI_CURRENCY_TICKET_PUNCHING_NAME, VINCI_CURRENCY_TICKET_PUNCHING);
			if (currencies.size() == 0)
			{
				ticketCurrency.reset(new Currency);
				ticketCurrency->setName(VINCI_CURRENCY_TICKET_PUNCHING_NAME);
				ticketCurrency->setSymbol(VINCI_CURRENCY_TICKET_PUNCHING);
				CurrencyTableSync::save(ticketCurrency.get ());
			}
			else
				ticketCurrency = currencies.front();

			// Guarantee accounts
			vector<shared_ptr<Account> > accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				shared_ptr<Account> guaranteeAccount(new Account);
				guaranteeAccount->setRightClassNumber(VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE);
				guaranteeAccount->setRightCurrency(_euroCurrency);
				guaranteeAccount->setLeftCurrency(_euroCurrency);
				guaranteeAccount->setRightUserId(_vinciUser->getKey());
				AccountTableSync::save(guaranteeAccount.get ());
			}

			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				shared_ptr<Account> checkGuaranteeAccount(new Account);
				checkGuaranteeAccount->setRightClassNumber(VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE);
				checkGuaranteeAccount->setRightCurrency(_euroCurrency);
				checkGuaranteeAccount->setLeftCurrency(_euroCurrency);
				checkGuaranteeAccount->setRightUserId(_vinciUser->getKey());
				AccountTableSync::save(checkGuaranteeAccount.get ());
			}

			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				shared_ptr<Account> cardGuaranteeAccount(new Account);
				cardGuaranteeAccount->setRightClassNumber(VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE);
				cardGuaranteeAccount->setRightCurrency(_euroCurrency);
				cardGuaranteeAccount->setLeftCurrency(_euroCurrency);
				cardGuaranteeAccount->setRightUserId(_vinciUser->getKey());
				AccountTableSync::save(cardGuaranteeAccount.get ());
			}

			// Customer accounts
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				shared_ptr<Account> customerTicketAccount(new Account);
				customerTicketAccount->setRightClassNumber(VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE);
				customerTicketAccount->setRightCurrency(ticketCurrency);
				customerTicketAccount->setLeftCurrency(ticketCurrency);
				customerTicketAccount->setRightUserId(_vinciUser->getKey());
				AccountTableSync::save(customerTicketAccount.get ());
			}

			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				shared_ptr<Account> customerEuroAccount(new Account);
				customerEuroAccount->setRightClassNumber(VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE);
				customerEuroAccount->setRightCurrency(ticketCurrency);
				customerEuroAccount->setLeftCurrency(ticketCurrency);
				customerEuroAccount->setRightUserId(_vinciUser->getKey());
				AccountTableSync::save(customerEuroAccount.get ());
			}

			// Services accounts
			shared_ptr<Account> ticketBikeRent;
			accounts = AccountTableSync::search(getVinciUser()->getKey(), VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE, 0, "");
			if (accounts.empty())
				ticketBikeRent.reset(new Account);
			else
				ticketBikeRent = accounts.front();
			ticketBikeRent->setRightClassNumber(VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);
			ticketBikeRent->setRightCurrency(ticketCurrency);
			ticketBikeRent->setLeftCurrency(ticketCurrency);
			ticketBikeRent->setRightUserId(_vinciUser->getKey());
			AccountTableSync::save(ticketBikeRent.get ());
			
			accounts = AccountTableSync::search(getVinciUser()->getKey(), VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE, 0, "");
			if (accounts.empty())
				_freeLockRent.reset(new Account);
			else
				_freeLockRent = accounts.front();
			_freeLockRent->setRightClassNumber(VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE);
			_freeLockRent->setRightCurrency(_euroCurrency);
			_freeLockRent->setLeftCurrency(_euroCurrency);
			_freeLockRent->setRightUserId(_vinciUser->getKey());
			AccountTableSync::save(_freeLockRent.get ());
			

			// Payment accounts
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				shared_ptr<Account> ticketPunchings(new Account);
				ticketPunchings->setRightClassNumber(VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE);
				ticketPunchings->setRightCurrency(ticketCurrency);
				ticketPunchings->setLeftCurrency(ticketCurrency);
				ticketPunchings->setRightUserId(_vinciUser->getKey());
				AccountTableSync::save(ticketPunchings.get ());
			}

			// Stock accounts
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_AVAILABLE_BIKES_STOCKS_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				shared_ptr<Account> bikeStockAccount(new Account);
				bikeStockAccount->setRightClassNumber(VINCI_AVAILABLE_BIKES_STOCKS_ACCOUNT_CODE);
				bikeStockAccount->setRightCurrency(bikeCurrency);
				bikeStockAccount->setLeftCurrency(bikeCurrency);
				bikeStockAccount->setRightUserId(_vinciUser->getKey());
				AccountTableSync::save(bikeStockAccount.get ());
			}


			
			// Special profile for customers
			profiles = ProfileTableSync::search(VINCI_CUSTOMER_PROFILE);
			if (profiles.size() == 0)
				_vinciCustomerProfile.reset(new Profile);
			else
				_vinciCustomerProfile = profiles.front();
			_vinciCustomerProfile->setName(VINCI_CUSTOMER_PROFILE);
			r = Factory<Right>::create<GlobalRight>();
			r->setPrivateLevel(Right::FORBIDDEN);
			r->setPublicLevel(Right::FORBIDDEN);
			_vinciCustomerProfile->cleanRights();
			_vinciCustomerProfile->addRight(r);
			ProfileTableSync::save(_vinciCustomerProfile.get ());
		}

		shared_ptr<const Profile> VinciBikeRentalModule::getCustomerProfile()
		{
			return _vinciCustomerProfile;
		}

		shared_ptr<const User> VinciBikeRentalModule::getVinciUser()
		{
			return _vinciUser;
		}

		shared_ptr<const Currency> VinciBikeRentalModule::getEuroCurrency()
		{
			return _euroCurrency;
		}

		shared_ptr<Account> VinciBikeRentalModule::getAccount(const std::string& code)
		{
			vector<shared_ptr<Account> > accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), code, 0, "");
			if (accounts.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the " + code + " account is missing");
			return accounts.front();
		}

		shared_ptr<const Account> VinciBikeRentalModule::getFreeLockRentServiceAccount()
		{
			return _freeLockRent;
		}
	}
}
