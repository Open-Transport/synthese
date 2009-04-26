////////////////////////////////////////////////////////////////////////////////
/// ScenarioUpdateDatesAction class implementation.
///	@file ScenarioUpdateDatesAction.cpp
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

#include "ScenarioUpdateDatesAction.h"
#include "RequestMissingParameterException.h"
#include "MessagesModule.h"
#include "ScenarioTableSync.h"
#include "ScenarioTemplate.h"
#include "SentScenarioInheritedTableSync.h"
#include "MessagesLog.h"
#include "MessagesLibraryLog.h"
#include "MessagesRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "TimeParseException.h"
#include "Conversion.h"
#include "DBLogModule.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace time;
	using namespace util;
	using namespace security;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action,messages::ScenarioUpdateDatesAction>::FACTORY_KEY("ScenarioUpdateDatesAction");
		
	namespace messages
	{
		const string ScenarioUpdateDatesAction::PARAMETER_ENABLED(Action_PARAMETER_PREFIX + "ena");
		const string ScenarioUpdateDatesAction::PARAMETER_START_DATE(Action_PARAMETER_PREFIX + "sda");
		const string ScenarioUpdateDatesAction::PARAMETER_END_DATE(Action_PARAMETER_PREFIX + "eda");
		const string ScenarioUpdateDatesAction::PARAMETER_VARIABLE(Action_PARAMETER_PREFIX + "var");
		const string ScenarioUpdateDatesAction::PARAMETER_SCENARIO_ID(Action_PARAMETER_PREFIX + "sid");
		const string ScenarioUpdateDatesAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "nam";
		const string ScenarioUpdateDatesAction::PARAMETER_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";

		ParametersMap ScenarioUpdateDatesAction::getParametersMap() const
		{
			ParametersMap map;
			if (_scenario.get()) map.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			return map;
		}

		void ScenarioUpdateDatesAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				setScenarioId(map.getUid(PARAMETER_SCENARIO_ID, true, FACTORY_KEY));

				// Name
				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);

				if(_tscenario.get())
				{
					uid folderId(map.getUid(PARAMETER_FOLDER_ID, true, FACTORY_KEY));

					if (folderId != 0)
					{
						_folder = ScenarioFolderTableSync::GetEditable(folderId, _env);
					}

					// Uniqueness control
					Env env;
					ScenarioTemplateInheritedTableSync::Search(env, folderId, _name, dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario).get(), 0, 1);
					if (!env.getRegistry<ScenarioTemplate>().empty())
						throw ActionException("Le nom spécifié est déjà utilisé par un autre scénario.");
				}

				if(_sscenario.get())
				{
					_enabled = map.getBool(PARAMETER_ENABLED, true, false, FACTORY_KEY);

					_startDate = map.getDateTime(PARAMETER_START_DATE, true, FACTORY_KEY);

					_endDate = map.getDateTime(PARAMETER_END_DATE, true, FACTORY_KEY);

					const ScenarioTemplate::VariablesMap& variables(_sscenario->getTemplate()->getVariables());
					BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
					{
						_variables.insert(make_pair(variable.second.code, map.getString(PARAMETER_VARIABLE + variable.second.code, variable.second.compulsory, FACTORY_KEY)));
					}
				}
			}
			catch(RequestMissingParameterException& e)
			{
				throw ActionException(e.getMessage());
			}
			catch(TimeParseException& e)
			{
				throw ActionException("Une date ou une heure est mal formée");
			}
		}



		ScenarioUpdateDatesAction::ScenarioUpdateDatesAction()
			: FactorableTemplate<Action, ScenarioUpdateDatesAction>()
			, _startDate(TIME_UNKNOWN)
			, _endDate(TIME_UNKNOWN)
		{}



		void ScenarioUpdateDatesAction::run()
		{
			// Log message
			stringstream text;
			DBLogModule::appendToLogIfChange(text, "Nom", _scenario->getName(), _name);
			if(_tscenario.get())
			{
				DBLogModule::appendToLogIfChange(
					text,
					"Dossier",
					_tscenario->getFolder() ? "/" : _tscenario->getFolder()->getFullName(),
					_folder.get() ? _folder->getFullName() : "/"
				);
			}
			if(_sscenario.get())
			{
				DBLogModule::appendToLogIfChange(text, "Affichage ", _sscenario->getIsEnabled() ? "activé" : "désactivé", _enabled ? "activé" : "désactivé");
				DBLogModule::appendToLogIfChange(text, "Date de début", _sscenario->getPeriodStart().toString(), _startDate.toString());
				DBLogModule::appendToLogIfChange(text, "Date de fin", _sscenario->getPeriodEnd().toString(), _endDate.toString());
			
				const ScenarioTemplate::VariablesMap& variables(_sscenario->getTemplate()->getVariables());
				BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
				{
					string value;
					SentScenario::VariablesMap::const_iterator it(_sscenario->getVariables().find(variable.second.code));
					if (it != _sscenario->getVariables().end()) value = it->second;

					DBLogModule::appendToLogIfChange(text, variable.second.code, value, _variables[variable.second.code]);
				}
			}


			// Action
			_scenario->setName(_name);
			if(_tscenario.get())
			{
				_tscenario->setFolder(_folder.get());
			}
			if(_sscenario.get())
			{
				_sscenario->setIsEnabled(_enabled);
				_sscenario->setPeriodStart(_startDate);
				_sscenario->setPeriodEnd(_endDate);
				_sscenario->setVariables(_variables);
			}
			ScenarioTableSync::Save(_scenario.get());

			// Log
			if(_sscenario.get())
			{
				MessagesLog::addUpdateEntry(_sscenario.get(), text.str(), _request->getUser().get());
			}
			else
			{
				MessagesLibraryLog::addUpdateEntry(_tscenario.get(), text.str(), _request->getUser().get());
			}
		}



		bool ScenarioUpdateDatesAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesRight>(WRITE);
		}



		void ScenarioUpdateDatesAction::setScenarioId(
			const util::RegistryKeyType id
		) throw(ActionException) {
			try
			{
				_scenario = ScenarioTableSync::GetEditable(id, _env, UP_LINKS_LOAD_LEVEL);
				_sscenario = dynamic_pointer_cast<SentScenario, Scenario>(_scenario);
				_tscenario = dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario);
			}
			catch(ObjectNotFoundException<Scenario>& e)
			{
				throw ActionException(PARAMETER_SCENARIO_ID, id, FACTORY_KEY, e);
			}
		}
	}
}
