
/** UserPasswordUpdateAction class implementation.
	@file UserPasswordUpdateAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "UserPasswordUpdateAction.h"

#include "Profile.h"
#include "Session.h"
#include "UserTableSync.h"
#include "SecurityRight.h"
#include "SecurityLog.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, security::UserPasswordUpdateAction>::FACTORY_KEY("upua");

	namespace security
	{
		const string UserPasswordUpdateAction::PARAMETER_USER_ID(Action_PARAMETER_PREFIX + "u");
		const string UserPasswordUpdateAction::PARAMETER_PASS1(Action_PARAMETER_PREFIX + "p1");
		const string UserPasswordUpdateAction::PARAMETER_PASS2(Action_PARAMETER_PREFIX + "p2");


		ParametersMap UserPasswordUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_user.get()) map.insert(PARAMETER_USER_ID, _user->getKey());
			return map;
		}

		void UserPasswordUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_USER_ID),
					*_env
				);

				_password = map.get<string>(PARAMETER_PASS1, false);

				string pass2(map.getDefault<string>(PARAMETER_PASS2, string(), false));
				if (pass2 != _password)
					throw ActionException("Les mots de passe entrés ne sont pas identiques");

				if(_password.empty())
					throw ActionException("Le mot de passe ne peut pas être vide");
			}
			catch (ObjectNotFoundException<User>)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}

		void UserPasswordUpdateAction::run(Request& request)
		{
			_user->setPassword(_password);

			UserTableSync::Save(_user.get());

			SecurityLog::addUserAdmin(
				request.getUser().get(),
				_user.get(),
				"Modification du mot de passe"
			);
		}



		bool UserPasswordUpdateAction::isAuthorized(
			const Session* session
		) const {
			return
				session &&
				(	(session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE)) ||
					(session->getUser() != NULL && session->getUser()->getKey() == _user->getKey())
				)
			;
		}


		void UserPasswordUpdateAction::setUser(boost::shared_ptr<User> value)
		{
			_user = value;
		}

		void UserPasswordUpdateAction::setUserC(boost::shared_ptr<const User> value)
		{
			_user = const_pointer_cast<User>(value);
		}
	}
}
