
//////////////////////////////////////////////////////////////////////////
/// StopAreaAddAction class implementation.
/// @file StopAreaAddAction.cpp
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
#include "StopAreaAddAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "SQLiteTransaction.h"
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
		const string StopAreaAddAction::PARAMETER_PHYSICAL_STOP_OPERATOR_CODE = Action_PARAMETER_PREFIX + "sc";
		const string StopAreaAddAction::PARAMETER_PHYSICAL_STOP_DATA_SOURCE_ID = Action_PARAMETER_PREFIX + "ds";
		const string StopAreaAddAction::PARAMETER_CREATE_PHYSICAL_STOP = Action_PARAMETER_PREFIX + "cp";
		const string StopAreaAddAction::PARAMETER_PHYSICAL_STOP_X = Action_PARAMETER_PREFIX + "xx";
		const string StopAreaAddAction::PARAMETER_PHYSICAL_STOP_Y = Action_PARAMETER_PREFIX + "yy";
		
		
		
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
			map.insert(PARAMETER_CREATE_PHYSICAL_STOP, _createPhysicalStop);
			map.insert(PARAMETER_CREATE_CITY_IF_NECESSARY, _createCityIfNecessary);
			if(_point.get() && !_point->isEmpty())
			{
				map.insert(PARAMETER_PHYSICAL_STOP_X, _point->getX());
				map.insert(PARAMETER_PHYSICAL_STOP_Y, _point->getY());
			}
			if(_dataSourceLinks.size() == 1)
			{
				if(_dataSourceLinks.begin()->first)
				{
					map.insert(PARAMETER_PHYSICAL_STOP_DATA_SOURCE_ID, _dataSourceLinks.begin()->first->getKey());
				}
				if(!_dataSourceLinks.begin()->second.empty())
				{
					map.insert(PARAMETER_PHYSICAL_STOP_OPERATOR_CODE, _dataSourceLinks.begin()->second);
			}	}
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
			_createPhysicalStop = map.getDefault<bool>(PARAMETER_CREATE_PHYSICAL_STOP, false);
			string operatorCode(map.getDefault<string>(PARAMETER_PHYSICAL_STOP_OPERATOR_CODE));
			DataSource* source(NULL);
			try
			{
				RegistryKeyType sourceId(map.getDefault<RegistryKeyType>(PARAMETER_PHYSICAL_STOP_DATA_SOURCE_ID));
				if(sourceId)
				{
					source = DataSourceTableSync::GetEditable(sourceId, *_env).get();
				}
			}
			catch(ObjectNotFoundException<DataSource>)
			{
				throw ActionException("Data source not found");
			}
			if(source || !operatorCode.empty())
			{
				_dataSourceLinks.insert(make_pair(source, operatorCode));
			}

			optional<double> x(map.getOptional<double>(PARAMETER_PHYSICAL_STOP_X));
			optional<double> y(map.getOptional<double>(PARAMETER_PHYSICAL_STOP_Y));
			if(x && y)
			{
				_point = 
					CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
						*DBModule::GetStorageCoordinatesSystem().createPoint(*x, *y)
					);
			}
		}
		
		
		
		void StopAreaAddAction::run(
			Request& request
		){

			SQLiteTransaction transaction;

			StopArea stopArea;
			stopArea.setAllowedConnection(true);
			
			if(!_city.get())
			{
				_city.reset(new City);
				_city->setName(_cityName);
				_city->setKey(CityTableSync::getId());
				CityTableSync::Save(_city.get(), transaction);
			}
			stopArea.setCity(_city.get());
			stopArea.setKey(StopAreaTableSync::getId());
			stopArea.setDefaultTransferDelay(minutes(2));
			stopArea.setName(_name);
			StopAreaTableSync::Save(&stopArea, transaction);

			if(_createPhysicalStop || _point.get() && !_point->isEmpty() && _point->getX() && _point->getY())
			{
				StopPoint stop;
				stop.setName(_name);
				stop.setHub(&stopArea);
				stop.setDataSourceLinks(_dataSourceLinks);
				stop.setGeometry(_point);
				StopPointTableSync::Save(&stop, transaction);
			}
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
