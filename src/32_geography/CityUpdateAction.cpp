
//////////////////////////////////////////////////////////////////////////
/// CityUpdateAction class implementation.
/// @file CityUpdateAction.cpp
/// @author Hugues Romain
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ActionException.h"
#include "ParametersMap.h"
#include "CityUpdateAction.h"
#include "Request.h"
#include "CityTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, geography::CityUpdateAction>::FACTORY_KEY("CityUpdateAction");
	}

	namespace geography
	{
		const string CityUpdateAction::PARAMETER_CITY_ID = Action_PARAMETER_PREFIX + "id";
		const string CityUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string CityUpdateAction::PARAMETER_CODE = Action_PARAMETER_PREFIX + "co";



		ParametersMap CityUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_city.get())
			{
				map.insert(PARAMETER_CITY_ID, _city->getKey());
				map.insert(PARAMETER_NAME, _name);
				map.insert(PARAMETER_CODE, _code);
			}
			return map;
		}



		void CityUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_city = CityTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CITY_ID), *_env);
			}
			catch(ObjectNotFoundException<City>&)
			{
				throw ActionException("No such city");
			}

			_name = map.get<string>(PARAMETER_NAME);
			_code = map.getDefault<string>(PARAMETER_CODE);
		}



		void CityUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			_city->setName(_name);
			_city->setCode(_code);

			CityTableSync::Save(_city.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool CityUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}
	}
}
