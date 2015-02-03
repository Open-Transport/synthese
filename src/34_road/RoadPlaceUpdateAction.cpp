
//////////////////////////////////////////////////////////////////////////
/// RoadPlaceUpdateAction class implementation.
/// @file RoadPlaceUpdateAction.cpp
/// @author Thomas Puigt
/// @date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "RoadPlaceUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"
#include "City.h"
#include "CityTableSync.h"
#include "PathGroup.h"
#include "JourneyPatternTableSync.hpp"

using namespace std;
using namespace boost::posix_time;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
//	using namespace impex;
	using namespace geography;
//	using namespace graph;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, road::RoadPlaceUpdateAction>::FACTORY_KEY("RoadPlaceUpdateAction");
	}

	namespace road
	{
		const string RoadPlaceUpdateAction::PARAMETER_ROAD_PLACE_ID = Action_PARAMETER_PREFIX + "id";
		const string RoadPlaceUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string RoadPlaceUpdateAction::PARAMETER_SHORT_NAME = Action_PARAMETER_PREFIX + "sn";
		const string RoadPlaceUpdateAction::PARAMETER_LONG_NAME = Action_PARAMETER_PREFIX + "ln";
		const string RoadPlaceUpdateAction::PARAMETER_CITY_ID = Action_PARAMETER_PREFIX + "ci";
		const string RoadPlaceUpdateAction::PARAMETER_IS_MAIN = Action_PARAMETER_PREFIX + "ma";
		const string RoadPlaceUpdateAction::PARAMETER_SRID = Action_PARAMETER_PREFIX + "srid";



		ParametersMap RoadPlaceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_place.get())
			{
				map.insert(PARAMETER_ROAD_PLACE_ID, _place->getKey());
			}
			if(_city.get())
			{
				map.insert(PARAMETER_CITY_ID, _city->getKey());
			}
			if(_longName)
			{
				map.insert(PARAMETER_LONG_NAME, *_longName);
			}
			if(_shortName)
			{
				map.insert(PARAMETER_SHORT_NAME, *_shortName);
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_isMain)
			{
				map.insert(PARAMETER_IS_MAIN, *_isMain);
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void RoadPlaceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_place = RoadPlaceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROAD_PLACE_ID), *_env);
			}
			catch(ObjectNotFoundException<RoadPlace>&)
			{
				throw ActionException("No such place");
			}

			if(map.getOptional<RegistryKeyType>(PARAMETER_CITY_ID)) try
			{
				_city = CityTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CITY_ID), *_env);
			}
			catch(ObjectNotFoundException<City>&)
			{
				throw ActionException("No such city");
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			if(map.isDefined(PARAMETER_SHORT_NAME))
			{
				_shortName = map.get<string>(PARAMETER_SHORT_NAME);
			}

			if(map.isDefined(PARAMETER_LONG_NAME))
			{
				_longName = map.get<string>(PARAMETER_LONG_NAME);
			}

			if(map.isDefined(PARAMETER_IS_MAIN))
			{
				_isMain = map.get<bool>(PARAMETER_IS_MAIN);
			}

			// Importable
			_setImportableUpdateFromParametersMap(*_env, map);
		}



		void RoadPlaceUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_city.get())
			{
				_place->setCity(_city.get());
				if(_isMain)
				{
					if(*_isMain && !_city->includes(*_place))
					{
						_city->addIncludedPlace(*_place);
					}
					if(!*_isMain && _city->includes(*_place))
					{
						_city->removeIncludedPlace(*_place);
					}
				}
			}

			if(_name)
			{
				_place->setName(*_name);
			}

			if(_shortName)
			{
				_place->setName13(*_shortName);
			}

			if(_longName)
			{
				_place->setName26(*_longName);
			}


			// Importable
			_doImportableUpdate(*_place, request);

			RoadPlaceTableSync::Save(_place.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool RoadPlaceUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
