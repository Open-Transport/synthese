
//////////////////////////////////////////////////////////////////////////
/// MapSourceUpdateAction class implementation.
/// @file MapSourceUpdateAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "ActionException.h"
#include "MapSourceTableSync.hpp"
#include "MapSourceUpdateAction.hpp"
#include "ParametersMap.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, geography::MapSourceUpdateAction>::FACTORY_KEY("MapSourceUpdateAction");
	}

	namespace geography
	{
		const string MapSourceUpdateAction::PARAMETER_MAP_SOURCE_ID = Action_PARAMETER_PREFIX + "map_source_id";
		const string MapSourceUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const string MapSourceUpdateAction::PARAMETER_URL = Action_PARAMETER_PREFIX + "url";
		const string MapSourceUpdateAction::PARAMETER_SRID = Action_PARAMETER_PREFIX + "srid";
		const string MapSourceUpdateAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "type";



		ParametersMap MapSourceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_mapSource)
			{
				map.insert(PARAMETER_MAP_SOURCE_ID, _mapSource->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_url)
			{
				map.insert(PARAMETER_URL, *_url);
			}
			if(_coordinatesSystem)
			{
				map.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}
			if(_type)
			{
				map.insert(PARAMETER_TYPE, static_cast<int>(*_type));
			}
			return map;
		}



		void MapSourceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Map source
			RegistryKeyType mapSourceId(map.getDefault<RegistryKeyType>(PARAMETER_MAP_SOURCE_ID, 0));
			if(mapSourceId > 0)
			{
				try
				{
					_mapSource = MapSourceTableSync::GetEditable(mapSourceId, *_env);
				}
				catch(ObjectNotFoundException<MapSource>&)
				{
					throw ActionException("No such map source");
				}
			}
			else
			{
				_mapSource.reset(new MapSource);
			}

			// Name
			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			// URL
			if(map.isDefined(PARAMETER_URL))
			{
				_url = map.get<string>(PARAMETER_URL);
			}

			// SRID
			if(map.isDefined(PARAMETER_SRID))
			{
				try
				{
					_coordinatesSystem = CoordinatesSystem::GetCoordinatesSystem(map.get<int>(PARAMETER_SRID));
				}
				catch(CoordinatesSystem::CoordinatesSystemNotFoundException&)
				{
					throw ActionException("No such SRID");
				}
			}

			// Type
			if(map.isDefined(PARAMETER_TYPE))
			{
				int typeId(map.get<int>(PARAMETER_TYPE));
				if(typeId < 0 || typeId > MapSource::MAX_TYPE_INT)
				{
					throw ActionException("Bad value for type");
				}
				_type = static_cast<MapSource::Type>(typeId);
			}
		}



		void MapSourceUpdateAction::run(
			Request& request
		){
			if(_name)
			{
				_mapSource->setName(*_name);
			}
			if(_url)
			{
				_mapSource->setURL(*_url);
			}
			if(_coordinatesSystem)
			{
				_mapSource->setCoordinatesSystem(*_coordinatesSystem);
			}
			if(_type)
			{
				_mapSource->setType(*_type);
			}

			// Save
			MapSourceTableSync::Save(_mapSource.get());

			// Created id
			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_mapSource->getKey());
			}
		}



		bool MapSourceUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
			// Todo return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}
}	}
