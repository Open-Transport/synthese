
/** UserPasswordUpdateAction class implementation.
	@file UserPasswordUpdateAction.cpp

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

#include "UserPasswordUpdateAction.h"
#include "RequestMissingParameterException.h"
#include "UserTableSync.h"
#include "SecurityRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	
	template<> const string util::FactorableTemplate<Action, security::UserPasswordUpdateAction>::FACTORY_KEY("upua");
	
	namespace security
	{
		const std::string UserPasswordUpdateAction::PARAMETER_PASS1 = Action_PARAMETER_PREFIX + "pass1";
		const std::string UserPasswordUpdateAction::PARAMETER_PASS2 = Action_PARAMETER_PREFIX + "pass2";


		ParametersMap UserPasswordUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void UserPasswordUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::GetEditable(_request->getObjectId(), _env);

				_password = map.getString(PARAMETER_PASS1, true, FACTORY_KEY);

				string pass2(map.getString(PARAMETER_PASS2, false, FACTORY_KEY));
				if (pass2 != _password)
					throw ActionException("Les mots de passe entrés ne sont pas identiques");
			}
			catch (ObjectNotFoundException<User>)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}

		void UserPasswordUpdateAction::run()
		{
			_user->setPassword(_password);
			UserTableSync::Save(_user.get());
		}



		bool UserPasswordUpdateAction::_isAuthorized(
		) const {
			return _request->isAuthorized<SecurityRight>(WRITE) ||
				_request->getUser() != NULL && _request->getUser()->getKey() == _user->getKey();
		}
	}
}
