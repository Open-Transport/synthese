
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

#include "30_server/ActionException.h"
#include "30_server/Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace db;
	
	namespace security
	{
		const string AddRightAction::PARAMETER_RIGHT = Action_PARAMETER_PREFIX + "right";
		const string AddRightAction::PARAMETER_PUBLIC_LEVEL = Action_PARAMETER_PREFIX + "pulev";
		const string AddRightAction::PARAMETER_PRIVATE_LEVEL = Action_PARAMETER_PREFIX + "prlev";
		const string AddRightAction::PARAMETER_PARAMETER = Action_PARAMETER_PREFIX + "param";


		ParametersMap AddRightAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void AddRightAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_profile = ProfileTableSync::get(_request->getObjectId());

				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_RIGHT);
				if (it == map.end())
					throw ActionException("Right class not specified");
				_rightName = it->second;
				if (!Factory<Right>::contains(_rightName))
					throw ActionException("Specified right class not found");
				
				it = map.find(PARAMETER_PARAMETER);
				if (it == map.end())
					throw ActionException("Parameter not specified");
				_parameter = it->second;

				it = map.find(PARAMETER_PUBLIC_LEVEL);
				if (it == map.end())
					throw ActionException("Public level not specified");
				_publicLevel = (Right::Level) Conversion::ToInt(it->second);

				it = map.find(PARAMETER_PRIVATE_LEVEL);
				if (it == map.end())
					throw ActionException("Private level not specified");
				_privateLevel = (Right::Level) Conversion::ToInt(it->second);
			}
			catch(DBEmptyResultException<Profile>)
			{
				throw ActionException("Profil introuvable");
			}
		}

		void AddRightAction::run()
		{
			shared_ptr<Right> right = Factory<Right>::create(_rightName);
			right->setParameter(_parameter);
			right->setPrivateLevel(_privateLevel);
			right->setPublicLevel(_publicLevel);
			_profile->addRight(right);
			ProfileTableSync::save(_profile.get());
		}
	}
}
