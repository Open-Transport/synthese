////////////////////////////////////////////////////////////////////////////////
/// NewMessageAction class implementation.
///	@file NewMessageAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "Profile.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
#include "Alarm.h"
#include "Session.h"
#include "User.h"
#include "AlarmTableSync.h"
#include "SentScenarioTableSync.h"
#include "ScenarioTemplateTableSync.h"
#include "MessagesModule.h"
#include "ActionException.h"
#include "Request.h"
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
			try
			{
				setScenarioId(map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID));
			}
			catch(ParametersMap::MissingParameterException e)
			{
				throw ActionException(e, *this);
			}
		}



		void NewMessageAction::run(
			Request& request
		) throw(ActionException) {
			if (_scenarioTemplate.get())
			{
				Alarm alarm;
				alarm.setScenario(_scenarioTemplate.get());
				AlarmTableSync::Save(&alarm);

				request.setActionCreatedId(alarm.getKey());

				MessagesLog::AddNewScenarioMessageEntry(
					alarm,
					*_scenarioTemplate,
					request.getUser().get()
				);
			}
			else
			{
				Alarm alarm;
				alarm.setScenario(_sentScenario.get());
				AlarmTableSync::Save(&alarm);

				request.setActionCreatedId(alarm.getKey());

				MessagesLog::AddNewScenarioMessageEntry(
					alarm,
					*_sentScenario,
					request.getUser().get()
				);
			}
		}



		void NewMessageAction::setScenarioId(RegistryKeyType key)
		{
			util::RegistryTableType tableId(util::decodeTableId(key));
			if (tableId == ScenarioTemplateTableSync::TABLE.ID)
			{
				try
				{
					_scenarioTemplate = ScenarioTemplateTableSync::Get(key, *_env);
					_sentScenario.reset();
				}
				catch(ObjectNotFoundException<ScenarioTemplate>& e)
				{
					throw ActionException("Scenario", e, *this);
				}
			}
			else if (tableId == SentScenarioTableSync::TABLE.ID)
			{
				try
				{
					_sentScenario = SentScenarioTableSync::Get(key, *_env);
					_scenarioTemplate.reset();
				}
				catch(ObjectNotFoundException<SentScenario>& e)
				{
					throw ActionException("Scenario", e, *this);
				}
			}
		}



		bool NewMessageAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
		}
	}
}
