
//////////////////////////////////////////////////////////////////////////
/// DRTAreaUpdateAction class implementation.
/// @file DRTAreaUpdateAction.cpp
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

#include "FreeDRTAreaUpdateAction.hpp"

#include "ActionException.h"
#include "CommercialLineTableSync.h"
#include "FreeDRTAreaTableSync.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "RollingStock.hpp"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::FreeDRTAreaUpdateAction>::FACTORY_KEY("FreeDRTAreaUpdate");
	}

	namespace pt
	{
		const string FreeDRTAreaUpdateAction::PARAMETER_AREA_ID = Action_PARAMETER_PREFIX + "id";
		const string FreeDRTAreaUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";
		const string FreeDRTAreaUpdateAction::PARAMETER_CITIES = Action_PARAMETER_PREFIX + "cities";
		const string FreeDRTAreaUpdateAction::PARAMETER_STOP_AREAS = Action_PARAMETER_PREFIX + "stop_areas";
		const string FreeDRTAreaUpdateAction::PARAMETER_COMMERCIAL_LINE_ID = Action_PARAMETER_PREFIX + "line_id";

		ParametersMap FreeDRTAreaUpdateAction::getParametersMap() const
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
			if(_cities)
			{
				map.insert(PARAMETER_CITIES, FreeDRTArea::SerializeCities(*_cities));
			}
			if(_stopAreas)
			{
				map.insert(PARAMETER_STOP_AREAS, FreeDRTArea::SerializeStopAreas(*_stopAreas));
			}
			if(_line && _line->get())
			{
				map.insert(PARAMETER_COMMERCIAL_LINE_ID, (*_line)->getKey());
			}
			return map;
		}



		void FreeDRTAreaUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getOptional<RegistryKeyType>(PARAMETER_AREA_ID)) try
			{
				_area = FreeDRTAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_AREA_ID), *_env);
			}
			catch (ObjectNotFoundException<FreeDRTArea>&)
			{
				throw ActionException("No such area");
			}
			else
			{
				_area.reset(new FreeDRTArea);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			if(map.isDefined(PARAMETER_CITIES))
			{
				_cities = FreeDRTArea::UnserializeCities(map.get<string>(PARAMETER_CITIES), *_env);
			}

			if(map.isDefined(PARAMETER_STOP_AREAS))
			{
				_stopAreas = FreeDRTArea::UnserializeStopAreas(map.get<string>(PARAMETER_STOP_AREAS), *_env);
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_COMMERCIAL_LINE_ID, 0))
			{
				try
				{
					_line = CommercialLineTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_COMMERCIAL_LINE_ID), *_env);
				}
				catch (ObjectNotFoundException<CommercialLine>&)
				{
					throw ActionException("No such line");
				}
			}
		}



		void FreeDRTAreaUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			if(_name)
			{
				_area->setName(*_name);
			}
			if(_cities)
			{
				_area->setCities(*_cities);
			}
			if(_stopAreas)
			{
				_area->setStopAreas(*_stopAreas);
			}
			if(_line)
			{
				_area->setLine(_line->get());
			}

			FreeDRTAreaTableSync::Save(_area.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_area->getKey());
			}
		}



		bool FreeDRTAreaUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
}	}
