
/** AddProfileAction class implementation.
	@file AddProfileAction.cpp

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

#include "AddProfileAction.h"

#include "Profile.h"
#include "ProfileTableSync.h"
#include "Session.h"
#include "User.h"
#include "GlobalRight.h"
#include "SecurityLog.h"
#include "SecurityRight.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"

using namespace std;
using namespace boost;

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
			try
			{
				// Name
				_name = map.get<string>(PARAMETER_NAME);

				// Template
				optional<RegistryKeyType> id = map.getOptional<RegistryKeyType>(PARAMETER_TEMPLATE_ID);
				if (id)
				{
					_templateProfile = ProfileTableSync::Get(*id, *_env);
				}

				// Name uniqueness
				Env env;
				ProfileTableSync::Search(env, _name, string(),0,1);
				if (!env.getRegistry<Profile>().empty())
					throw ActionException("Le nom choisi est déjà pris par un autre profil. Veuillez entrer un autre nom.");
			}
			catch(ObjectNotFoundException<Profile> e)
			{
				throw ActionException("root profile", e, *this);
			}
			catch(ParametersMap::MissingParameterException e)
			{
				throw ActionException(e, *this);
			}
		}

		void AddProfileAction::run(Request& request)
		{
			boost::shared_ptr<Profile> profile(new Profile);
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
				boost::shared_ptr<Right> r(new GlobalRight);
				r->setPrivateLevel(FORBIDDEN);
				r->setPublicLevel(FORBIDDEN);
				profile->addRight(r);
			}
			ProfileTableSync::Save(profile.get());

			request.setActionCreatedId(profile->getKey());

			// DBLog
			SecurityLog::addProfileAdmin(request.getUser().get(), profile.get(), "Création du profil" + (_templateProfile.get() ? " à partir de " + _templateProfile->getName() : string()));
		}



		bool AddProfileAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE);
			/// @todo Add a check on the profile on the user who creates the new profile
		}
	}
}
