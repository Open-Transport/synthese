
//////////////////////////////////////////////////////////////////////////
/// DRTAreaUpdateAction class implementation.
/// @file DRTAreaUpdateAction.cpp
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
#include "DRTAreaUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "DRTAreaTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::DRTAreaUpdateAction>::FACTORY_KEY("DRTAreaUpdateAction");
	}

	namespace pt
	{
		const string DRTAreaUpdateAction::PARAMETER_AREA_ID = Action_PARAMETER_PREFIX + "id";
		const string DRTAreaUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string DRTAreaUpdateAction::PARAMETER_STOPS = Action_PARAMETER_PREFIX + "st";

		ParametersMap DRTAreaUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_area.get())
			{
				map.insert(PARAMETER_AREA_ID, _area->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_stops)
			{
				map.insert(PARAMETER_STOPS, DRTAreaTableSync::SerializeStops(*_stops));
			}
			return map;
		}



		void DRTAreaUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getOptional<RegistryKeyType>(PARAMETER_AREA_ID)) try
			{
				_area = DRTAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_AREA_ID), *_env);
			}
			catch (ObjectNotFoundException<DRTArea>&)
			{
				throw ActionException("No such area");
			}
			else
			{
				_area.reset(new DRTArea);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			if(map.isDefined(PARAMETER_STOPS))
			{
				_stops = DRTAreaTableSync::UnserializeStops(map.get<string>(PARAMETER_STOPS), *_env);
			}
		}



		void DRTAreaUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			if(_name)
			{
				_area->setName(*_name);
			}
			if(_stops)
			{
				_area->setStops(*_stops);
			}

			DRTAreaTableSync::Save(_area.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_area->getKey());
			}
		}



		bool DRTAreaUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
