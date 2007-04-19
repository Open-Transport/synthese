
/** NewMessageAction class implementation.
	@file NewMessageAction.cpp

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

#include "17_messages/NewMessageAction.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioSentAlarm.h"
#include "17_messages/SingleSentAlarm.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/MessagesModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string NewMessageAction::PARAMETER_IS_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";
		const string NewMessageAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "tps";

		ParametersMap NewMessageAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_IS_TEMPLATE, Conversion::ToString(_isTemplate)));
			if (_scenario.get())
				map.insert(make_pair(PARAMETER_SCENARIO_ID, Conversion::ToString(_scenario->getId())));
			return map;
		}

		void NewMessageAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			it = map.find(PARAMETER_IS_TEMPLATE);
			if (it != map.end())
			{
				_isTemplate = Conversion::ToBool(it->second);
			}

			if (_isTemplate)
			{
				it = map.find(PARAMETER_SCENARIO_ID);
				if (it == map.end())
					throw ActionException("Scenario not specified");
				try
				{
					_scenarioTemplate = ScenarioTableSync::getTemplate(Conversion::ToLongLong(it->second));
					_scenario = static_pointer_cast<Scenario, ScenarioTemplate>(_scenarioTemplate);
				}
				catch (...)
				{
					throw ActionException("Specified scenario not found");
				}
			}
			else
			{
				it = map.find(PARAMETER_SCENARIO_ID);
				if (it != map.end())
				{
					try
					{
						_sentScenario = ScenarioTableSync::getSent(Conversion::ToLongLong(it->second));
						_scenario = static_pointer_cast<Scenario, SentScenario>(_sentScenario);
					}
					catch (...)
					{
						throw ActionException("Specified scenario not found");
					}
				}
			}

			_request->setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void NewMessageAction::run()
		{
			if (_isTemplate)
			{
				shared_ptr<AlarmTemplate> alarm(new AlarmTemplate(_scenarioTemplate->getKey()));
				AlarmTableSync::save(alarm.get());
				_request->setObjectId(alarm->getKey());
			}
			else
			{
				if (_sentScenario.get())
				{
					shared_ptr<ScenarioSentAlarm> alarm(new ScenarioSentAlarm(*_sentScenario));
					AlarmTableSync::save(alarm.get());
					_request->setObjectId(alarm->getKey());
				}
				else
				{
					shared_ptr<SingleSentAlarm> alarm(new SingleSentAlarm);
					AlarmTableSync::save(alarm.get());
					_request->setObjectId(alarm->getKey());
				}
			}
		}

		void NewMessageAction::setIsTemplate( bool value )
		{
			_isTemplate = value;
		}

		void NewMessageAction::setScenarioId(uid scenario )
		{
			_sentScenario = ScenarioTableSync::getSent(scenario);
			_scenarioTemplate = ScenarioTableSync::getTemplate(scenario);
			_scenario = _sentScenario.get()
				? static_pointer_cast<Scenario, SentScenario>(_sentScenario) 
				: static_pointer_cast<Scenario, ScenarioTemplate>(_scenarioTemplate);
		}
	}
}