////////////////////////////////////////////////////////////////////////////////
/// AlarmStopAction class implementation.
///	@file AlarmStopAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "AlarmStopAction.h"
#include "SingleSentAlarm.h"
#include "SingleSentAlarmInheritedTableSync.h"
#include "MessagesLog.h"
#include "MessagesRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "ObjectNotFoundException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace time;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, messages::AlarmStopAction>::FACTORY_KEY("masa");
	
	namespace messages
	{
		const string AlarmStopAction::PARAMETER_ALARM_ID(Action_PARAMETER_PREFIX + "ai");


		ParametersMap AlarmStopAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void AlarmStopAction::_setFromParametersMap(const ParametersMap& map)
			throw(ActionException)
		{
			try
			{
				_alarm = SingleSentAlarmInheritedTableSync::GetEditable(_request->getObjectId(), _env);
			}
			catch(ObjectNotFoundException<SingleSentAlarm>& e)
			{
				throw ActionException("alarm", FACTORY_KEY, e);
			}

			DateTime now(TIME_CURRENT);
			if (!_alarm->getPeriodEnd().isUnknown() && _alarm->getPeriodEnd() < now)
			{
				throw ActionException("This alarm has not to be stopped.");
			}
		}

		AlarmStopAction::AlarmStopAction()
			: FactorableTemplate<Action,AlarmStopAction>()
			, _stopDateTime(TIME_CURRENT)
		{}

		void AlarmStopAction::run() throw(ActionException)
		{
			// Action
			_alarm->setPeriodEnd(_stopDateTime);
			_alarm->setIsEnabled(false);
			AlarmTableSync::Save(_alarm.get());

			// Log
			MessagesLog::addUpdateEntry(dynamic_pointer_cast<const SingleSentAlarm, SingleSentAlarm>(_alarm).get(), "Diffusion arrêtée le " + _stopDateTime.toString(), _request->getUser().get());
		}



		bool AlarmStopAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesRight>(WRITE);
		}
	}
}
