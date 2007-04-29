
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

#include <boost/shared_ptr.hpp>

#include "12_security/User.h"
#include "12_security/UserException.h"
#include "12_security/UserTableSync.h"
#include "12_security/UserTableSyncException.h"
#include "12_security/SecurityLog.h"

#include "30_server/ActionException.h"
#include "30_server/Session.h"
#include "30_server/Request.h"
#include "30_server/ServerModule.h"
#include "30_server/LoginAction.h"

using boost::shared_ptr;

namespace synthese
{
	using namespace security;

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
			ParametersMap::const_iterator it = map.find(PARAMETER_LOGIN);
			if (it == map.end())
				throw ActionException("Login field not found");
			_login = it->second;

			it = map.find(PARAMETER_PASSWORD);
			if (it == map.end())
				throw ActionException("Password field not found");
			_password = it->second;
		}

		void LoginAction::run()
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

				Session* session = new Session(_request->getIP());
				session->setUser(user);
				_request->setSession(session);

				SecurityLog::addUserLogin(user);
			}
			catch (UserTableSyncException e)
			{
				throw ActionException("Utilisateur introuvable");
			}
			catch (UserException e)
			{
				throw ActionException("Mot de passe erroné");
			}
		}

		bool LoginAction::_beforeSessionControl() const
		{
			return true;
		}
	}
}
