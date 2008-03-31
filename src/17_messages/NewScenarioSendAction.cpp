
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

#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTemplateInheritedTableSync.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/AlarmObjectLinkTableSync.h"
#include "17_messages/MessagesLog.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/QueryString.h"
#include "30_server/ParametersMap.h"

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
				_template.reset(ScenarioTemplateInheritedTableSync::Get(id));
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
			vector<shared_ptr<AlarmTemplate> > alarms = AlarmTableSync::searchTemplates(_template.get());
			for (vector<shared_ptr<AlarmTemplate> >::const_iterator it = alarms.begin(); it != alarms.end(); ++it)
			{
				shared_ptr<ScenarioSentAlarm> alarm(new ScenarioSentAlarm(scenario.get(), **it));
				AlarmTableSync::Save(alarm.get());

				vector<shared_ptr<AlarmObjectLink> > aols = AlarmObjectLinkTableSync::search(it->get());
				for (vector<shared_ptr<AlarmObjectLink> >::const_iterator itaol = aols.begin(); itaol != aols.end(); ++itaol)
				{
					shared_ptr<AlarmObjectLink> aol(new AlarmObjectLink);
					aol->setAlarmId(alarm->getKey());
					aol->setObjectId((*itaol)->getObjectId());
					aol->setRecipientKey((*itaol)->getRecipientKey());
					AlarmObjectLinkTableSync::save(aol.get());
				}
			}

			// The log
			MessagesLog::addUpdateEntry(scenario.get(), "Diffusion", _request->getUser().get());
		}
	}
}
