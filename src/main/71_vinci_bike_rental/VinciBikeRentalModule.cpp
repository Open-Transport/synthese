
#include <vector>

#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/Profile.h"

#include "30_server/ServerModule.h"

#include "57_accounting/Account.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/Currency.h"
#include "57_accounting/CurrencyTableSync.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;

namespace synthese
{
	using namespace security;
	using namespace server;
	using namespace accounts;

	namespace vinci
	{
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE = "4111";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE = "4117";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE = "4119";
		const std::string VinciBikeRentalModule::VINCI_STOCKS_BIKE_ACCOUNT_CODE = "371";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_EUROS_ACCOUNT_CODE = "70831";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE = "70832";
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
		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_PROFILE_RIGHTS = "*,*,0,0";

		// Currencies
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO_NAME = "Euro";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO = "EUR";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_BIKE_NAME = "velo(s)";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_BIKE = "_bike";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING_NAME = "validation(s)";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING = "_punching";

		// Rights
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_PROFILE = "VinciCustomerWithoutAccess";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_PROFILE_RIGHTS = "*,*,0,0";

		void VinciBikeRentalModule::initialize()
		{
			// Profile for virtual owner user
			Profile* vinciProfile;
			vector<Profile*> profiles = ProfileTableSync::searchProfiles(ServerModule::getSQLiteThread(), VINCI_ACCOUNTING_PROFILE);
			if (profiles.size() == 0)
			{
				vinciProfile = new Profile;
				vinciProfile->setName(VINCI_ACCOUNTING_PROFILE);
				vinciProfile->setRights(VINCI_ACCOUNTING_PROFILE_RIGHTS);
				ProfileTableSync::save(ServerModule::getSQLiteThread(), vinciProfile);
			}
			else
			{
				vinciProfile = profiles.front();
			}

			// Virtual owner user
			User* vinciUser;
			vector<User*> users = UserTableSync::searchUsers(ServerModule::getSQLiteThread(), VINCI_ACCOUNTING_USER, VINCI_ACCOUNTING_USER, vinciProfile->getKey());
			if (users.size() == 0)
			{
				vinciUser = new User;
				vinciUser->setName(VINCI_ACCOUNTING_USER);
				vinciUser->setLogin(VINCI_ACCOUNTING_USER);
				vinciUser->setProfile(vinciProfile);
				UserTableSync::save(ServerModule::getSQLiteThread(), vinciUser);
			}
			else
			{
				vinciUser = users.front();
			}

			// Required currencies
			Currency* euroCurrency;
			vector<Currency*> currencies = CurrencyTableSync::searchCurrencies(ServerModule::getSQLiteThread(), VINCI_CURRENCY_EURO_NAME, VINCI_CURRENCY_EURO);
			if (currencies.size() == 0)
			{
				euroCurrency = new Currency;
				euroCurrency->setName(VINCI_CURRENCY_EURO_NAME);
				euroCurrency->setSymbol(VINCI_CURRENCY_EURO);
				CurrencyTableSync::save(ServerModule::getSQLiteThread(), euroCurrency);
			}
			else
				euroCurrency = currencies.front();

			Currency* bikeCurrency;
			currencies = CurrencyTableSync::searchCurrencies(ServerModule::getSQLiteThread(), VINCI_CURRENCY_BIKE_NAME, VINCI_CURRENCY_BIKE);
			if (currencies.size() == 0)
			{
				bikeCurrency = new Currency;
				bikeCurrency->setName(VINCI_CURRENCY_BIKE_NAME);
				bikeCurrency->setSymbol(VINCI_CURRENCY_BIKE);
				CurrencyTableSync::save(ServerModule::getSQLiteThread(), bikeCurrency);
			}
			else
				bikeCurrency = currencies.front();

			Currency* ticketCurrency;
			currencies = CurrencyTableSync::searchCurrencies(ServerModule::getSQLiteThread(), VINCI_CURRENCY_TICKET_PUNCHING_NAME, VINCI_CURRENCY_TICKET_PUNCHING);
			if (currencies.size() == 0)
			{
				ticketCurrency = new Currency;
				ticketCurrency->setName(VINCI_CURRENCY_TICKET_PUNCHING_NAME);
				ticketCurrency->setSymbol(VINCI_CURRENCY_TICKET_PUNCHING);
				CurrencyTableSync::save(ServerModule::getSQLiteThread(), ticketCurrency);
			}
			else
				ticketCurrency = currencies.front();

			// Guarantee accounts
			Account* guaranteeAccount;
			vector<Account*> accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				guaranteeAccount = new Account;
				guaranteeAccount->setRightClassNumber(VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE);
				guaranteeAccount->setRightCurrency(euroCurrency);
				guaranteeAccount->setLeftCurrency(euroCurrency);
				guaranteeAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ServerModule::getSQLiteThread(), guaranteeAccount);
			}

			Account* checkGuaranteeAccount;
			accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				checkGuaranteeAccount = new Account;
				checkGuaranteeAccount->setRightClassNumber(VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE);
				checkGuaranteeAccount->setRightCurrency(euroCurrency);
				checkGuaranteeAccount->setLeftCurrency(euroCurrency);
				checkGuaranteeAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ServerModule::getSQLiteThread(), checkGuaranteeAccount);
			}

			Account* cardGuaranteeAccount;
			accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				cardGuaranteeAccount = new Account;
				cardGuaranteeAccount->setRightClassNumber(VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE);
				cardGuaranteeAccount->setRightCurrency(euroCurrency);
				cardGuaranteeAccount->setLeftCurrency(euroCurrency);
				cardGuaranteeAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ServerModule::getSQLiteThread(), cardGuaranteeAccount);
			}

			// Customer accounts
			Account* customerTicketAccount;
			accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				customerTicketAccount = new Account;
				customerTicketAccount->setRightClassNumber(VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE);
				customerTicketAccount->setRightCurrency(ticketCurrency);
				customerTicketAccount->setLeftCurrency(ticketCurrency);
				customerTicketAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ServerModule::getSQLiteThread(), customerTicketAccount);
			}

			Account* customerEuroAccount;
			accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				customerEuroAccount = new Account;
				customerEuroAccount->setRightClassNumber(VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE);
				customerEuroAccount->setRightCurrency(ticketCurrency);
				customerEuroAccount->setLeftCurrency(ticketCurrency);
				customerEuroAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ServerModule::getSQLiteThread(), customerEuroAccount);
			}

			// Services accounts
			Account* ticketBikeRent;
			accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				ticketBikeRent = new Account;
				ticketBikeRent->setRightClassNumber(VINCI_SERVICES_BIKE_RENT_TICKETS_ACCOUNT_CODE);
				ticketBikeRent->setRightCurrency(ticketCurrency);
				ticketBikeRent->setLeftCurrency(ticketCurrency);
				ticketBikeRent->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ServerModule::getSQLiteThread(), ticketBikeRent);
			}

			// Payment accounts
			Account* ticketPunchings;
			accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				ticketPunchings= new Account;
				ticketPunchings->setRightClassNumber(VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE);
				ticketPunchings->setRightCurrency(ticketCurrency);
				ticketPunchings->setLeftCurrency(ticketCurrency);
				ticketPunchings->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ServerModule::getSQLiteThread(), ticketPunchings);
			}

			// Stock accounts
			Account* bikeStockAccount;
			accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_STOCKS_BIKE_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
			{
				bikeStockAccount = new Account;
				bikeStockAccount->setRightClassNumber(VINCI_STOCKS_BIKE_ACCOUNT_CODE);
				bikeStockAccount->setRightCurrency(bikeCurrency);
				bikeStockAccount->setLeftCurrency(bikeCurrency);
				bikeStockAccount->setRightUserId(vinciUser->getKey());
				AccountTableSync::save(ServerModule::getSQLiteThread(), bikeStockAccount);
			}


			
			// Special profile for customers
			Profile* vinciCustomerProfile;
			profiles = ProfileTableSync::searchProfiles(ServerModule::getSQLiteThread(), VINCI_CUSTOMER_PROFILE);
			if (profiles.size() == 0)
			{
				vinciCustomerProfile = new Profile;
				vinciCustomerProfile->setName(VINCI_CUSTOMER_PROFILE);
				vinciCustomerProfile->setRights(VINCI_CUSTOMER_PROFILE_RIGHTS);
				ProfileTableSync::save(ServerModule::getSQLiteThread(), vinciCustomerProfile);
			}
			else
			{
				vinciCustomerProfile = profiles.front();
			}


		}

		Profile* VinciBikeRentalModule::getCustomerProfile()
		{
			vector<Profile*> profiles = ProfileTableSync::searchProfiles(ServerModule::getSQLiteThread(), VINCI_CUSTOMER_PROFILE);
			if (profiles.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the customer profile is missing");
			return profiles.front();
		}

		User* VinciBikeRentalModule::getVinciUser()
		{
			vector<Profile*> profiles = ProfileTableSync::searchProfiles(ServerModule::getSQLiteThread(), VINCI_ACCOUNTING_PROFILE);
			if (profiles.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the rental root user is missing");
			Profile* vinciProfile = profiles.front();
			vector<User*> users = UserTableSync::searchUsers(ServerModule::getSQLiteThread(), VINCI_ACCOUNTING_USER, VINCI_ACCOUNTING_USER, vinciProfile->getKey());
			if (users.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the rental root user is missing");
			return users.front();
		}

		Currency* VinciBikeRentalModule::getEuroCurrency()
		{
			vector<Currency*> currencies = CurrencyTableSync::searchCurrencies(ServerModule::getSQLiteThread(), VINCI_CURRENCY_EURO_NAME, VINCI_CURRENCY_EURO);
			if (currencies.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the euro currency is missing");
			return currencies.front();
		}

		accounts::Account* VinciBikeRentalModule::getGuaranteeAccount()
		{
			vector<Account*> accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the guarantee account is missing");
			return accounts.front();
		}
		 
		accounts::Account* VinciBikeRentalModule::getCheckGuaranteeAccount()
		{
			vector<Account*> accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CHECK_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the check guarantee account is missing");
			return accounts.front();
		}

		accounts::Account* VinciBikeRentalModule::getCardGuaranteeAccount()
		{
			vector<Account*> accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), VinciBikeRentalModule::VINCI_CHANGE_GUARANTEE_CARD_ACCOUNT_CODE, 0, "");
			if (accounts.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the card guarantee account is missing");
			return accounts.front();
		}

		Account* VinciBikeRentalModule::getAccount(const std::string& code)
		{
			vector<Account*> accounts = AccountTableSync::searchAccounts(ServerModule::getSQLiteThread(), VinciBikeRentalModule::getVinciUser()->getKey(), code, 0, "");
			if (accounts.size() == 0)
				throw Exception("Vinci bike rental module incomplete installation : the " + code + " account is missing");
			return accounts.front();
		}

	}
}
