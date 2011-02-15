
//////////////////////////////////////////////////////////////////////////
/// StopPointAddAction class implementation.
/// @file StopPointAddAction.cpp
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
#include "StopPointAddAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "DBModule.h"
#include "ImportableTableSync.hpp"
#include "CityTableSync.h"
#include "GeographyModule.h"

#include <geos/geom/Point.h>

using namespace std;
using namespace geos::geom;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace geography;
	using namespace db;
	using namespace impex;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::StopPointAddAction>::FACTORY_KEY("StopPointAddAction");
	}

	namespace pt
	{
		const string StopPointAddAction::PARAMETER_PLACE_ID = Action_PARAMETER_PREFIX + "pl";
		const string StopPointAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string StopPointAddAction::PARAMETER_OPERATOR_CODE = Action_PARAMETER_PREFIX + "oc";
		const string StopPointAddAction::PARAMETER_X = Action_PARAMETER_PREFIX + "x";
		const string StopPointAddAction::PARAMETER_Y = Action_PARAMETER_PREFIX + "y";
		const string StopPointAddAction::PARAMETER_LONGITUDE = Action_PARAMETER_PREFIX + "lon";
		const string StopPointAddAction::PARAMETER_LATITUDE = Action_PARAMETER_PREFIX + "lat";
		const string StopPointAddAction::PARAMETER_CITY_NAME = Action_PARAMETER_PREFIX + "cn";
		const string StopPointAddAction::PARAMETER_CITY_ID = Action_PARAMETER_PREFIX + "ci";
		const string StopPointAddAction::PARAMETER_CREATE_CITY_IF_NECESSARY = Action_PARAMETER_PREFIX + "cc";

		
		
		ParametersMap StopPointAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_place.get())
			{
				map.insert(PARAMETER_PLACE_ID, _place->getKey());
			}
			map.insert(
				PARAMETER_OPERATOR_CODE,
				ImportableTableSync::SerializeDataSourceLinks(_operatorCode)
			);
			if(_point.get() && !_point->isEmpty())
			{
				map.insert(PARAMETER_X, _point->getX());
				map.insert(PARAMETER_Y, _point->getY());
			}
			map.insert(PARAMETER_NAME, _name);
			if(_city.get())
			{
				map.insert(PARAMETER_CITY_ID, _city->getKey());
			}
			else if(_cityName)
			{
				map.insert(PARAMETER_CITY_NAME, *_cityName);
			}
			map.insert(PARAMETER_CREATE_CITY_IF_NECESSARY, _createCityIfNecessary);

			return map;
		}
		
		
		
		void StopPointAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.getDefault<string>(PARAMETER_NAME);

			if(map.getDefault<RegistryKeyType>(PARAMETER_PLACE_ID, 0)) try
			{
				_place = StopAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PLACE_ID), *_env);
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("No such connection place");
			}
			else
			{
				if(map.isDefined(PARAMETER_CITY_ID)) try
				{
					_city = CityTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CITY_ID), *_env);
				}
				catch(ObjectNotFoundException<City>&)
				{
					throw ActionException("No such city");
				}
				else
				{
					_createCityIfNecessary = map.getDefault<bool>(PARAMETER_CREATE_CITY_IF_NECESSARY, false);
					_cityName = map.get<string>(PARAMETER_CITY_NAME);
					if(_cityName->empty())
					{
						throw ActionException("Empty city name");
					}
					

					GeographyModule::CitiesMatcher::MatchResult cities(
						GeographyModule::GetCitiesMatcher().bestMatches(*_cityName,1)
					);
					if(cities.empty())
					{
						throw ActionException("City not found");
					}
					if(	!cities.empty() &&
						(	to_lower_copy(*_cityName) == to_lower_copy(cities.front().value->getName()) ||
							_createCityIfNecessary
					)	){
						_city = cities.front().value;
					}
				}
				if(_city.get())
				{
					vector<shared_ptr<StopArea> > places(_city->search<StopArea>(_name, 1));
					if(!places.empty() && (*places.begin())->getName() == _name)
					{
						_place = *places.begin();
					}
				}
			}

			_operatorCode = ImportableTableSync::GetDataSourceLinksFromSerializedString(
				map.getDefault<string>(PARAMETER_OPERATOR_CODE),
				*_env
			);
			if(map.getDefault<double>(PARAMETER_X, 0) && map.getDefault<double>(PARAMETER_Y, 0))
			{
				_point = CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
					map.get<double>(PARAMETER_X),
					map.get<double>(PARAMETER_Y)
				);
			}
			else if(map.getDefault<double>(PARAMETER_LONGITUDE, 0) && map.getDefault<double>(PARAMETER_LATITUDE, 0))
			{
				_point = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
					*DBModule::GetStorageCoordinatesSystem().createPoint(
						map.get<double>(PARAMETER_LONGITUDE),
						map.get<double>(PARAMETER_LATITUDE)
				)	);
			}
		}
		
		
		
		void StopPointAddAction::run(
			Request& request
		){
			SQLiteTransaction transaction;

			// Place creation
			if(!_place.get())
			{
				// City creation
				if(!_city.get())
				{
					_city.reset(new City);
					_city->setName(*_cityName);
					CityTableSync::Save(_city.get(), transaction);
				}

				_place.reset(new StopArea);
				_place->setAllowedConnection(true);
				_place->setCity(_city.get());
				_place->setDefaultTransferDelay(minutes(2));
				Importable::DataSourceLinks links;
				BOOST_FOREACH(impex::Importable::DataSourceLinks::value_type& link, _operatorCode)
				{
					if(link.first)
					{
						links.insert(make_pair(link.first, string()));
					}
				}
				_place->setDataSourceLinks(links);
				_place->setName(_name);
				StopAreaTableSync::Save(_place.get(), transaction);
			}

			StopPoint object;
			object.setHub(_place.get());
			object.setName(_name);
			object.setDataSourceLinks(_operatorCode);
			object.setGeometry(_point);

			StopPointTableSync::Save(&object, transaction);

			transaction.run();

//			::AddCreationEntry(object, request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(object.getKey());
			}
		}
		
		
		
		bool StopPointAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}



		StopPointAddAction::StopPointAddAction():
			_createCityIfNecessary(false)
		{}
}	}
