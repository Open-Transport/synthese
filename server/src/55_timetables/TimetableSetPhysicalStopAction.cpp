
//////////////////////////////////////////////////////////////////////////
/// TimetableSetPhysicalStopAction class implementation.
/// @file TimetableSetPhysicalStopAction.cpp
/// @author Hugues
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

#include "TimetableSetPhysicalStopAction.h"

#include "ActionException.h"
#include "Env.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Timetable.h"
#include "TimetableTableSync.h"
#include "TimetableRight.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::TimetableSetPhysicalStopAction>::FACTORY_KEY("TimetableSetPhysicalStopAction");
	}

	namespace timetables
	{
		const string TimetableSetPhysicalStopAction::PARAMETER_PHYSICAL_STOP_ID = Action_PARAMETER_PREFIX + "pi";
		const string TimetableSetPhysicalStopAction::PARAMETER_TIMETABLE_ID = Action_PARAMETER_PREFIX + "ti";



		ParametersMap TimetableSetPhysicalStopAction::getParametersMap() const
		{
			ParametersMap map;
			if(_timetable.get())
			{
				map.insert(PARAMETER_TIMETABLE_ID, _timetable->getKey());
			}
			if(_physicalStop.get())
			{
				map.insert(PARAMETER_PHYSICAL_STOP_ID, _physicalStop->getKey());
			}
			return map;
		}



		void TimetableSetPhysicalStopAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_timetable = TimetableTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TIMETABLE_ID), *_env);
			}
			catch(ObjectNotFoundException<Timetable>& e)
			{
				throw ActionException("No such timetable", e, *this);
			}

			try
			{
				_physicalStop = Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(PARAMETER_PHYSICAL_STOP_ID));
			}
			catch(ObjectNotFoundException<StopPoint>& e)
			{
				throw ActionException("No such physical stop", e, *this);
			}
		}



		void TimetableSetPhysicalStopAction::run(
			Request& request
		){
			if(_timetable->getAuthorizedPhysicalStops().find(_physicalStop.get()) == _timetable->getAuthorizedPhysicalStops().end())
			{
				_timetable->addAuthorizedPhysicalStop(_physicalStop.get());
			}
			else
			{
				_timetable->removeAuthorizedPhysicalStop(_physicalStop.get());
			}
			TimetableTableSync::Save(_timetable.get());
		}



		bool TimetableSetPhysicalStopAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(WRITE);
		}



		void TimetableSetPhysicalStopAction::setTimetable( boost::shared_ptr<Timetable> value )
		{
			_timetable = value;
		}



		void TimetableSetPhysicalStopAction::setPhysicalStop( boost::shared_ptr<const pt::StopPoint> value )
		{
			_physicalStop = value;
		}
	}
}
