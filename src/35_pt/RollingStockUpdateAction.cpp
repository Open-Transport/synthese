
//////////////////////////////////////////////////////////////////////////
/// RollingStockUpdateAction class implementation.
/// @file RollingStockUpdateAction.cpp
/// @author RCSobility
/// @date 2011
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
#include "RollingStockUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "RollingStockTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::RollingStockUpdateAction>::FACTORY_KEY("RollingStockUpdateAction");
	}

	namespace pt
	{
		const string RollingStockUpdateAction::PARAMETER_ROLLING_STOCK_ID = Action_PARAMETER_PREFIX + "id";
		const string RollingStockUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";

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
		}



		void RollingStockUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			if(_name)
			{
				_rollingStock->setName(*_name);
			}

			RollingStockTableSync::Save(_rollingStock.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_rollingStock->getKey());
			}
		}



		bool RollingStockUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
