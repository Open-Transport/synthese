
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

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
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
		
		
		
		ParametersMap StopPointAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_place.get())
			{
				map.insert(PARAMETER_PLACE_ID, _place->getKey());
			}
			map.insert(PARAMETER_OPERATOR_CODE, _operatorCode);
			map.insert(PARAMETER_X, _x);
			map.insert(PARAMETER_Y, _y);
			map.insert(PARAMETER_NAME, _name);
			return map;
		}
		
		
		
		void StopPointAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_place = StopAreaTableSync::Get(map.get<RegistryKeyType>(PARAMETER_PLACE_ID), *_env);
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw ActionException("No such connection place");
			}

			_name = map.getDefault<string>(PARAMETER_NAME);
			_operatorCode = map.getDefault<string>(PARAMETER_OPERATOR_CODE);
			_x = map.getDefault<double>(PARAMETER_X, 0);
			_y = map.getDefault<double>(PARAMETER_Y, 0);
		}
		
		
		
		void StopPointAddAction::run(
			Request& request
		){
			StopPoint object;
			object.setHub(_place.get());
			object.setName(_name);
			object.setCodeBySource(_operatorCode);
			object.setXY(_x, _y);

			StopPointTableSync::Save(&object);

//			::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(object.getKey());
		}
		
		
		
		bool StopPointAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
