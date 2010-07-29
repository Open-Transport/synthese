
//////////////////////////////////////////////////////////////////////////
/// LineStopUpdateAction class implementation.
/// @file LineStopUpdateAction.cpp
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
#include "LineStopUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "LineStopTableSync.h"
#include "PhysicalStopTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::LineStopUpdateAction>::FACTORY_KEY("LineStopUpdateAction");
	}

	namespace pt
	{
		const string LineStopUpdateAction::PARAMETER_LINE_STOP_ID = Action_PARAMETER_PREFIX + "id";
		const string LineStopUpdateAction::PARAMETER_PHYSICAL_STOP_ID = Action_PARAMETER_PREFIX + "ps";
		const string LineStopUpdateAction::PARAMETER_ALLOWED_DEPARTURE = Action_PARAMETER_PREFIX + "ad";
		const string LineStopUpdateAction::PARAMETER_ALLOWED_ARRIVAL = Action_PARAMETER_PREFIX + "aa";
		
		
		
		ParametersMap LineStopUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_lineStop.get())
			{
				map.insert(PARAMETER_LINE_STOP_ID, _lineStop->getKey());
			}
			if(_physicalStop.get())
			{
				map.insert(PARAMETER_PHYSICAL_STOP_ID, _physicalStop->getKey());
			}
			if(_allowedDeparture)
			{
				map.insert(PARAMETER_ALLOWED_DEPARTURE, *_allowedDeparture);
			}
			if(_allowedArrival)
			{
				map.insert(PARAMETER_ALLOWED_ARRIVAL, *_allowedArrival);
			}
			return map;
		}
		
		
		
		void LineStopUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_lineStop = LineStopTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_LINE_STOP_ID), *_env);
				RegistryKeyType psid(map.getDefault<RegistryKeyType>(PARAMETER_PHYSICAL_STOP_ID, 0));
				if(psid)
				{
					_physicalStop = PhysicalStopTableSync::GetEditable(psid, *_env);
				}
				_allowedArrival = map.getOptional<bool>(PARAMETER_ALLOWED_ARRIVAL);
				_allowedDeparture = map.getOptional<bool>(PARAMETER_ALLOWED_DEPARTURE);
			}
			catch(ObjectNotFoundException<LineStop>&)
			{
				throw ActionException("No such line stop");
			}
			catch(ObjectNotFoundException<StopPoint>&)
			{
				throw ActionException("No such physical stop");
			}
		}
		
		
		
		void LineStopUpdateAction::run(
			Request& request
		){
			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_physicalStop.get())
			{
				_lineStop->setPhysicalStop(_physicalStop.get());
			}
			if(_allowedArrival)
			{
				_lineStop->setIsArrival(*_allowedArrival);
			}
			if(_allowedDeparture)
			{
				_lineStop->setIsDeparture(*_allowedDeparture);
			}

			LineStopTableSync::Save(_lineStop.get());

			//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool LineStopUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
