
/** LoginAction class implementation.
	@file LoginAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

// At first to avoid the Windows bug "WinSock.h has already been included"
#include "ServerModule.h"

#include "LoginAction.h"
#include "ActionException.h"
#include "Session.h"
#include "Request.h"
#include "ParametersMap.h"

#include <boost/shared_ptr.hpp>

#include "User.h"
#include "UserException.h"
#include "UserTableSync.h"
#include "UserTableSyncException.h"
#include "SecurityLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace security;

	template<> const string util::FactorableTemplate<server::Action, server::LoginAction>::FACTORY_KEY("login");

	namespace server
	{
		const std::string LoginAction::PARAMETER_LOGIN = Action_PARAMETER_PREFIX + "login";
		const std::string LoginAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "pwd";

		ParametersMap LoginAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void LoginAction::_setFromParametersMap(const ParametersMap& map )
		{
			try
			{
				_login = map.get<string>(PARAMETER_LOGIN);
				_password = map.get<string>(PARAMETER_PASSWORD);
			}
			catch(ParametersMap::MissingParameterException e)
			{
				throw ActionException(e, *this);
			}
		}

		void LoginAction::run(Request& request)
		{
			// Fetch user
			try
			{
				if (_login.empty() || _password.empty())
					throw ActionException("Champ utilisateur ou mot de passe vide");
				
				shared_ptr<User> user = UserTableSync::getUserFromLogin(_login);
				user->verifyPassword(_password);
				
				if (!user->getConnectionAllowed())
					throw ActionException("Connexion impossible");

				Session* session = new Session(request.getIP());
				session->setUser(user);
				request.setSession(session);

				SecurityLog::addUserLogin(user.get());
			}
			catch (UserTableSyncException e)
			{
				throw ActionException("Utilisateur introuvable");
			}
			catch (UserException e)
			{
				throw ActionException("Mot de passe erron�");
			}
		}

		bool LoginAction::_isSessionRequired() const
		{
			return false;
		}

		void LoginAction::setLogin( const std::string& login )
		{
			_login = login;
		}

		void LoginAction::setPassword( const std::string& password )
		{
			_password = password;
		}



		bool LoginAction::isAuthorized(const Profile& profile
		) const {
			return true;
		}
	}
}
