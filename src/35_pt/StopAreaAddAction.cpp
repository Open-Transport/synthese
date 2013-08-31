
//////////////////////////////////////////////////////////////////////////
/// StopAreaAddAction class implementation.
/// @file StopAreaAddAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "StopAreaAddAction.h"

#include "ActionException.h"
#include "DataSourceLinksField.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "StopAreaTableSync.hpp"
#include "DBTransaction.hpp"
#include "CityTableSync.h"
#include "CoordinatesSystem.hpp"
#include "DataSourceTableSync.h"
#include "ImportableTableSync.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;
	using namespace geography;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopAreaAddAction>::FACTORY_KEY("StopAreaAddAction");
	}

	namespace pt
	{
		const string StopAreaAddAction::PARAMETER_CITY_NAME = Action_PARAMETER_PREFIX + "cn";
		const string StopAreaAddAction::PARAMETER_CITY_ID = Action_PARAMETER_PREFIX + "ci";
		const string StopAreaAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "sn";
		const string StopAreaAddAction::PARAMETER_CREATE_CITY_IF_NECESSARY = Action_PARAMETER_PREFIX + "cc";
		const string StopAreaAddAction::PARAMETER_DATA_SOURCE_LINKS = Action_PARAMETER_PREFIX + "sl";



		ParametersMap StopAreaAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_city.get())
			{
				map.insert(PARAMETER_CITY_ID, _city->getKey());
			}
			else
			{
				map.insert(PARAMETER_CITY_NAME, _cityName);
			}
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_CREATE_CITY_IF_NECESSARY, _createCityIfNecessary);
			map.insert(
				PARAMETER_DATA_SOURCE_LINKS,
				DataSourceLinks::Serialize(_dataSourceLinks)
			);
			return map;
		}



		void StopAreaAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getDefault<RegistryKeyType>(PARAMETER_CITY_ID, 0))
			{
				_city = CityTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CITY_ID), *_env);
			}
			else
			{
				_cityName = map.get<string>(PARAMETER_CITY_NAME);
				CityTableSync::SearchResult cities(CityTableSync::Search(*_env, _cityName));
				if(!cities.empty())
				{
					_city = cities.front();
				}
				_createCityIfNecessary = map.getDefault<bool>(PARAMETER_CREATE_CITY_IF_NECESSARY, false);
				if(!_city.get() && !_createCityIfNecessary)
				{
					throw ActionException("City not found");
				}
			}

			_name = map.get<string>(PARAMETER_NAME);

			_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(
				map.getDefault<string>(PARAMETER_DATA_SOURCE_LINKS),
				*_env
			);
		}



		void StopAreaAddAction::run(
			Request& request
		){

			DBTransaction transaction;

			StopArea stopArea;
			stopArea.setAllowedConnection(true);

			if(!_city.get())
			{
				_city.reset(new City);
				_city->set<Name>(_cityName);
				CityTableSync::Save(_city.get(), transaction);
			}
			stopArea.setCity(_city.get());
			stopArea.setDefaultTransferDelay(minutes(2));
			stopArea.setName(_name);
			stopArea.setDataSourceLinksWithoutRegistration(_dataSourceLinks);
			StopAreaTableSync::Save(&stopArea, transaction);

			transaction.run();

//			::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(stopArea.getKey());
		}



		bool StopAreaAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
