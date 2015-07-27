
/** ScenarioStopAction class implementation.
	@file ScenarioStopAction.cpp

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

#include "ScenarioStopAction.h"

#include "DataSource.h"
#include "DBTransaction.hpp"
#include "Profile.h"
#include "ScenarioTableSync.h"
#include "SentScenario.h"
#include "Session.h"
#include "User.h"
#include "AlarmTableSync.h"
#include "MessagesLog.h"
#include "MessagesRight.h"
#include "MessagesSection.hpp"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;	
	using namespace impex;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace messages;

	template<> const string util::FactorableTemplate<Action, messages::ScenarioStopAction>::FACTORY_KEY("scenariostop");

	namespace messages
	{
		const string ScenarioStopAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "s";
		const string ScenarioStopAction::PARAMETER_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_datasource_id";
		const string ScenarioStopAction::PARAMETER_REMOVE = Action_PARAMETER_PREFIX + "_remove";
		const string ScenarioStopAction::PARAMETER_ARCHIVE = Action_PARAMETER_PREFIX + "_archive";



		ParametersMap ScenarioStopAction::getParametersMap() const
		{
			ParametersMap m;
			if(_scenario)
			{
				m.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			}
			return m;
		}



		void ScenarioStopAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Datasource
			RegistryKeyType dataSourceId(map.getDefault<RegistryKeyType>(PARAMETER_DATASOURCE_ID, 0));
			if(dataSourceId) try
			{
				_dataSource = Env::GetOfficialEnv().get<DataSource>(dataSourceId).get();
			}
			catch(...)
			{
				throw ActionException("No such data source");
			}

			// Scenario
			if(_dataSource)
			{
				_scenario = dynamic_cast<SentScenario*>(
					_dataSource->getObjectByCode<Scenario>(map.get<string>(PARAMETER_SCENARIO_ID))
				);
				if(!_scenario)
				{
					throw ActionException("No such scenario");
				}
			}
			else
			{
				try
				{
					_scenario = ScenarioTableSync::GetCastEditable<SentScenario>(
							map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID),
							*_env
						).get();
				}
				catch (...) {
					throw ActionException("Invalid scenario");
				}
			}

			// True removal ?
			_remove = map.getDefault<bool>(PARAMETER_REMOVE, false);

			_archive = map.getDefault<bool>(PARAMETER_ARCHIVE, false);
		}



		void ScenarioStopAction::run(Request& request)
		{
			if(_remove)
			{
				DBTransaction transaction;
				ScenarioTableSync::Remove(NULL, _scenario->getKey(), transaction, false);
				transaction.run();
			}
			else
			{
				// Action
				_scenario->setPeriodEnd(_stopDateTime);
				_scenario->setIsEnabled(false);

				if (_archive)
				{
					_scenario->setArchived(true);
				}

				ScenarioTableSync::Save(_scenario);

				// Log
				MessagesLog::addUpdateEntry(_scenario, "Diffusion arrêtée le " + to_simple_string(_stopDateTime), request.getUser().get());
			}
		}



		ScenarioStopAction::ScenarioStopAction(
		): FactorableTemplate<Action, ScenarioStopAction>(),
			_scenario(NULL),
			_dataSource(NULL),
			_stopDateTime(second_clock::local_time()),
			_remove(false)
		{}



		bool ScenarioStopAction::isAuthorized(const Session* session
		) const {
/*			bool result = session && session->hasProfile();
			if (_scenario)
			{
				if (!_scenario->getSections().empty())
				{
					BOOST_FOREACH(const Scenario::Sections::value_type& section, _scenario->getSections())
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
			*/
			return true;
		}
}	}
