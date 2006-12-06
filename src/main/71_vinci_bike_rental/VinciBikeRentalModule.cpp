
#include <vector>

#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/Profile.h"

#include "30_server/ServerModule.h"

#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

using namespace std;

namespace synthese
{
	using namespace security;
	using namespace server;

	namespace vinci
	{
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_FINANCIAL_ACCOUNT_CODE = "4111";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_GUARANTEES_ACCOUNT_CODE = "4117";
		const std::string VinciBikeRentalModule::VINCI_CUSTOMER_TICKETS_ACCOUNT_CODE = "4119";
		const std::string VinciBikeRentalModule::VINCI_STOCKS_BIKE_ACCOUNT_CODE = "371";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_BIKE_RENT_ACCOUNT_CODE = "7083";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_DELAYED_PAYMENTS_ACCOUNT_CODE = "763";
		const std::string VinciBikeRentalModule::VINCI_SERVICES_UNRETURNED_BIKE_ACCOUNT_CODE = "707";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CHECKS_ACCOUNT_CODE = "5112";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CREDIT_CARD_ACCOUNT_CODE = "5121";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_CASH_ACCOUNT_CODE = "532";
		const std::string VinciBikeRentalModule::VINCI_CHANGE_TICKETS_PUNCHING_ACCOUNT_CODE = "59";

		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_USER = "Vinci";
		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_PROFILE = "VinciAccounts";
		const std::string VinciBikeRentalModule::VINCI_ACCOUNTING_PROFILE_RIGHTS = "*,*,0,0";

		const std::string VinciBikeRentalModule::VINCI_CURRENCY_EURO = "EUR";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_BIKE = "_bike";
		const std::string VinciBikeRentalModule::VINCI_CURRENCY_TICKET_PUNCHING = "_punching";

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
				ProfileTableSync::saveProfile(ServerModule::getSQLiteThread(), vinciProfile);
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


			// Special accounts

		}
	}
}