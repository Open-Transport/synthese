
#include <vector>

#include "12_security/ProfileTableSync.h"
#include "12_security/UserTableSync.h"
#include "12_security/User.h"
#include "12_security/Profile.h"
#include "12_security/SecurityModule.h"

#include "30_server/ServerModule.h"

using namespace std;

namespace synthese
{
	using namespace server;

	namespace security
	{
		const std::string SecurityModule::ROOT_PROFILE = "root";
		const std::string SecurityModule::ROOT_RIGHTS = "*,*,100,100";
		const std::string SecurityModule::ROOT_USER = "root";

		Profile::Registry SecurityModule::_profiles;

		void SecurityModule::initialize()
		{
			Profile* rootProfile;
			vector<Profile*> profiles = ProfileTableSync::searchProfiles(ServerModule::getSQLiteThread(), ROOT_PROFILE);
			if (profiles.size() == 0)
			{
				rootProfile = new Profile;
				rootProfile->setName(ROOT_PROFILE);
				rootProfile->setRights(ROOT_RIGHTS);
				ProfileTableSync::saveProfile(ServerModule::getSQLiteThread(), rootProfile);
			}
			else
				rootProfile = profiles.front();

			vector<User*> users = UserTableSync::searchUsers(ServerModule::getSQLiteThread(), ROOT_USER, ROOT_USER, rootProfile->getKey());
			if (users.size() == 0)
			{
				User* rootUser = new User;
				rootUser->setName(ROOT_USER);
				rootUser->setLogin(ROOT_USER);
				rootUser->setPassword(ROOT_USER);
				rootUser->setProfile(rootProfile);
				UserTableSync::saveUser(ServerModule::getSQLiteThread(), rootUser);
			}
		}

		Profile::Registry& SecurityModule::getProfiles()
		{
			return _profiles;
		}
	}
}