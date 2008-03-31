
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

#include "DeleteScenarioAction.h"

#include "17_messages/ScenarioTemplateInheritedTableSync.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/AlarmObjectLinkTableSync.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/MessagesLibraryLog.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<> const string util::FactorableTemplate<Action,messages::DeleteScenarioAction>::FACTORY_KEY("mdsca");
	
	namespace messages
	{
		const string DeleteScenarioAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "si";


		ParametersMap DeleteScenarioAction::getParametersMap() const
		{
			ParametersMap map;
			if (_scenario.get())
				map.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			return map;
		}

		void DeleteScenarioAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_SCENARIO_ID, true, FACTORY_KEY));
			try
			{
				_scenario.reset(ScenarioTemplateInheritedTableSync::Get(id));
			}
			catch(...)
			{
				throw ActionException("Specified scenario not found.");	
			}
		}


		void DeleteScenarioAction::run()
		{

			// The action on the alarms
			vector<shared_ptr<AlarmTemplate> > alarms = AlarmTableSync::searchTemplates(_scenario.get());
			for (vector<shared_ptr<AlarmTemplate> >::const_iterator it = alarms.begin(); it != alarms.end(); ++it)
			{
				AlarmObjectLinkTableSync::Remove((*it)->getKey());
				AlarmTableSync::Remove((*it)->getKey());
			}

			// Action
			ScenarioTableSync::Remove(_scenario->getKey());

			// Log
			MessagesLibraryLog::addDeleteEntry(_scenario.get(), _request->getUser().get());
		}

		void DeleteScenarioAction::setScenario( boost::shared_ptr<const ScenarioTemplate> scenario )
		{
			_scenario = scenario;
		}
	}
}
