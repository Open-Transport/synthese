
//////////////////////////////////////////////////////////////////////////
/// RollingStockUpdateAction class implementation.
/// @file RollingStockUpdateAction.cpp
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

#include "RollingStockUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "Request.h"
#include "RollingStockTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<Action, vehicle::RollingStockUpdateAction>::FACTORY_KEY("RollingStockUpdateAction");
	}

	namespace vehicle
	{
		const string RollingStockUpdateAction::PARAMETER_ROLLING_STOCK_ID = Action_PARAMETER_PREFIX + "id";
		const string RollingStockUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string RollingStockUpdateAction::PARAMETER_ARTICLE = Action_PARAMETER_PREFIX + "ar";
		const string RollingStockUpdateAction::PARAMETER_CO2_EMISSIONS = Action_PARAMETER_PREFIX + "co2";
		const string RollingStockUpdateAction::PARAMETER_ENERGY_CONSUMPTION = Action_PARAMETER_PREFIX + "ec";



		ParametersMap RollingStockUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_rollingStock.get())
			{
				map.insert(PARAMETER_ROLLING_STOCK_ID, _rollingStock->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_article)
			{
				map.insert(PARAMETER_ARTICLE, *_article);
			}
			if(_CO2Emissions)
			{
				map.insert(PARAMETER_CO2_EMISSIONS, *_CO2Emissions);
			}
			if(_energyConsumption)
			{
				map.insert(PARAMETER_ENERGY_CONSUMPTION, *_energyConsumption);
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void RollingStockUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getOptional<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID)) try
			{
				_rollingStock = RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID), *_env);
			}
			catch (ObjectNotFoundException<RollingStock>&)
			{
				throw ActionException("No such rolling stock");
			}
			else
			{
				_rollingStock.reset(new RollingStock);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}
			if(map.isDefined(PARAMETER_ARTICLE))
			{
				_article = map.get<string>(PARAMETER_ARTICLE);
			}
			if(map.isDefined(PARAMETER_CO2_EMISSIONS))
			{
				_CO2Emissions = map.get<double>(PARAMETER_CO2_EMISSIONS);
			}
			if(map.isDefined(PARAMETER_ENERGY_CONSUMPTION))
			{
				_energyConsumption = map.get<double>(PARAMETER_ENERGY_CONSUMPTION);
			}

			// Importable
			_setImportableUpdateFromParametersMap(*_env, map);
		}



		void RollingStockUpdateAction::run(
			Request& request
		){
			if(_name)
			{
				_rollingStock->setName(*_name);
			}
			if(_article)
			{
				_rollingStock->setArticle(*_article);
			}
			if(_CO2Emissions)
			{
				_rollingStock->setCO2Emissions(*_CO2Emissions);
			}
			if(_energyConsumption)
			{
				_rollingStock->setEnergyConsumption(*_energyConsumption);
			}

			// Importable
			_doImportableUpdate(*_rollingStock, request);

			RollingStockTableSync::Save(_rollingStock.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_rollingStock->getKey());
			}
		}



		bool RollingStockUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true; // TODO create vehicle right
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
