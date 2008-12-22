
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

#include "NewMessageAction.h"

#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTemplateInheritedTableSync.h"
#include "17_messages/SentScenario.h"
#include "17_messages/SentScenarioInheritedTableSync.h"
#include "17_messages/ScenarioSentAlarm.h"
#include "17_messages/SingleSentAlarm.h"
#include "17_messages/AlarmTemplate.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/MessagesModule.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/QueryString.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	
	template<> const string util::FactorableTemplate<Action, messages::NewMessageAction>::FACTORY_KEY("nmes");

	namespace messages
	{
		const string NewMessageAction::PARAMETER_IS_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";
		const string NewMessageAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "tps";
		const string NewMessageAction::PARAMETER_MESSAGE_TEMPLATE(Action_PARAMETER_PREFIX + "mt");

		ParametersMap NewMessageAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_IS_TEMPLATE, _isTemplate);
			if (_sentScenario.get())
				map.insert(PARAMETER_SCENARIO_ID, _sentScenario->getKey());
			if (_scenarioTemplate.get())
				map.insert(PARAMETER_SCENARIO_ID, _scenarioTemplate->getKey());
			if (_messageTemplate.get())
				map.insert(PARAMETER_MESSAGE_TEMPLATE, _messageTemplate->getKey());
			return map;
		}

		void NewMessageAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Template alarm ?
			_isTemplate = map.getBool(PARAMETER_IS_TEMPLATE, false, false, FACTORY_KEY);

			// Case template alarm
			if (_isTemplate)
			{
				uid id(map.getUid(PARAMETER_SCENARIO_ID, true, FACTORY_KEY));
				try
				{
					_scenarioTemplate = ScenarioTemplateInheritedTableSync::Get(id, _env);
				}
				catch (...)
				{
					throw ActionException("Specified scenario not found");
				}
			}
			else
			{
				uid id(map.getUid(PARAMETER_SCENARIO_ID, false, FACTORY_KEY));
				if (id != UNKNOWN_VALUE)
				{
					try
					{
						_sentScenario = SentScenarioInheritedTableSync::Get(id, _env);
					}
					catch (...)
					{
						throw ActionException("Specified scenario not found");
					}
				}
			}

			// Message template
			uid id(map.getUid(PARAMETER_MESSAGE_TEMPLATE, false, FACTORY_KEY));
			if (id != UNKNOWN_VALUE)
			try
			{
				_messageTemplate = AlarmTableSync::Get(id, _env);
			}
			catch (...)
			{
				throw ActionException("Specified template message not found");
			}

			_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void NewMessageAction::run()
		{
			if (_isTemplate)
			{
				shared_ptr<AlarmTemplate> alarm(
					_messageTemplate.get()
					? new AlarmTemplate(*static_cast<const AlarmTemplate*>(_messageTemplate.get()))
					: new AlarmTemplate(UNKNOWN_VALUE, _scenarioTemplate.get())
				);
				AlarmTableSync::Save(alarm.get());
				_request->setObjectId(alarm->getKey());
			}
			else
			{
				if (_sentScenario.get())
				{
					shared_ptr<ScenarioSentAlarm> alarm(
						_messageTemplate.get()
						? new ScenarioSentAlarm(*static_cast<const ScenarioSentAlarm*>(_messageTemplate.get()))
						: new ScenarioSentAlarm(UNKNOWN_VALUE, _sentScenario.get())
					);
					AlarmTableSync::Save(alarm.get());
					_request->setObjectId(alarm->getKey());
				}
				else
				{
					shared_ptr<SingleSentAlarm> alarm(
						_messageTemplate.get()
						? new SingleSentAlarm(*static_cast<const SingleSentAlarm*>(_messageTemplate.get()))
						: new SingleSentAlarm()
					);
					AlarmTableSync::Save(alarm.get());
					_request->setObjectId(alarm->getKey());
				}
			}
		}

		void NewMessageAction::setIsTemplate( bool value )
		{
			_isTemplate = value;
		}

		void NewMessageAction::setScenarioId(uid key)
		{
			shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(key, _env));
			_sentScenario = dynamic_pointer_cast<const SentScenario, const Scenario>(scenario);
			_scenarioTemplate = dynamic_pointer_cast<const ScenarioTemplate, const Scenario>(scenario);
		}
	}
}
