
#include "12_security/User.h"
#include "12_security/UserException.h"
#include "12_security/UserTableSync.h"
#include "12_security/UserTableSyncException.h"

#include "30_server/ActionException.h"
#include "30_server/Session.h"
#include "30_server/ServerModule.h"
#include "30_server/LoginAction.h"

namespace synthese
{
	using namespace security;

	namespace server
	{
		const std::string LoginAction::PARAMETER_LOGIN = "login";
		const std::string LoginAction::PARAMETER_PASSWORD = "pwd";

		server::Request::ParametersMap LoginAction::getParametersMap() const
		{
			Request::ParametersMap map;
			return map;
		}

		void LoginAction::setFromParametersMap(Request::ParametersMap& map )
		{
			Request::ParametersMap::iterator it = map.find(Action::PARAMETER_PREFIX + PARAMETER_LOGIN);
			if (it == map.end())
				throw ActionException("Login field not found");
			_login = it->second;
			map.erase(it);

			it = map.find(Action::PARAMETER_PREFIX + PARAMETER_PASSWORD);
			if (it == map.end())
				throw ActionException("Password field not found");
			_password = it->second;
			map.erase(it);
		}

		void LoginAction::run()
		{
			// Fetch user
			try
			{
				User* user = UserTableSync::getUser(ServerModule::getSQLiteThread(), _login);
				user->verifyPassword(_password);
				Session* session = new Session(_request->getIP());
				session->setUser(user);
				_request->setSession(session);
			}
			catch (UserTableSyncException e)
			{
				throw ActionException("Bad user");
			}
			catch (UserException e)
			{
				throw ActionException("Bad password");
			}
		}
	}
}
