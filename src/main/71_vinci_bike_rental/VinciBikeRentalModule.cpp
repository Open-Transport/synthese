
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

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"
#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"

#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/Profile.h"
#include "12_security/GlobalRight.h"
#include "12_security/Types.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Currency.h"
#include "57_accounting/CurrencyTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace security;
	using namespace accounts;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,vinci::VinciBikeRentalModule>::FACTORY_KEY("71_vinci_bike_rental");
	}

	namespace vinci
	{
		shared_ptr<Account> VinciBikeRentalModule::_freeLockRent;
		shared_ptr<Account> VinciBikeRentalModule::_stockChargeAccount;
		shared_ptr<User> VinciBikeRentalModule::_vinciUser;
		shared_ptr<Profile> VinciBikeRentalModule::_adminProfile;
		shared_ptr<Profile> VinciBikeRentalModule::_operatorProfile;
		shared_ptr<Profile> VinciBikeRentalModule::_vinciProfile;
		shared_ptr<Profile> VinciBikeRentalModule::_vinciCustomerProfile;
		shared_ptr<Currency> VinciBikeRentalModule::_euroCurrency;
		shared_ptr<Currency> VinciBikeRentalModule::_objectCurrency;
		VinciBikeRentalModule::_SessionsSitesMap VinciBikeRentalModule::_sessionsSites;
		shared_ptr<Currency> VinciBikeRentalModule::_ticketCurrency;

		const string VinciBikeRentalModule::CSS_LIMITED_HEIGHT = "limitedheight";

		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE = "4111";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE = "4117";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE = "4119";
		
		const std::string VinciBikeRentalModule::VINCI_STOCK_CODE_PREFIX("3");
		const std::string VinciBikeRentalModule::VINCI_STOCK_CODE("3%");
		const std::string VinciBikeRentalModule::VINCI_AVAILABLE_BIKES_STOCKS_ACCOUNT_CODE = "30831";

		const std::string VinciBikeRentalModule::VINCI_SERVICES_CODE_PREFIX("7");
		const std::string VinciBikeRentalModule::VINCI_SERVICES_CODE("7%");
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE = "70831";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE = "70832";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE = "70833";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_DELAYED_PAYMENTS_ACCOUNT_CODE = "763";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_UNRETURNED_BIKE_ACCOUNT_CODE = "707";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_SALES_CODE("701");
		
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CODE("5%");
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CHECKS_ACCOUNT_CODE = "5112";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE = "5331";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE = "5332";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CREDIT_CARD_ACCOUNT_CODE = "5121";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CASH_ACCOUNT_CODE = "532";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE = "59";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CUSTOM_CODE("533");

		const std::string VinciBikeRentalModule::VINCI_CHARGE_STOCK_CHANGE_CODE("603");
		
		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_USER = "Vinci";
		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_PROFILE = "VinciAccounts";

		const std::string VinciBikeRentalModule::VINCI_ADMIN_PROFILE = "VinciAdministrator";
		const std::string VinciBikeRentalModule::VINCI_OPERATOR_PROFILE = "VinciOperator";

		// Currencies
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO_NAME = "Euro";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO = "EUR";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_OBJECT_NAME = "objet(s)";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_OBJECT = "_obj";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING_NAME = "validation(s)";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING = "_punching";

		// Rights
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_PROFILE = "VinciCustomerWithoutAccess";
		
		void VinciBikeRentalModule::initialize()
		{
			// Profile for bike rental administrator user
			vector<shared_ptr<Profile> > profiles = ProfileTableSync::search(string(), VINCI_ADMIN_PROFILE);
			if (profiles.size() == 0)
				_adminProfile.reset(new Profile);
			else
				_adminProfile = profiles.front();
			_adminProfile->setName(VINCI_ADMIN_PROFILE);
			shared_ptr<Right> r(new GlobalRight);
			r->setPrivateLevel(DELETE_RIGHT);
			r->setPublicLevel(DELETE_RIGHT);
			_adminProfile->cleanRights();
			_adminProfile->addRight(r);
			ProfileTableSync::save(_adminProfile.get ());

			// Profile for bike rental operator user
			profiles = ProfileTableSync::search(string(), VINCI_OPERATOR_PROFILE);
			if (profiles.size() == 0)
				_operatorProfile.reset(new Profile);
			else
				_operatorProfile = profiles.front();
			_operatorProfile->setName(VINCI_OPERATOR_PROFILE);
			r.reset(new GlobalRight);
			r->setPrivateLevel(USE);
			r->setPublicLevel(USE);
			_operatorProfile->cleanRights();
			_operatorProfile->addRight(r);
			ProfileTableSync::save(_operatorProfile.get ());

			// Profile for virtual owner user
			profiles = ProfileTableSync::search(string(), VINCI_ACCOUNTING_PROFILE);
			if (profiles.size() == 0)
				_vinciProfile.reset(new Profile);
			else
				_vinciProfile = profiles.front();
			_vinciProfile->setName(VINCI_ACCOUNTING_PROFILE);
			r.reset(new GlobalRight);
			r->setPrivateLevel(FORBIDDEN);
			r->setPublicLevel(FORBIDDEN);
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
				_vinciUser->setProfile(_vinciProfile.get());
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

			currencies = CurrencyTableSync::search(VINCI_CURRENCY_OBJECT_NAME, VINCI_CURRENCY_OBJECT);
			if (currencies.empty())
				_objectCurrency.reset(new Currency);
			else
				_objectCurrency = currencies.front();
			_objectCurrency->setName(VINCI_CURRENCY_OBJECT_NAME);
			_objectCurrency->setSymbol(VINCI_CURRENCY_OBJECT);
			CurrencyTableSync::save(_objectCurrency.get ());
				

			currencies = CurrencyTableSync::search(VINCI_CURRENCY_TICKET_PUNCHING_NAME, VINCI_CURRENCY_TICKET_PUNCHING);
			if (currencies.size() == 0)
			{
				_ticketCurrency.reset(new Currency);
				_ticketCurrency->setName(VINCI_CURRENCY_TICKET_PUNCHING_NAME);
				_ticketCurrency->setSymbol(VINCI_CURRENCY_TICKET_PUNCHING);
				CurrencyTableSync::save(_ticketCurrency.get ());
			}
			else
				_ticketCurrency = currencies.front();

			// Guarantee accounts
			vector<shared_ptr<Account> > accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE, 0, "");
			shared_ptr<Account> guaranteeAccount;
			if (accounts.empty())
				guaranteeAccount.reset(new Account);
			else
				guaranteeAccount = accounts.front();
			guaranteeAccount->setRightClassNumber(VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE);
			guaranteeAccount->setRightCurrency(_euroCurrency.get());
			guaranteeAccount->setLeftCurrency(_euroCurrency.get());
			guaranteeAccount->setRightUserId(_vinciUser->getKey());
			guaranteeAccount->setName("Clients : cautions dues");
			guaranteeAccount->setLocked(true);
			AccountTableSync::save(guaranteeAccount.get());

			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE, 0, "");
			shared_ptr<Account> checkGuaranteeAccount;
			if (accounts.empty())
				checkGuaranteeAccount.reset(new Account);
			else
				checkGuaranteeAccount = accounts.front();
			checkGuaranteeAccount->setRightClassNumber(VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE);
			checkGuaranteeAccount->setRightCurrency(_euroCurrency.get());
			checkGuaranteeAccount->setLeftCurrency(_euroCurrency.get());
			checkGuaranteeAccount->setRightUserId(_vinciUser->getKey());
			checkGuaranteeAccount->setName("Caisse : Chèques de cautions");
			checkGuaranteeAccount->setLocked(true);
			AccountTableSync::save(checkGuaranteeAccount.get ());

			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE, 0, "");
			shared_ptr<Account> cardGuaranteeAccount;
			if (accounts.empty())
				cardGuaranteeAccount.reset(new Account);
			else
				cardGuaranteeAccount = accounts.front();
			cardGuaranteeAccount->setRightClassNumber(VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE);
			cardGuaranteeAccount->setRightCurrency(_euroCurrency.get());
			cardGuaranteeAccount->setLeftCurrency(_euroCurrency.get());
			cardGuaranteeAccount->setRightUserId(_vinciUser->getKey());
			cardGuaranteeAccount->setName("Caisse : Empruntes de carte de crédit pour caution");
			cardGuaranteeAccount->setLocked(true);
			AccountTableSync::save(cardGuaranteeAccount.get ());

			// Customer accounts
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE, 0, "");
			shared_ptr<Account> customerTicketAccount;
			if (accounts.empty())
				customerTicketAccount.reset(new Account);
			else
				customerTicketAccount = accounts.front();
			customerTicketAccount->setRightClassNumber(VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE);
			customerTicketAccount->setRightCurrency(_ticketCurrency.get());
			customerTicketAccount->setLeftCurrency(_ticketCurrency.get());
			customerTicketAccount->setRightUserId(_vinciUser->getKey());
			customerTicketAccount->setName("Clients : prestations en titres de transport");
			customerTicketAccount->setLocked(true);
			AccountTableSync::save(customerTicketAccount.get ());

			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE, 0, "");
			shared_ptr<Account> customerEuroAccount;
			if (accounts.empty())
				customerEuroAccount.reset(new Account);
			else
				customerEuroAccount = accounts.front();
			customerEuroAccount->setRightClassNumber(VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE);
			customerEuroAccount->setRightCurrency(_ticketCurrency.get());
			customerEuroAccount->setLeftCurrency(_ticketCurrency.get());
			customerEuroAccount->setRightUserId(_vinciUser->getKey());
			customerEuroAccount->setName("Clients : prestations en euros");
			customerEuroAccount->setLocked(true);
			AccountTableSync::save(customerEuroAccount.get ());

			// Services accounts
			shared_ptr<Account> ticketBikeRent;
			accounts = AccountTableSync::search(getVinciUser()->getKey(), VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE, 0, "");
			if (accounts.empty())
				ticketBikeRent.reset(new Account);
			else
				ticketBikeRent = accounts.front();
			ticketBikeRent->setRightClassNumber(VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);
			ticketBikeRent->setRightCurrency(_ticketCurrency.get());
			ticketBikeRent->setLeftCurrency(_ticketCurrency.get());
			ticketBikeRent->setRightUserId(_vinciUser->getKey());
			ticketBikeRent->setName("Produits : locations de vélo en titres de transport");
			ticketBikeRent->setLocked(true);
			AccountTableSync::save(ticketBikeRent.get ());
			
			accounts = AccountTableSync::search(getVinciUser()->getKey(), VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE, 0, "");
			if (accounts.empty())
				_freeLockRent.reset(new Account);
			else
				_freeLockRent = accounts.front();
			_freeLockRent->setRightClassNumber(VINCI_SERVICES_LOCK_RENT_FREE_ACCOUNT_CODE);
			_freeLockRent->setRightCurrency(_euroCurrency.get());
			_freeLockRent->setLeftCurrency(_euroCurrency.get());
			_freeLockRent->setRightUserId(_vinciUser->getKey());
			_freeLockRent->setName("Produits : locations d'antivol");
			_freeLockRent->setLocked(true);
			AccountTableSync::save(_freeLockRent.get ());

			shared_ptr<Account> financialBikeRent;
			accounts = AccountTableSync::search(getVinciUser()->getKey(), VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE, 0, "");
			if (accounts.empty())
				financialBikeRent.reset(new Account);
			else
				financialBikeRent = accounts.front();
			financialBikeRent->setRightClassNumber(VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE);
			financialBikeRent->setRightCurrency(_euroCurrency.get());
			financialBikeRent->setLeftCurrency(_euroCurrency.get());
			financialBikeRent->setRightUserId(_vinciUser->getKey());
			financialBikeRent->setName("Produits : locations de vélo en euros");
			financialBikeRent->setLocked(true);
			AccountTableSync::save(financialBikeRent.get ());


			// Payment accounts
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE, 0, "");
			shared_ptr<Account> ticketPunchings;
			if (accounts.empty())
				ticketPunchings.reset(new Account);
			else
				ticketPunchings = accounts.front();
			ticketPunchings->setRightClassNumber(VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE);
			ticketPunchings->setRightCurrency(_ticketCurrency.get());
			ticketPunchings->setLeftCurrency(_ticketCurrency.get());
			ticketPunchings->setRightUserId(_vinciUser->getKey());
			ticketPunchings->setName("Caisse : validations de titres de transport");
			ticketPunchings->setLocked(true);
			AccountTableSync::save(ticketPunchings.get ());

			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_CASH_ACCOUNT_CODE, 0, "");
			shared_ptr<Account> cash;
			if (accounts.empty())
				cash.reset(new Account);
			else
				cash = accounts.front();
			cash->setRightClassNumber(VINCI_CHANGE_CASH_ACCOUNT_CODE);
			cash->setRightCurrency(_euroCurrency.get());
			cash->setLeftCurrency(_euroCurrency.get());
			cash->setRightUserId(_vinciUser->getKey());
			cash->setName("Caisse : monnaie euros");
			cash->setLocked(true);
			AccountTableSync::save(cash.get ());

			// Stock accounts
			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_AVAILABLE_BIKES_STOCKS_ACCOUNT_CODE, 0, "");
			shared_ptr<Account> bikeStockAccount;
			if (accounts.empty())
				bikeStockAccount.reset(new Account);
			else
				bikeStockAccount = accounts.front();
			bikeStockAccount->setRightClassNumber(VINCI_AVAILABLE_BIKES_STOCKS_ACCOUNT_CODE);
			bikeStockAccount->setRightCurrency(_objectCurrency.get());
			bikeStockAccount->setLeftCurrency(_objectCurrency.get());
			bikeStockAccount->setRightUserId(_vinciUser->getKey());
			bikeStockAccount->setName("Stocks : vélos");
			bikeStockAccount->setLocked(true);
			AccountTableSync::save(bikeStockAccount.get ());


			accounts = AccountTableSync::search(VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHARGE_STOCK_CHANGE_CODE, 0, "");
			if (accounts.empty())
				_stockChargeAccount.reset(new Account);
			else
				_stockChargeAccount = accounts.front();
			_stockChargeAccount->setRightClassNumber(VINCI_CHARGE_STOCK_CHANGE_CODE);
			_stockChargeAccount->setRightCurrency(_objectCurrency.get());
			_stockChargeAccount->setLeftCurrency(_objectCurrency.get());
			_stockChargeAccount->setRightUserId(_vinciUser->getKey());
			_stockChargeAccount->setName("Charge : variation de stock");
			_stockChargeAccount->setLocked(true);
			AccountTableSync::save(_stockChargeAccount.get ());
			
			// Special profile for customers
			profiles = ProfileTableSync::search(string(), VINCI_CUSTOMER_PROFILE);
			if (profiles.size() == 0)
				_vinciCustomerProfile.reset(new Profile);
			else
				_vinciCustomerProfile = profiles.front();
			_vinciCustomerProfile->setName(VINCI_CUSTOMER_PROFILE);
			r.reset(new GlobalRight);
			r->setPrivateLevel(FORBIDDEN);
			r->setPublicLevel(FORBIDDEN);
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

		boost::shared_ptr<const accounts::Account> VinciBikeRentalModule::getStockChargeAccount()
		{
			return _stockChargeAccount;
		}

		boost::shared_ptr<const accounts::Currency> VinciBikeRentalModule::getObjectCurrency()
		{
			return _objectCurrency;
		}

		void VinciBikeRentalModule::AddSessionSite( const server::Session* session, uid siteId )
		{
			_sessionsSites[session] = siteId;
		}

		uid VinciBikeRentalModule::GetSessionSite( const server::Session* session )
		{
			_SessionsSitesMap::const_iterator it(_sessionsSites.find(session));
			return (it != _sessionsSites.end()) ? it->second : UNKNOWN_VALUE;
		}

		std::vector<std::pair<uid, std::string> > VinciBikeRentalModule::GetSitesName(uid differentValue)
		{
			string all("%");
			vector<shared_ptr<VinciSite> > sites(VinciSiteTableSync::search(all, differentValue, 0, 0, true, true, false));
			vector<pair<uid, string> > result;
			for (vector<shared_ptr<VinciSite> >::const_iterator it(sites.begin()); it != sites.end(); ++it)
				result.push_back(make_pair((*it)->getKey(), (*it)->getName()));
			return result;
		}
	}
}
