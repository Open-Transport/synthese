
//////////////////////////////////////////////////////////////////////////
/// PlaceAliasUpdateAction class implementation.
/// @file PlaceAliasUpdateAction.cpp
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

#include "PlaceAliasUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "CityTableSync.h"
#include "PlaceAliasTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::PlaceAliasUpdateAction>::FACTORY_KEY("PlaceAliasUpdateAction");
	}

	namespace pt
	{
		const string PlaceAliasUpdateAction::PARAMETER_ALIAS_ID = Action_PARAMETER_PREFIX + "ai";
		const string PlaceAliasUpdateAction::PARAMETER_STOP_AREA_ID = Action_PARAMETER_PREFIX + "sa";
		const string PlaceAliasUpdateAction::PARAMETER_CITY_ID = Action_PARAMETER_PREFIX + "ci";
		const string PlaceAliasUpdateAction::PARAMETER_IS_CITY_MAIN_PLACE = Action_PARAMETER_PREFIX + "im";
		const string PlaceAliasUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";



		ParametersMap PlaceAliasUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_alias.get())
			{
				map.insert(PARAMETER_ALIAS_ID, _alias->getKey());
			}
			if(_stopArea)
			{
				map.insert(PARAMETER_STOP_AREA_ID, _stopArea->get() ? (*_stopArea)->getKey() : RegistryKeyType(0));
			}
			if(_city)
			{
				map.insert(PARAMETER_CITY_ID, _city->get() ? (*_city)->getKey() : RegistryKeyType(0));
			}
			if(_isCityMainPlace)
			{
				map.insert(PARAMETER_IS_CITY_MAIN_PLACE, *_isCityMainPlace);
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			return map;
		}



		void PlaceAliasUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(PARAMETER_ALIAS_ID))
			{
				try
				{
					_alias = PlaceAliasTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ALIAS_ID), *_env);
				}
				catch (ObjectNotFoundException<PlaceAlias>&)
				{
					throw ActionException("No such alias");
				}
			}
			else
			{
				_alias = boost::shared_ptr<PlaceAlias>(new PlaceAlias);
			}

			if(map.isDefined(PARAMETER_STOP_AREA_ID))
			{
				try
				{
					_stopArea = StopAreaTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_ID), *_env);
				}
				catch (ObjectNotFoundException<StopArea>&)
				{
					throw ActionException("No such stop area");
				}
			}

			if(map.isDefined(PARAMETER_CITY_ID))
			{
				try
				{
					_city = CityTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CITY_ID), *_env);
				}
				catch (ObjectNotFoundException<City>&)
				{
					throw ActionException("No such city");
				}
			}

			if(map.isDefined(PARAMETER_IS_CITY_MAIN_PLACE))
			{
				_isCityMainPlace = map.getDefault<bool>(PARAMETER_IS_CITY_MAIN_PLACE, false);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}
		}



		void PlaceAliasUpdateAction::run(
			Request& request
		){
			if(_name)
			{
				_alias->setName(*_name);
				_alias->set<Name>(*_name);
			}

			if(_city)
			{
				_alias->setCity(_city->get());
				_alias->set<ParentCity>(*_city->get());
			}

			if(_stopArea)
			{
				_alias->setAliasedPlace(_stopArea->get());
				_alias->set<AliasedPlaceId>((*_stopArea)->getKey());
			}

			if(_isCityMainPlace && _alias->getCity())
			{
				if(*_isCityMainPlace && !_alias->getCity()->includes(*_alias))
				{
					const_cast<City*>(_alias->getCity())->addIncludedPlace(*_alias);
				}
				if(!*_isCityMainPlace && _alias->getCity()->includes(*_alias))
				{
					const_cast<City*>(_alias->getCity())->removeIncludedPlace(*_alias);
				}
				_alias->set<geography::IsCityMainConnection>(*_isCityMainPlace);
			}

			PlaceAliasTableSync::Save(_alias.get());
		}



		bool PlaceAliasUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
