
//////////////////////////////////////////////////////////////////////////
/// StopAreaNameUpdateAction class implementation.
/// @file StopAreaNameUpdateAction.cpp
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
#include "StopAreaNameUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "ConnectionPlaceTableSync.h"
#include "CityTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace geography;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopAreaNameUpdateAction>::FACTORY_KEY("StopAreaNameUpdateAction");
	}

	namespace pt
	{
		const string StopAreaNameUpdateAction::PARAMETER_PLACE_ID = Action_PARAMETER_PREFIX + "id";
		const string StopAreaNameUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string StopAreaNameUpdateAction::PARAMETER_SHORT_NAME = Action_PARAMETER_PREFIX + "sn";
		const string StopAreaNameUpdateAction::PARAMETER_LONG_NAME = Action_PARAMETER_PREFIX + "ln";
		const string StopAreaNameUpdateAction::PARAMETER_CITY_ID = Action_PARAMETER_PREFIX + "ci";
		const string StopAreaNameUpdateAction::PARAMETER_CODE = Action_PARAMETER_PREFIX + "co";
		const string StopAreaNameUpdateAction::PARAMETER_IS_MAIN = Action_PARAMETER_PREFIX + "ma";

		
		
		ParametersMap StopAreaNameUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_place.get())
			{
				map.insert(PARAMETER_PLACE_ID, _place->getKey());
			}
			if(_city.get())
			{
				map.insert(PARAMETER_CITY_ID, _city->getKey());
			}
			map.insert(PARAMETER_LONG_NAME, _longName);
			map.insert(PARAMETER_SHORT_NAME, _shortName);
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_CODE, _code);
			map.insert(PARAMETER_IS_MAIN, _isMain);
			return map;
		}
		
		
		
		void StopAreaNameUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_place = ConnectionPlaceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PLACE_ID), *_env);
				_city = CityTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CITY_ID), *_env);
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("No such place");
			}
			catch(ObjectNotFoundException<City>&)
			{
				throw ActionException("No such city");
			}

			_name = map.get<string>(PARAMETER_NAME);
			_shortName = map.get<string>(PARAMETER_SHORT_NAME);
			_longName = map.get<string>(PARAMETER_LONG_NAME);
			_code = map.get<string>(PARAMETER_CODE);
			_isMain = map.get<bool>(PARAMETER_IS_MAIN);
		}
		
		
		
		void StopAreaNameUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			_place->setCity(_city.get());
			_place->setName(_name);
			_place->setName13(_shortName);
			_place->setName26(_longName);
			_place->setCodeBySource(_code);
			if(_isMain && !_city->includes(_place.get()))
			{
				_city->addIncludedPlace(_place.get());
			}
			if(!_isMain && _city->includes(_place.get()))
			{
				_city->removeIncludedPlace(_place.get());
			}

			ConnectionPlaceTableSync::Save(_place.get());
			
			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool StopAreaNameUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
