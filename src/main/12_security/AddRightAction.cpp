
/** AddRightAction class implementation.
	@file AddRightAction.cpp

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

#include "01_util/Conversion.h"

#include "12_security/AddRightAction.h"
#include "12_security/SecurityModule.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/Right.h"
#include "12_security/SecurityLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, security::AddRightAction>::FACTORY_KEY("ara");
	}
	
	namespace security
	{
		const string AddRightAction::PARAMETER_RIGHT = Action_PARAMETER_PREFIX + "right";
		const string AddRightAction::PARAMETER_PUBLIC_LEVEL = Action_PARAMETER_PREFIX + "pulev";
		const string AddRightAction::PARAMETER_PRIVATE_LEVEL = Action_PARAMETER_PREFIX + "prlev";
		const string AddRightAction::PARAMETER_PARAMETER = Action_PARAMETER_PREFIX + "param";


		ParametersMap AddRightAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void AddRightAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_profile = ProfileTableSync::GetUpdateable(_request->getObjectId());

				_rightName = map.getString(PARAMETER_RIGHT, true, FACTORY_KEY);
				if (!Factory<Right>::contains(_rightName))
					throw ActionException("Specified right class not found");
				
				_parameter = map.getString(PARAMETER_PARAMETER, true, FACTORY_KEY);
				_publicLevel = static_cast<RightLevel>(map.getInt(PARAMETER_PUBLIC_LEVEL, true, FACTORY_KEY));
				_privateLevel = static_cast<RightLevel>(map.getInt(PARAMETER_PRIVATE_LEVEL, false, FACTORY_KEY));
			}
			catch(DBEmptyResultException<Profile>)
			{
				throw ActionException("Profil introuvable");
			}
		}

		void AddRightAction::run()
		{
			shared_ptr<Right> right = Factory<Right>::createSharedPtr(_rightName);
			right->setParameter(_parameter);
			right->setPrivateLevel(_privateLevel);
			right->setPublicLevel(_publicLevel);
			_profile->addRight(right);

			ProfileTableSync::save(_profile.get());

			SecurityLog::addProfileAdmin(_request->getUser().get(), _profile.get(), "Ajout habilitation " + _rightName + "/" + _parameter);
		}
	}
}
