////////////////////////////////////////////////////////////////////////////////
/// NewMessageAction class implementation.
///	@file NewMessageAction.cpp
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

#include "NewMessageAction.h"
#include "MessagesRight.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "SentScenario.h"
#include "SentScenarioInheritedTableSync.h"
#include "ScenarioSentAlarm.h"
#include "SingleSentAlarm.h"
#include "AlarmTemplate.h"
#include "AlarmTableSync.h"
#include "ScenarioTableSync.h"
#include "MessagesModule.h"
#include "ActionException.h"
#include "Request.h"
#include "QueryString.h"
#include "ParametersMap.h"
#include "MessagesLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	
	template<> const string util::FactorableTemplate<Action, messages::NewMessageAction>::FACTORY_KEY("nmes");

	namespace messages
	{
		const string NewMessageAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "tps";

		ParametersMap NewMessageAction::getParametersMap() const
		{
			ParametersMap map;
			if (_sentScenario.get())
				map.insert(PARAMETER_SCENARIO_ID, _sentScenario->getKey());
			if (_scenarioTemplate.get())
				map.insert(PARAMETER_SCENARIO_ID, _scenarioTemplate->getKey());
			return map;
		}

		void NewMessageAction::_setFromParametersMap(const ParametersMap& map) throw(ActionException)
		{
			setScenarioId(map.getUid(PARAMETER_SCENARIO_ID, true, FACTORY_KEY));
		}

		void NewMessageAction::run() throw(ActionException)
		{
			if (_scenarioTemplate.get())
			{
				AlarmTemplate alarm(UNKNOWN_VALUE, _scenarioTemplate.get());
				AlarmTableSync::Save(&alarm);
				
				if(_request->getObjectId() == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				{
					_request->setObjectId(alarm.getKey());
				}
				
				MessagesLog::AddNewScenarioMessageEntry(
					alarm,
					*_scenarioTemplate,
					_request->getUser().get()
				);
			}
			else
			{
				ScenarioSentAlarm alarm(UNKNOWN_VALUE, _sentScenario.get());
					
				AlarmTableSync::Save(&alarm);
				
				_request->setObjectId(alarm.getKey());
			
				MessagesLog::AddNewScenarioMessageEntry(
					alarm,
					*_sentScenario,
					_request->getUser().get()
				);
			}
		}



		void NewMessageAction::setScenarioId(uid key)
		{
			try
			{
				boost::shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(key, _env));
				_sentScenario = dynamic_pointer_cast<const SentScenario, const Scenario>(scenario);
				_scenarioTemplate = dynamic_pointer_cast<const ScenarioTemplate, const Scenario>(scenario);
			}
			catch(Exception& e)
			{
				throw ActionException("Scenario", key, FACTORY_KEY, e);
			}
		}



		bool NewMessageAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesRight>(WRITE);
		}
	}
}
