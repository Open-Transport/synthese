
//////////////////////////////////////////////////////////////////////////
/// PhysicalStopUpdateAction class implementation.
/// @file PhysicalStopUpdateAction.cpp
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
#include "PhysicalStopUpdateAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "PhysicalStopTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::PhysicalStopUpdateAction>::FACTORY_KEY("PhysicalStopUpdateAction");
	}

	namespace pt
	{
		const string PhysicalStopUpdateAction::PARAMETER_STOP_ID = Action_PARAMETER_PREFIX + "id";
		const string PhysicalStopUpdateAction::PARAMETER_X = Action_PARAMETER_PREFIX + "x";
		const string PhysicalStopUpdateAction::PARAMETER_Y = Action_PARAMETER_PREFIX + "y";
		const string PhysicalStopUpdateAction::PARAMETER_OPERATOR_CODE = Action_PARAMETER_PREFIX + "oc";
		const string PhysicalStopUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";

		
		
		ParametersMap PhysicalStopUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_stop.get())
			{
				map.insert(PARAMETER_STOP_ID, _stop->getKey());
			}
			map.insert(PARAMETER_X, _x);
			map.insert(PARAMETER_Y, _y);
			map.insert(PARAMETER_OPERATOR_CODE, _operatorCode);
			map.insert(PARAMETER_NAME, _name);
			return map;
		}
		
		
		
		void PhysicalStopUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_stop = PhysicalStopTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_STOP_ID), *_env);
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such physical stop");
			}

			_x = map.get<double>(PARAMETER_X);
			_y = map.get<double>(PARAMETER_Y);
			_operatorCode = map.get<string>(PARAMETER_OPERATOR_CODE);
			_name = map.get<string>(PARAMETER_NAME);
		}
		
		
		
		void PhysicalStopUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			_stop->setXY(_x, _y);
			_stop->setCodeBySource(_operatorCode);
			_stop->setName(_name);

			PhysicalStopTableSync::Save(_stop.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool PhysicalStopUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
