
/** NewScenarioSendAction class implementation.
	@file NewScenarioSendAction.cpp

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

#include "NewScenarioSendAction.h"

#include "SentScenario.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "ScenarioTableSync.h"
#include "AlarmObjectLinkTableSync.h"
#include "MessagesLog.h"
#include "AlarmTemplateInheritedTableSync.h"

#include "ActionException.h"
#include "Request.h"
#include "QueryString.h"
#include "ParametersMap.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace dblog;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, messages::NewScenarioSendAction>::FACTORY_KEY("nssa");
	
	namespace messages
	{
		const string NewScenarioSendAction::PARAMETER_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";


		ParametersMap NewScenarioSendAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void NewScenarioSendAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Template to source
			uid id(map.getUid(PARAMETER_TEMPLATE, true, FACTORY_KEY));
			try
			{
				_template = ScenarioTemplateInheritedTableSync::Get(id);
			}
			catch(...)
			{
				throw ActionException("specified scenario template not found");
			}
			
			// Anti error
			_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void NewScenarioSendAction::run()
		{
			// The action on the scenario
			shared_ptr<SentScenario> scenario(new SentScenario(_template->getName()));
			ScenarioTableSync::Save (scenario.get());

			// Remember of the id of created object to view it after the action
			_request->setObjectId(scenario->getKey());

			// The action on the alarms
			Env env;
			AlarmTemplateInheritedTableSync::Search(env, _template.get());
			BOOST_FOREACH(shared_ptr<AlarmTemplate> templateAlarm, env.getRegistry<AlarmTemplate>())
			{
				shared_ptr<ScenarioSentAlarm> alarm(new ScenarioSentAlarm(scenario.get(), *templateAlarm));
				AlarmTableSync::Save(alarm.get());

				Env lenv;
				AlarmObjectLinkTableSync::Search(lenv, templateAlarm.get());
				BOOST_FOREACH(shared_ptr<AlarmObjectLink> aol, lenv.getRegistry<AlarmObjectLink>())
				{
					aol->setAlarmId(alarm->getKey());
					aol->setObjectId(aol->getObjectId());
					aol->setRecipientKey(aol->getRecipientKey());
					AlarmObjectLinkTableSync::Save(aol.get());
				}
			}

			// The log
			MessagesLog::addUpdateEntry(scenario.get(), "Diffusion", _request->getUser().get());
		}
	}
}
