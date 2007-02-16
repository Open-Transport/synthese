
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

#include "30_server/ActionException.h"

#include "17_messages/NewScenarioSendAction.h"
#include "17_messages/Scenario.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string NewScenarioSendAction::PARAMETER_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";


		Request::ParametersMap NewScenarioSendAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void NewScenarioSendAction::setFromParametersMap(Request::ParametersMap& map)
		{
			try
			{
				Request::ParametersMap::iterator it;

				it = map.find(PARAMETER_TEMPLATE);
				if (it == map.end())
					throw ActionException("Template not specified");

				_template = MessagesModule::getScenarii().get(Conversion::ToLongLong(it->second));
				
			}
			catch (Scenario::RegistryKeyException e)
			{
				throw ActionException("Specified template not found");
			}
		}

		NewScenarioSendAction::NewScenarioSendAction()
			: Action()
			, _template(NULL)
		{}

		void NewScenarioSendAction::run()
		{
			Scenario* scenario = _template->createCopy();
			ScenarioTableSync::save(scenario);
			for (Scenario::AlarmsSet::const_iterator it = scenario->getAlarms().begin(); it != scenario->getAlarms().end(); ++it)
			{
				AlarmTableSync::save(*it);

				/// @todo Saving of the broadcast list
			}
			delete scenario;
		}
	}
}