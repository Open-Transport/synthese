
#include "12_security/User.h"
#include "12_security/UserException.h"
#include "12_security/UserTableSync.h"
#include "12_security/UserTableSyncException.h"

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

		void LoginAction::setFromParametersMap( server::Request::ParametersMap& map )
		{
			Request::ParametersMap::const_iterator it = map.find(PARAMETER_LOGIN);
			_login = it->second;
			it = map.find(PARAMETER_PASSWORD);
			_password = it->second;
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
				/// @todo See what to do here
			}
			catch (UserException e)
			{
				/// @todo See what to do here
			}
		}
	}
}