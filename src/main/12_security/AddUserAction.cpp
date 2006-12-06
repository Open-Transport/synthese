
#include "01_util/Conversion.h"

#include "12_security/SecurityModule.h"
#include "12_security/Profile.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"
#include "12_security/UserException.h"
#include "12_security/AddUserAction.h"

#include "30_server/ServerModule.h"

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace security
	{
		const std::string AddUserAction::PARAMETER_NAME = Action::PARAMETER_PREFIX + "nm";
		const std::string AddUserAction::PARAMETER_LOGIN = Action::PARAMETER_PREFIX + "lg";
		const std::string AddUserAction::PARAMETER_PROFILE_ID = Action::PARAMETER_PREFIX + "pid";

		Request::ParametersMap AddUserAction::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_NAME, _name));
			map.insert(make_pair(PARAMETER_LOGIN, _login));
			map.insert(make_pair(PARAMETER_PROFILE_ID, Conversion::ToString(_profile->getKey())));
			return map;
		}

		void AddUserAction::setFromParametersMap(Request::ParametersMap& map )
		{
			Request::ParametersMap::const_iterator it;
			it = map.find(PARAMETER_NAME);
			if (it != map.end())
				_name = it->second;
			it = map.find(PARAMETER_LOGIN);
			if (it != map.end())
				_login = it->second;
			it = map.find(PARAMETER_PROFILE_ID);
			if (it != map.end())
			{
				if (SecurityModule::getProfiles().contains(Conversion::ToLongLong(it->second)))
					_profile = SecurityModule::getProfiles().get(Conversion::ToLongLong(it->second));
				else
					throw UserException("Profile not found");
			}

		}

		void AddUserAction::run()
		{
			User* user = new User;
			user->setLogin(_login);
			user->setName(_name);
			user->setProfile(_profile);
			UserTableSync::save(ServerModule::getSQLiteThread(), user);
		}
	}
}