
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
#include "MessagesLibraryRight.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "AlarmObjectLinkTableSync.h"
#include "ScenarioTemplate.h"
#include "MessagesLibraryLog.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	

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
				_scenario = ScenarioTemplateInheritedTableSync::Get(id, *_env);
			}
			catch(...)
			{
				throw ActionException("Specified scenario not found.");	
			}
		}


		void DeleteScenarioAction::run(Request& request)
		{
			// The action on the alarms
			AlarmTemplateInheritedTableSync::SearchResult alarms(
				AlarmTemplateInheritedTableSync::Search(*_env,_scenario->getKey())
			);
			BOOST_FOREACH(shared_ptr<AlarmTemplate> alarm, alarms)
			{
				AlarmObjectLinkTableSync::Remove(alarm->getKey());
				AlarmTableSync::Remove(alarm->getKey());
			}

			// Action
			ScenarioTableSync::Remove(_scenario->getKey());

			// Log
			MessagesLibraryLog::addDeleteEntry(_scenario.get(), request.getUser().get());
		}

		void DeleteScenarioAction::setScenario( boost::shared_ptr<const ScenarioTemplate> scenario )
		{
			_scenario = scenario;
		}



		bool DeleteScenarioAction::isAuthorized(const Profile& profile
		) const {
			return profile.isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
		}
	}
}
