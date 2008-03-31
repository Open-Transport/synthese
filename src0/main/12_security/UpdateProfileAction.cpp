
/** UpdateProfileAction class implementation.
	@file UpdateProfileAction.cpp

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

#include "UpdateProfileAction.h"

#include "12_security/SecurityModule.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/SecurityLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

#include "13_dblog/DBLogModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace dblog;

	namespace util
	{
		template<> const string FactorableTemplate<server::Action,security::UpdateProfileAction>::FACTORY_KEY("upa");
	}

	namespace security
	{
		const string UpdateProfileAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";


		ParametersMap UpdateProfileAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			return map;
		}

		void UpdateProfileAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Profile
			try
			{
				_profile = ProfileTableSync::GetUpdateable(_request->getObjectId());
			}
			catch (Profile::ObjectNotFoundException& e)
			{
				throw ActionException(e.getMessage());
			}

			// Name
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			vector<shared_ptr<Profile> > existingProfiles = ProfileTableSync::Search(_name, string(), 0,1);
			if (!existingProfiles.empty())
				throw ActionException("Le nom choisi est déjà pris par un autre profil. Veuillez entrer un autre nom.");
		}

		void UpdateProfileAction::run()
		{
			// Old value
			stringstream log;
			DBLogModule::appendToLogIfChange(log, "Nom", _profile->getName(), _name);

			// Action
			_profile->setName(_name);
			ProfileTableSync::save(_profile.get());

			// Log
			SecurityLog::addProfileAdmin(_request->getUser().get(), _profile.get(), log.str());
		}
	}
}
