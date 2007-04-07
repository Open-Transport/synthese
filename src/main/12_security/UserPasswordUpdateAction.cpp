
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

#include "02_db/DBEmptyResultException.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "12_security/UserPasswordUpdateAction.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"


using namespace std;

namespace synthese
{
	using namespace server;
	using namespace db;
	
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
				_user = UserTableSync::get(_request->getObjectId());

				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_PASS1);
				if (it == map.end())
					throw ActionException("Mot de passe non spécifié");
				_password = it->second;

				it = map.find(PARAMETER_PASS2);
				if (it == map.end() || it->second != _password)
					throw ActionException("Les mots de passe entrés ne sont pas identiques");
			}
			catch (DBEmptyResultException e)
			{
				throw ActionException("Utilisateur introuvable");
			}
		}

		UserPasswordUpdateAction::UserPasswordUpdateAction()
			: Action()
			, _user(NULL)
		{}

		void UserPasswordUpdateAction::run()
		{
			_user->setPassword(_password);
			UserTableSync::save(_user);
		}

		UserPasswordUpdateAction::~UserPasswordUpdateAction()
		{
			delete _user;
		}
	}
}
