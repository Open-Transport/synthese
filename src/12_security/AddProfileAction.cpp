
/** AddProfileAction class implementation.
	@file AddProfileAction.cpp

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

#include "AddProfileAction.h"

#include "12_security/Profile.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/GlobalRight.h"
#include "12_security/SecurityLog.h"

#include "30_server/ActionException.h"
#include "30_server/QueryString.h"
#include "30_server/ParametersMap.h"
#include "30_server/Request.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action, security::AddProfileAction>::FACTORY_KEY("apa");
	
	namespace security
	{
		const string AddProfileAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "apan";
		const string AddProfileAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "apat";


		ParametersMap AddProfileAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			if (_templateProfile.get())
				map.insert(PARAMETER_TEMPLATE_ID, _templateProfile->getKey());
			return map;
		}

		void AddProfileAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Name
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);

			// Template
			uid id = map.getUid(PARAMETER_TEMPLATE_ID, false, FACTORY_KEY);
			if (id != UNKNOWN_VALUE)
			{
				if (!Env::GetOfficialEnv().getRegistry<Profile>().contains(id))
					throw ActionException("Specified root profile not found.");
				_templateProfile = ProfileTableSync::Get(id, _env);
			}

			// Name unicity
			Env env;
			ProfileTableSync::Search(env, _name, string(),0,1);
			if (!env.getRegistry<Profile>().empty())
				throw ActionException("Le nom choisi est déjà pris par un autre profil. Veuillez entrer un autre nom.");

			_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void AddProfileAction::run()
		{
			shared_ptr<Profile> profile(new Profile);
			profile->setName(_name);
			if (_templateProfile.get())
			{
				profile->setParent(_templateProfile.get());
				const RightsVector& rights(_templateProfile->getRights());
				for (RightsVector::const_iterator it(rights.begin()); it != rights.end(); ++it)
					profile->addRight(it->second);
			}
			else
			{
				shared_ptr<Right> r(new GlobalRight);
				r->setPrivateLevel(FORBIDDEN);
				r->setPublicLevel(FORBIDDEN);
				profile->addRight(r);
			}
			ProfileTableSync::Save(profile.get());
			_request->setObjectId(profile->getKey());

			// DBLog
			SecurityLog::addProfileAdmin(_request->getUser().get(), profile.get(), "Création du profil" + (_templateProfile.get() ? " à partir de " + _templateProfile->getName() : string()));
		}
	}
}
