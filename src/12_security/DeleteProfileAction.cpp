

/** DeleteProfileAction class implementation.
	@file DeleteProfileAction.cpp

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


#include "DeleteProfileAction.h"

#include "12_security/UserTableSync.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/SecurityLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

#include <vector>

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	
	template<> const std::string util::FactorableTemplate<Action,security::DeleteProfileAction>::FACTORY_KEY("dpa");

	namespace security
	{
		ParametersMap DeleteProfileAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void DeleteProfileAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_profile = ProfileTableSync::Get(_request->getObjectId());
			}
			catch(...)
			{
				throw ActionException("Specified Profile not found");
			}

			// Search of child profiles
			Env env;
			ProfileTableSync::Search(env, _profile, 0, 1);
			if (!env.getRegistry<Profile>().empty())
				throw ActionException("Au moins un profil hérite du profil spécifié. La suppression est impossible.");

			// Search of users
			UserTableSync::Search(env, "%","%","%","%", _profile->getKey(), boost::logic::indeterminate, 0, 1);
			if (!env.getRegistry<User>().empty())
				throw ActionException("Au moins un utilisateur appartient au profil spécifié. La suppression est impossible.");
		}

		void DeleteProfileAction::run()
		{
			ProfileTableSync::Remove(_profile->getKey());

			// Log
			SecurityLog::addProfileAdmin(_request->getUser().get(), _profile.get(), "Suppression de " + _profile->getName());
		}
	}
}
