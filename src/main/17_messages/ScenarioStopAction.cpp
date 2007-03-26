
/** ScenarioStopAction class implementation.
	@file ScenarioStopAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "30_server/ActionException.h"

#include "17_messages/ScenarioStopAction.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace time;
	
	namespace messages
	{
		// const string ScenarioStopAction::PARAMETER_xxx = Action_PARAMETER_PREFIX + "xxx";


		Request::ParametersMap ScenarioStopAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void ScenarioStopAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			try
			{
				_scenario = MessagesModule::getScenarii().get(_request->getObjectId());
			}
			catch (...) {
				throw ActionException("Invalid scenario");
			}
		}

		ScenarioStopAction::ScenarioStopAction()
			: Action()
			, _scenario(NULL)
		{}

		void ScenarioStopAction::run()
		{
			_scenario->stop(DateTime());
			for (Scenario::AlarmsSet::const_iterator it = _scenario->getAlarms().begin(); it != _scenario->getAlarms().end(); ++it)
				AlarmTableSync::save(*it);
			ScenarioTableSync::save(_scenario);
		}
	}
}
