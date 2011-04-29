////////////////////////////////////////////////////////////////////////////////
/// ScenarioSaveAction class implementation.
///	@file ScenarioSaveAction.cpp
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

#include "ScenarioSaveAction.h"
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
#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "MessagesTypes.h"

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

	template<> const string util::FactorableTemplate<Action,messages::ScenarioSaveAction>::FACTORY_KEY("scenario_save");

	namespace messages
	{
		const string ScenarioSaveAction::PARAMETER_CREATE_TEMPLATE(Action_PARAMETER_PREFIX + "t");
		const string ScenarioSaveAction::PARAMETER_VARIABLE(Action_PARAMETER_PREFIX + "var");
		const string ScenarioSaveAction::PARAMETER_ENABLED(Action_PARAMETER_PREFIX + "ena");
		const string ScenarioSaveAction::PARAMETER_START_DATE(Action_PARAMETER_PREFIX + "sda");
		const string ScenarioSaveAction::PARAMETER_END_DATE(Action_PARAMETER_PREFIX + "eda");
		const string ScenarioSaveAction::PARAMETER_SCENARIO_ID(Action_PARAMETER_PREFIX + "sid");
		const string ScenarioSaveAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "nam";
		const string ScenarioSaveAction::PARAMETER_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";
		const string ScenarioSaveAction::PARAMETER_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_TO_COPY(Action_PARAMETER_PREFIX + "mt");
		const string ScenarioSaveAction::PARAMETER_MESSAGE_TO_CREATE(Action_PARAMETER_PREFIX + "me");
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_ID(Action_PARAMETER_PREFIX + "re");
		const string ScenarioSaveAction::PARAMETER_LEVEL(Action_PARAMETER_PREFIX + "le");

		ParametersMap ScenarioSaveAction::getParametersMap() const
		{
			ParametersMap map;
			if (_scenario.get()) map.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			return map;
		}

		void ScenarioSaveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Update
				if(map.isDefined(PARAMETER_SCENARIO_ID))
				{
					setScenarioId(map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID));
				}
				else
				{	// Creation
					_creation = true;
					if(map.getDefault<bool>(PARAMETER_CREATE_TEMPLATE,false))
					{
						if(map.getOptional<RegistryKeyType>(PARAMETER_TEMPLATE))
						{
							try
							{
								_template = ScenarioTemplateInheritedTableSync::Get(
									map.get<RegistryKeyType>(PARAMETER_TEMPLATE),
									*_env
								);
								_tscenario.reset(
									new ScenarioTemplate(*_template, map.get<string>(PARAMETER_NAME))
								);
							}
							catch(...)
							{
								throw ActionException("specified scenario template not found");
							}
						}
						else
						{	// New template
							shared_ptr<ScenarioFolder> folder;
							if(map.isDefined(PARAMETER_FOLDER_ID))
							{
								try
								{
									folder = ScenarioFolderTableSync::GetEditable(
										map.get<RegistryKeyType>(PARAMETER_FOLDER_ID),
										*_env
									);
								}
								catch (...)
								{
									throw ActionException("Bad folder ID");
								}
							}
							_tscenario.reset(
								new ScenarioTemplate(map.get<string>(PARAMETER_NAME), folder.get())
							);
						}

						// Name control
						if(_tscenario->getName().empty())
						{
							throw ActionException("Le scénario doit avoir un nom.");
						}
						Env env;
						ScenarioTemplateInheritedTableSync::Search(env, _tscenario->getFolder() ? _tscenario->getFolder()->getKey() : 0, _tscenario->getName(), NULL, 0, 1);
						if (!env.getRegistry<ScenarioTemplate>().empty())
						{
							throw ActionException("Un scénario de même nom existe déjà");
						}

						_scenario = static_pointer_cast<Scenario,ScenarioTemplate>(_tscenario);
					}
					else
					{	// Sent scenario creation

						// Copy an other sent scenario
						if(map.getDefault<RegistryKeyType>(PARAMETER_MESSAGE_TO_COPY, 0))
						{
							try
							{
								_source = SentScenarioInheritedTableSync::Get(
									map.get<RegistryKeyType>(PARAMETER_MESSAGE_TO_COPY),
									*_env,
									UP_LINKS_LOAD_LEVEL
								);
								_sscenario.reset(
									new SentScenario(*_source)
								);
							}
							catch(ObjectNotFoundException<SentScenario>& e)
							{
								throw ActionException("scenario to copy", e, *this);
							}
						}
						else if(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE, 0))
						{
							// Copy of a template
							try
							{
								_template = ScenarioTemplateInheritedTableSync::Get(
									map.get<RegistryKeyType>(PARAMETER_TEMPLATE),
									*_env,
									UP_LINKS_LOAD_LEVEL
								);
								_sscenario.reset(new SentScenario(*_template));
							}
							catch(ObjectNotFoundException<ScenarioTemplate>& e)
							{
								throw ActionException("scenario template", e, *this);
							}
						}
						else
						{	// Blank scenario
							_sscenario.reset(new SentScenario);

							if(	map.isDefined(PARAMETER_MESSAGE_TO_CREATE) &&
								map.isDefined(PARAMETER_RECIPIENT_ID)
							){
								_messageToCreate = map.get<string>(PARAMETER_MESSAGE_TO_CREATE);
								_recipientId = map.get<RegistryKeyType>(PARAMETER_RECIPIENT_ID);
								_level = static_cast<AlarmLevel>(map.getDefault<int>(PARAMETER_LEVEL, static_cast<int>(ALARM_LEVEL_WARNING)));
							}
						}
					}
					_scenario = static_pointer_cast<Scenario, SentScenario>(_sscenario);
				}

				// Properties

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



		ScenarioSaveAction::ScenarioSaveAction()
			: FactorableTemplate<Action, ScenarioSaveAction>(),
			_creation(false)
		{}



		void ScenarioSaveAction::run(Request& request)
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

			// Mesages
			if(_creation)
			{
				if(_sscenario.get())
				{
					if(_source.get())
					{
						SentScenarioInheritedTableSync::CopyMessagesFromTemplate(
							_source->getTemplate() ? _source->getTemplate()->getKey() : _source->getKey(),
							*_sscenario
						);
					}
					else if(_template.get())
					{
						// The action on the alarms
						SentScenarioInheritedTableSync::CopyMessagesFromTemplate(
							_template->getKey(),
							*_sscenario
						);
					}
					else if(_messageToCreate && _recipientId && _level)
					{
						SentAlarm message;
						message.setScenario(_scenario.get());
						message.setLongMessage(*_messageToCreate);
						message.setShortMessage("Unique message");
						message.setLevel(*_level);
						message.setTemplate(NULL);

						AlarmTableSync::Save(&message);

						AlarmObjectLink link;
						link.setAlarm(&message);
						link.setObjectId(*_recipientId);

						AlarmObjectLinkTableSync::Save(&link);
					}
				}
				else
				{
					ScenarioTemplateInheritedTableSync::CopyMessagesFromOther(
						_template->getKey(),
						*_tscenario
					);
				}
			}

			if(_creation && request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_scenario->getKey());
			}

			// Log
			if(_creation)
			{
				if(_sscenario.get())
				{
					if(_source.get())
					{
						// The log
						MessagesLog::AddNewSentScenarioEntry(
							*_source, *_sscenario, request.getUser().get()
						);
						if(_source->getTemplate())
						{
							MessagesLibraryLog::AddTemplateInstanciationEntry(
								*_sscenario, request.getUser().get()
							);
						}
					}
					else if (_template.get())
					{
						MessagesLog::AddNewSentScenarioEntry(
							*_template, *_sscenario, request.getUser().get()
						);
						MessagesLibraryLog::AddTemplateInstanciationEntry(
							*_sscenario, request.getUser().get()
						);
					}
					else
					{
						MessagesLog::AddNewSentScenarioEntry(
							*_sscenario, *request.getUser().get()
						);
					}
				}
				else
				{
					if(_template.get())
					{
						MessagesLibraryLog::addCreateEntry(
							*_tscenario, *_template, request.getUser().get()
						);
					}
					else
					{
						MessagesLibraryLog::addCreateEntry(
							*_tscenario, request.getUser().get()
						);
					}
				}
			}



			if(_sscenario.get())
			{
				MessagesLog::addUpdateEntry(_sscenario.get(), text.str(), request.getUser().get());
			}
			else
			{
				MessagesLibraryLog::addUpdateEntry(_tscenario.get(), text.str(), request.getUser().get());
			}
		}



		bool ScenarioSaveAction::isAuthorized(const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
		}



		void ScenarioSaveAction::setScenarioId(
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
