
/** ScenarioRemoveAction class implementation.
	@file ScenarioRemoveAction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ScenarioRemoveAction.h"

#include "ActionException.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRecipient.h"
#include "AlarmTableSync.h"
#include "DBTransaction.hpp"
#include "MessageAlternativeTableSync.hpp"
#include "MessageApplicationPeriod.hpp"
#include "MessageApplicationPeriodTableSync.hpp"
#include "MessagesRight.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Scenario.h"
#include "ScenarioCalendarTableSync.hpp"
#include "SentScenarioTableSync.h"
#include "SentScenario.h"
#include "Session.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;	
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace messages;

	template<> const string util::FactorableTemplate<Action, messages::ScenarioRemoveAction>::FACTORY_KEY("scenarioremove");

	namespace messages
	{
		const string ScenarioRemoveAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "s";



		ParametersMap ScenarioRemoveAction::getParametersMap() const
		{
			ParametersMap m;
			if(_scenario)
			{
				m.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			}
			return m;
		}



		void ScenarioRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Scenario
			try
			{
				_scenario = SentScenarioTableSync::GetCastEditable<SentScenario>(
					map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID),
					*_env
				).get();
			}
			catch (...) {
				throw ActionException("Invalid scenario");
			}
		}



		void ScenarioRemoveAction::run(Request& request)
		{
			DBTransaction transaction;
			std::set<ScenarioCalendar*> existingCalendars(_scenario->getCalendars());
			std::set<const Alarm*> existingMessages(_scenario->getMessages());
			BOOST_FOREACH(ScenarioCalendar* calendar, existingCalendars)
			{
				ScenarioCalendar::ApplicationPeriods existingPeriods(calendar->getApplicationPeriods());
				BOOST_FOREACH(MessageApplicationPeriod* period, existingPeriods)
				{
					MessageApplicationPeriodTableSync::Remove(request.getSession().get(), period->getKey(), transaction, false);
				}
			}
			BOOST_FOREACH(const Alarm* message, existingMessages)
			{
				Alarm::MessageAlternatives existingAlternatives(message->getMessageAlternatives());
				BOOST_FOREACH(const Alarm::MessageAlternatives::value_type& alternative, existingAlternatives)
				{
					MessageAlternativeTableSync::Remove(request.getSession().get(), alternative.second->getKey(), transaction, false);
				}
				BOOST_FOREACH(boost::shared_ptr<AlarmRecipient> linkType, Factory<AlarmRecipient>::GetNewCollection())
				{
					Alarm::LinkedObjects::mapped_type existingLinks(message->getLinkedObjects(linkType->getFactoryKey()));
					BOOST_FOREACH(const AlarmObjectLink* link, existingLinks)
					{
						AlarmObjectLinkTableSync::Remove(request.getSession().get(), link->getKey(), transaction, false);
					}
				}
			}
			BOOST_FOREACH(ScenarioCalendar* calendar, existingCalendars)
			{
				ScenarioCalendarTableSync::Remove(request.getSession().get(), calendar->getKey(), transaction, false);
			}
			BOOST_FOREACH(const Alarm* message, existingMessages)
			{
				AlarmTableSync::Remove(request.getSession().get(), message->getKey(), transaction, false);
			}

			SentScenarioTableSync::Remove(NULL, _scenario->getKey(), transaction, false);
			transaction.run();
		}



		ScenarioRemoveAction::ScenarioRemoveAction(
		): FactorableTemplate<Action, ScenarioRemoveAction>(),
			_scenario(NULL)
		{}



		bool ScenarioRemoveAction::isAuthorized(const Session* session
		) const {
			bool result = session && session->hasProfile();
			if (_scenario)
			{
				if (!_scenario->getSections().empty())
				{
					BOOST_FOREACH(const MessagesSection* section, _scenario->get<Sections>())
					{
						result = result && session->getUser()->getProfile()->isAuthorized<MessagesRight>(
							DELETE_RIGHT,
							UNKNOWN_RIGHT_LEVEL,
							MessagesRight::MESSAGES_SECTION_FACTORY_KEY + "/" + lexical_cast<string>(section->getKey())
						);
					}
				}
			}
			return result;
		}
}	}
