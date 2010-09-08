
/** ScenarioVariablesUpdateAction class implementation.
	@file ScenarioVariablesUpdateAction.cpp
	@author Hugues
	@date 2010

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ScenarioVariablesUpdateAction.h"
#include "MessagesRight.h"
#include "Request.h"
#include "SentScenarioInheritedTableSync.h"
#include "ScenarioTemplate.h"
#include "DBLogModule.h"
#include "MessagesLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace db;
	using namespace util;
	using namespace dblog;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, messages::ScenarioVariablesUpdateAction>::FACTORY_KEY("ScenarioVariablesUpdateAction");
	}

	namespace messages
	{
		const string ScenarioVariablesUpdateAction::PARAMETER_VARIABLE(Action_PARAMETER_PREFIX + "var");
		const string ScenarioVariablesUpdateAction::PARAMETER_SCENARIO_ID(Action_PARAMETER_PREFIX + "sid");
		
		
		ParametersMap ScenarioVariablesUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_sscenario.get()) map.insert(PARAMETER_SCENARIO_ID, _sscenario->getKey());
			return map;
		}
		
		
		
		void ScenarioVariablesUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_sscenario = SentScenarioInheritedTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID), *_env, UP_LINKS_LOAD_LEVEL);
			}
			catch(ObjectNotFoundException<Scenario>& e)
			{
				throw ActionException(PARAMETER_SCENARIO_ID, e, *this);
			}

			if(!_sscenario->getTemplate())
			{
				throw ActionException("The selected scenario is malformed (no template)");
			}
			
			const ScenarioTemplate::VariablesMap& variables(_sscenario->getTemplate()->getVariables());
			BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
			{
				_variables.insert(make_pair(
						variable.second.code,
						variable.second.compulsory ? map.get<string>(PARAMETER_VARIABLE + variable.second.code) : map.getDefault<string>(PARAMETER_VARIABLE + variable.second.code)
				)	);
			}
		}
		
		
		
		void ScenarioVariablesUpdateAction::run(Request& request)
		{
			const ScenarioTemplate::VariablesMap& variables(_sscenario->getTemplate()->getVariables());
			stringstream text;
			BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
			{
				string value;
				SentScenario::VariablesMap::const_iterator it(_sscenario->getVariables().find(variable.second.code));
				if (it != _sscenario->getVariables().end()) value = it->second;

				DBLogModule::appendToLogIfChange(text, variable.second.code, value, _variables[variable.second.code]);
			}

			_sscenario->setVariables(_variables);

			SentScenarioInheritedTableSync::WriteVariablesIntoMessages(*_sscenario);
			ScenarioTableSync::Save(_sscenario.get());

			MessagesLog::addUpdateEntry(_sscenario.get(), text.str(), request.getUser().get());
		}
		
		
		
		bool ScenarioVariablesUpdateAction::isAuthorized(
			const Session* session
		) const {
			return
				session &&
				session->hasProfile() &&
				session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE)
			;
		}



		void ScenarioVariablesUpdateAction::setScenario( boost::shared_ptr<SentScenario> value )
		{
			_sscenario = value;
		}
	}
}
