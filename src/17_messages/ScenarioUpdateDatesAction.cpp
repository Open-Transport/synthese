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
#include "Conversion.h"
#include "DBLogModule.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace dblog;

	template<> const string util::FactorableTemplate<Action,messages::ScenarioUpdateDatesAction>::FACTORY_KEY("ScenarioUpdateDatesAction");
		
	namespace messages
	{
		const string ScenarioUpdateDatesAction::PARAMETER_VARIABLE(Action_PARAMETER_PREFIX + "var");
		const string ScenarioUpdateDatesAction::PARAMETER_ENABLED(Action_PARAMETER_PREFIX + "ena");
		const string ScenarioUpdateDatesAction::PARAMETER_START_DATE(Action_PARAMETER_PREFIX + "sda");
		const string ScenarioUpdateDatesAction::PARAMETER_END_DATE(Action_PARAMETER_PREFIX + "eda");
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
				setScenarioId(map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID));

				// Name
				if(map.isDefined(PARAMETER_NAME))
				{
					_name = map.get<string>(PARAMETER_NAME);
				}

				// Template scenario only
				if(_tscenario.get())
				{
					// Folder
					if(map.isDefined(PARAMETER_FOLDER_ID))
					{
						RegistryKeyType folderId(map.get<RegistryKeyType>(PARAMETER_FOLDER_ID));
						if (folderId == 0)
						{
							_folder = shared_ptr<ScenarioFolder>();
						}
						else
						{
							_folder = ScenarioFolderTableSync::GetEditable(folderId, *_env);
						}

						// Uniqueness control
						Env env;
						ScenarioTemplateInheritedTableSync::Search(
							env,
							folderId,
							_name ? *_name : _scenario->getName(),
							dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario).get(),
							0,
							1
						);
						if (!env.getRegistry<ScenarioTemplate>().empty())
						{
							throw ActionException("Le nom spécifié est déjà utilisé par un autre scénario.");
						}
					}
				}
				
				// Sent scenario only
				if(_sscenario.get())
				{
					// Enabled
					if(map.isDefined(PARAMETER_ENABLED))
					{
						_enabled = map.get<bool>(PARAMETER_ENABLED);
					}

					// Start date
					if(map.isDefined(PARAMETER_START_DATE))
					{
						if(map.get<string>(PARAMETER_START_DATE).empty())
						{
							_startDate = ptime();
						}
						else
						{
							_startDate = time_from_string(map.get<string>(PARAMETER_START_DATE));
						}
					}

					// End date
					if(map.isDefined(PARAMETER_END_DATE))
					{
						if(map.get<string>(PARAMETER_END_DATE).empty())
						{
							_endDate = ptime();
						}
						else
						{
							_endDate = time_from_string(map.get<string>(PARAMETER_END_DATE));
						}
					}

					// Variables
					if(_sscenario->getTemplate())
					{
						const ScenarioTemplate::VariablesMap& variables(_sscenario->getTemplate()->getVariables());
						BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
						{
							if(!map.isDefined(PARAMETER_VARIABLE + variable.second.code))
							{
								if(	variable.second.compulsory &&
									(	_enabled && *_enabled ||
										!_enabled && _sscenario->getIsEnabled()
									)
								){
									SentScenario::VariablesMap::const_iterator it(
										_sscenario->getVariables().find(variable.second.code)
									);
									if(it == _sscenario->getVariables().end() || it->second.empty())
									{
										throw ActionException("The variable "+ variable.first +" is still undefined : the scenario cannot be active");
									}
								}
								continue;
							}
							string value(
								map.get<string>(PARAMETER_VARIABLE + variable.second.code)
							);
							if(	variable.second.compulsory &&
								(	_enabled && *_enabled ||
									!_enabled && _sscenario->getIsEnabled()
								) &&
								value.empty()
							){
								throw ActionException("Variable "+ variable.first +" must be defined to activate the scenario.");
							}
							_variables.insert(make_pair(
									variable.second.code,
									value
							)	);
						}
					}

				}
			}
			catch(ParametersMap::MissingParameterException& e)
			{
				throw ActionException(e, *this);
			}
		}



		ScenarioUpdateDatesAction::ScenarioUpdateDatesAction()
			: FactorableTemplate<Action, ScenarioUpdateDatesAction>()
		{}



		void ScenarioUpdateDatesAction::run(Request& request)
		{
			// Log message
			stringstream text;

			// Name
			if(_name)
			{
				DBLogModule::appendToLogIfChange(text, "Nom", _scenario->getName(), *_name);
				_scenario->setName(*_name);
			}

			if(_tscenario.get())
			{
				// Folder
				if(_folder)
				{
					DBLogModule::appendToLogIfChange(
						text,
						"Dossier",
						_tscenario->getFolder() ? "/" : _tscenario->getFolder()->getFullName(),
						_folder->get() ? (*_folder)->getFullName() : "/"
					);
					_tscenario->setFolder(_folder->get());
				}
			}
			if(_sscenario.get())
			{
				// Enabled
				if(_enabled)
				{
					DBLogModule::appendToLogIfChange(
						text,
						"Affichage ",
						_sscenario->getIsEnabled() ? "activé" : "désactivé",
						*_enabled ? "activé" : "désactivé"
					);
					_sscenario->setIsEnabled(*_enabled);
				}

				// Start date
				if(_startDate)
				{
					DBLogModule::appendToLogIfChange(
						text,
						"Date de début",
						to_simple_string(_sscenario->getPeriodStart()),
						to_simple_string(*_startDate)
					);
					_sscenario->setPeriodStart(*_startDate);
				}

				// End date
				if(_endDate)
				{
					DBLogModule::appendToLogIfChange(
						text,
						"Date de fin",
						to_simple_string(_sscenario->getPeriodEnd()),
						to_simple_string(*_endDate)
					);
					_sscenario->setPeriodEnd(*_endDate);
				}

				// Variables
				if(_sscenario->getTemplate())
				{
					const ScenarioTemplate::VariablesMap& variables(_sscenario->getTemplate()->getVariables());
					stringstream text;
					SentScenario::VariablesMap values;
					BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
					{
						SentScenario::VariablesMap::const_iterator itNew(
							_variables.find(variable.first)
						);
						SentScenario::VariablesMap::const_iterator itExist(
							_sscenario->getVariables().find(variable.first)
						);
						string oldValue(
							itExist == _sscenario->getVariables().end() ?
							string() :
							itExist->second
						);
						string newValue(
							itNew == _variables.end() ?
							oldValue :
							itNew->second
						);
						DBLogModule::appendToLogIfChange(
							text,
							variable.first,
							oldValue,
							newValue
						);
						values.insert(make_pair(
								variable.first,
								newValue
						)	);
					}
					_sscenario->setVariables(values);
				}

			}

			// Save
			ScenarioTableSync::Save(_scenario.get());

			// Log
			if(_sscenario.get())
			{
				MessagesLog::addUpdateEntry(_sscenario.get(), text.str(), request.getUser().get());
			}
			else
			{
				MessagesLibraryLog::addUpdateEntry(_tscenario.get(), text.str(), request.getUser().get());
			}
		}



		bool ScenarioUpdateDatesAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
		}



		void ScenarioUpdateDatesAction::setScenarioId(
			const util::RegistryKeyType id
		) throw(ActionException) {
			try
			{
				_scenario = ScenarioTableSync::GetEditable(id, *_env, UP_LINKS_LOAD_LEVEL);
				_sscenario = dynamic_pointer_cast<SentScenario, Scenario>(_scenario);
				_tscenario = dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario);
			}
			catch(ObjectNotFoundException<Scenario>& e)
			{
				throw ActionException(PARAMETER_SCENARIO_ID, e, *this);
			}
		}
	}
}
