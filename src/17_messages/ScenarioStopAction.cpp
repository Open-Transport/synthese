
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

#include "ScenarioStopAction.h"

#include "17_messages/ScenarioTableSync.h"
#include "17_messages/SentScenario.h"
#include "17_messages/SentScenarioInheritedTableSync.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesLog.h"
#include "MessagesRight.h"
#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace time;
	using namespace util;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, messages::ScenarioStopAction>::FACTORY_KEY("scenariostop");
	
	namespace messages
	{
		const string ScenarioStopAction::PARAMETER_SCENARIO_ID(
			Action_PARAMETER_PREFIX + "s"
		);
		
		ParametersMap ScenarioStopAction::getParametersMap() const
		{
			ParametersMap m;
			if(_scenario.get()) m.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			return m;
		}

		void ScenarioStopAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_scenario = SentScenarioInheritedTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID),
					*_env
				);
			}
			catch (...) {
				throw ActionException("Invalid scenario");
			}
		}

		void ScenarioStopAction::run(Request& request)
		{
			// Action
			_scenario->setPeriodEnd(_stopDateTime);
			_scenario->setIsEnabled(false);
			
			ScenarioTableSync::Save(_scenario.get());

			// Log
			MessagesLog::addUpdateEntry(_scenario.get(), "Diffusion arrêtée le " + _stopDateTime.toString(), request.getUser().get());
		}

		ScenarioStopAction::ScenarioStopAction()
			: FactorableTemplate<Action, ScenarioStopAction>(), _stopDateTime(TIME_CURRENT)
		{
	
		}



		bool ScenarioStopAction::isAuthorized(const Profile& profile
		) const {
			return profile.isAuthorized<MessagesRight>(WRITE);
		}
		
		void ScenarioStopAction::setScenario(boost::shared_ptr<SentScenario> value){
			_scenario = value;
		}
	}
}
