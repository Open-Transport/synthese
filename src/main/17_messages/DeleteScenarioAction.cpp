
/** DeleteScenarioAction class implementation.
	@file DeleteScenarioAction.cpp

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
#include "30_server/Request.h"

#include "17_messages/DeleteScenarioAction.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/MessagesLibraryLog.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string DeleteScenarioAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "si";


		ParametersMap DeleteScenarioAction::getParametersMap() const
		{
			ParametersMap map;
			if (_scenario.get())
				map.insert(make_pair(PARAMETER_SCENARIO_ID, Conversion::ToString(_scenario->getKey())));
			return map;
		}

		void DeleteScenarioAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_SCENARIO_ID);
			if (it == map.end())
				throw ActionException("Scenario not specified.");
			
			try
			{
				_scenario = ScenarioTableSync::getTemplate(Conversion::ToLongLong(it->second));
			}
			catch(...)
			{
				throw ActionException("Specified scenario not found.");	
			}
		}

		DeleteScenarioAction::DeleteScenarioAction()
			: Action()
		{}

		void DeleteScenarioAction::run()
		{
			// Action
			ScenarioTableSync::remove(_scenario->getKey());

			// Log
			MessagesLibraryLog::addDeleteEntry(_scenario, _request->getUser());
		}

		void DeleteScenarioAction::setScenario( boost::shared_ptr<const ScenarioTemplate> scenario )
		{
			_scenario = scenario;
		}
	}
}
