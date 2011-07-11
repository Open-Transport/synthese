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
#include "DataSourceTableSync.h"
#include "ImportableTableSync.hpp"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "AlarmRecipient.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::algorithm;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace dblog;
	using namespace impex;
	
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
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_DATASOURCE_ID(Action_PARAMETER_PREFIX + "rs");
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_TYPE(Action_PARAMETER_PREFIX + "rt");
		const string ScenarioSaveAction::PARAMETER_SCENARIO_DATASOURCE_ID(Action_PARAMETER_PREFIX + "is");

		ParametersMap ScenarioSaveAction::getParametersMap() const
		{
			ParametersMap map;
			if (_scenario.get()) map.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			return map;
		}

		void ScenarioSaveAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Update or creation
			try
			{
				// Data source
				if(map.getDefault<RegistryKeyType>(PARAMETER_SCENARIO_DATASOURCE_ID, 0))
				{
					_scenarioDataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_SCENARIO_DATASOURCE_ID), *_env);
					if(map.isDefined(PARAMETER_SCENARIO_ID))
					{
						_dataSourceLinkId = map.get<string>(PARAMETER_SCENARIO_ID);

						ImportableTableSync::ObjectBySource<SentScenarioInheritedTableSync> scenarios(*_scenarioDataSource, *_env);
						ImportableTableSync::ObjectBySource<SentScenarioInheritedTableSync>::Set scenarioSet(scenarios.get(_dataSourceLinkId));
						if(!scenarioSet.empty())
						{
							_scenario = _env->getEditableSPtr(*scenarioSet.begin());
							if(dynamic_cast<SentScenario*>(_scenario.get()))
							{
								_sscenario = static_pointer_cast<SentScenario, Scenario>(_scenario);
							}
							if(dynamic_cast<ScenarioTemplate*>(_scenario.get()))
							{
								_tscenario = static_pointer_cast<ScenarioTemplate, Scenario>(_scenario);
							}
						}
					}
				}
				else if(map.isDefined(PARAMETER_SCENARIO_ID))
				{
					setScenarioId(map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID));
				}

				if(!_scenario.get())
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
						}
					}
					_scenario = static_pointer_cast<Scenario, SentScenario>(_sscenario);
				}


				// Messages
				if(	map.isDefined(PARAMETER_MESSAGE_TO_CREATE)
				){
					_messageToCreate = map.get<string>(PARAMETER_MESSAGE_TO_CREATE);
					_level = static_cast<AlarmLevel>(map.getDefault<int>(PARAMETER_LEVEL, static_cast<int>(ALARM_LEVEL_WARNING)));

					if(!map.getDefault<string>(PARAMETER_RECIPIENT_ID).empty())
					{
						// Recipient data source
						if(map.getDefault<RegistryKeyType>(PARAMETER_RECIPIENT_DATASOURCE_ID, 0))
						{
							_recipientDataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_RECIPIENT_DATASOURCE_ID), *_env);
						}

						// Recipient type
						_recipientType = map.get<string>(PARAMETER_RECIPIENT_TYPE);
						shared_ptr<AlarmRecipient> recipientType(Factory<AlarmRecipient>::create(_recipientType));

						vector<string> recipients;
						string recipientStr(map.get<string>(PARAMETER_RECIPIENT_ID));
						boost::algorithm::split(recipients, recipientStr, is_any_of(","));
						_recipients = vector<RegistryKeyType>();
						BOOST_FOREACH(const string& recipient, recipients)
						{
							if(_recipientDataSource.get())
							{
								_recipients->push_back(
									recipientType->getObjectIdBySource(
										*_recipientDataSource,
										recipient,
										*_env
								)	);
							}
							else
							{
								_recipients->push_back(lexical_cast<RegistryKeyType>(recipient));
							}
						}
					}
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
					// Load of existing messages
					ScenarioSentAlarmInheritedTableSync::SearchResult alarms(
						ScenarioSentAlarmInheritedTableSync::Search(
							*_env,
							_sscenario->getKey(),
							0,
							2
					)	);
					if(alarms.size() == 1)
					{
						_message = *alarms.begin();
					}

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
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw ActionException("No such data source");
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

			// Data source link
			if( _sscenario.get() && _scenarioDataSource.get() && !_dataSourceLinkId.empty())
			{
				Importable::DataSourceLinks links;
				links.insert(Importable::DataSourceLinks::value_type(_scenarioDataSource.get(), _dataSourceLinkId));
				_sscenario->setDataSourceLinks(links);
			}

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

			// Messages
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
				}
				else
				{
					ScenarioTemplateInheritedTableSync::CopyMessagesFromOther(
						_template->getKey(),
						*_tscenario
					);
				}
			}
			if(_messageToCreate && _level && _sscenario.get())
			{
				shared_ptr<SentAlarm> message;

				ScenarioSentAlarmInheritedTableSync::SearchResult msgs(
					ScenarioSentAlarmInheritedTableSync::Search(
						*_env, _sscenario->getKey()
				)	);
				if(msgs.size() == 1 || _creation)
				{
					if(msgs.size() == 1)
					{
						message = msgs.front();
						if(_recipients)
						{
							AlarmObjectLinkTableSync::Remove(message->getKey());
						}
					}
					else
					{
						message.reset(new SentAlarm);
						message->setScenario(_scenario.get());
						message->setTemplate(NULL);
						message->setShortMessage("Unique message");
					}

					message->setLongMessage(*_messageToCreate);
					message->setLevel(*_level);

					AlarmTableSync::Save(message.get());

					if(_recipients)
					{
						BOOST_FOREACH(RegistryKeyType recipient, *_recipients)
						{
							AlarmObjectLink link;
							link.setAlarm(message.get());
							link.setObjectId(recipient);
							AlarmObjectLinkTableSync::Save(&link);
						}
					}
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

			if(_message.get() && _messageToCreate)
			{
				_message->setLongMessage(*_messageToCreate);
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
