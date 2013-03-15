////////////////////////////////////////////////////////////////////////////////
/// ScenarioSaveAction class implementation.
///	@file ScenarioSaveAction.cpp
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

#include "ScenarioSaveAction.h"

#include "IConv.hpp"
#include "MessageAlternativeTableSync.hpp"
#include "MessageType.hpp"
#include "MessagesModule.h"
#include "ScenarioTableSync.h"
#include "ScenarioTemplate.h"
#include "MessagesLog.h"
#include "MessagesLibraryLog.h"
#include "MessagesRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DBLogModule.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "MessagesTypes.h"
#include "DataSourceTableSync.h"
#include "ImportableTableSync.hpp"
#include "AlarmRecipient.h"
#include "ImportableAdmin.hpp"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
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
		const string ScenarioSaveAction::PARAMETER_CREATE_TEMPLATE = Action_PARAMETER_PREFIX + "t";
		const string ScenarioSaveAction::PARAMETER_VARIABLE = Action_PARAMETER_PREFIX + "var";
		const string ScenarioSaveAction::PARAMETER_ENABLED = Action_PARAMETER_PREFIX + "ena";
		const string ScenarioSaveAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sda";
		const string ScenarioSaveAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "eda";
		const string ScenarioSaveAction::PARAMETER_START_TIME = Action_PARAMETER_PREFIX + "_start_time";
		const string ScenarioSaveAction::PARAMETER_END_TIME = Action_PARAMETER_PREFIX + "_end_time";
		const string ScenarioSaveAction::PARAMETER_EVENT_START_DATE = Action_PARAMETER_PREFIX + "_event_start_date";
		const string ScenarioSaveAction::PARAMETER_EVENT_END_DATE = Action_PARAMETER_PREFIX + "_event_end_date";
		const string ScenarioSaveAction::PARAMETER_EVENT_START_TIME = Action_PARAMETER_PREFIX + "_event_start_time";
		const string ScenarioSaveAction::PARAMETER_EVENT_END_TIME = Action_PARAMETER_PREFIX + "_event_end_time";
		const string ScenarioSaveAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "sid";
		const string ScenarioSaveAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "nam";
		const string ScenarioSaveAction::PARAMETER_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";
		const string ScenarioSaveAction::PARAMETER_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_TO_COPY = Action_PARAMETER_PREFIX + "mt";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_TO_CREATE = Action_PARAMETER_PREFIX + "me";
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_ID = Action_PARAMETER_PREFIX + "re";
		const string ScenarioSaveAction::PARAMETER_LEVEL = Action_PARAMETER_PREFIX + "le";
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_DATASOURCE_ID = Action_PARAMETER_PREFIX + "rs";
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_TYPE = Action_PARAMETER_PREFIX + "rt";
		const string ScenarioSaveAction::PARAMETER_SCENARIO_DATASOURCE_ID = Action_PARAMETER_PREFIX + "is";
		const string ScenarioSaveAction::PARAMETER_CREATED_MESSAGE_TITLE = Action_PARAMETER_PREFIX + "_created_message_title";
		const string ScenarioSaveAction::PARAMETER_ENCODING = Action_PARAMETER_PREFIX + "_encoding";
		const string ScenarioSaveAction::PARAMETER_SECTIONS = Action_PARAMETER_PREFIX + "_sections";

		const string ScenarioSaveAction::PARAMETER_WITH_MESSAGES = Action_PARAMETER_PREFIX + "_with_messages";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_ID_ = Action_PARAMETER_PREFIX + "_message_id_";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_TITLE_ = Action_PARAMETER_PREFIX + "_message_title_";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_CONTENT_ = Action_PARAMETER_PREFIX + "_message_content_";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_LEVEL_ = Action_PARAMETER_PREFIX + "_message_level_";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_RECIPIENTS_ = Action_PARAMETER_PREFIX + "_message_recipients_";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_ALTERNATIVES_ = Action_PARAMETER_PREFIX + "_message_alternatives_";
		const string ScenarioSaveAction::VALUES_SEPARATOR = ",";
		const string ScenarioSaveAction::VALUES_PARAMETERS_SEPARATOR = "|";


		ParametersMap ScenarioSaveAction::getParametersMap() const
		{
			ParametersMap map;
			if (_scenario.get()) map.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());

			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void ScenarioSaveAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Encoding
			IConv iconv(map.getDefault<string>(PARAMETER_ENCODING, "UTF-8"), "UTF-8");

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

						ImportableTableSync::ObjectBySource<ScenarioTableSync> scenarios(*_scenarioDataSource, *_env);
						ImportableTableSync::ObjectBySource<ScenarioTableSync>::Set scenarioSet(scenarios.get(_dataSourceLinkId));
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
						if(map.getOptional<RegistryKeyType>(PARAMETER_MESSAGE_TO_COPY))
						{
							try
							{
								_source = ScenarioTableSync::GetCast<SentScenario>(
									map.get<RegistryKeyType>(PARAMETER_MESSAGE_TO_COPY),
									*_env
								);
								_tscenario.reset(
									new ScenarioTemplate(*_source, _source->getName())
								);
								_tscenario->setSections(_source->getSections());
							}
							catch(...)
							{
								throw ActionException("specified scenario not found");
							}
						}
						else if(map.getOptional<RegistryKeyType>(PARAMETER_TEMPLATE))
						{
							try
							{
								_template = ScenarioTableSync::GetCast<ScenarioTemplate>(
									map.get<RegistryKeyType>(PARAMETER_TEMPLATE),
									*_env
								);
								_tscenario.reset(
									new ScenarioTemplate(*_template, map.get<string>(PARAMETER_NAME))
								);
								_tscenario->setSections(_template->getSections());
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

						// Name check
						if(_tscenario->getName().empty())
						{
							throw ActionException("Le scénario doit avoir un nom.");
						}
						Env env;
						ScenarioTableSync::SearchResult r(
							ScenarioTableSync::SearchTemplates(
								env,
								_tscenario->getFolder() ? _tscenario->getFolder()->getKey() : 0,
								_tscenario->getName(),
								NULL,
								0,
								1
						)	);
						if (!r.empty())
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
								_source = ScenarioTableSync::GetCast<SentScenario>(
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
								_template = ScenarioTableSync::GetCast<ScenarioTemplate>(
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
						_scenario = static_pointer_cast<Scenario, SentScenario>(_sscenario);
					}
				}


				// Messages
				if(map.isDefined(PARAMETER_MESSAGE_TO_CREATE)
				){
					_messageToCreate = iconv.convert(map.get<string>(PARAMETER_MESSAGE_TO_CREATE));
					if(map.isDefined(PARAMETER_CREATED_MESSAGE_TITLE))
					{
						_messageToCreateTitle = iconv.convert(map.get<string>(PARAMETER_CREATED_MESSAGE_TITLE));
					}
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
						_recipients = vector<shared_ptr<Registrable> >();
						BOOST_FOREACH(const string& recipient, recipients)
						{
							try
							{
								if(_recipientDataSource.get())
								{
									_recipients->push_back(
										DBModule::GetEditableObject(
											recipientType->getObjectIdBySource(
												*_recipientDataSource,
												recipient,
												*_env
											),
											*_env
									)	);
								}
								else
								{
									_recipients->push_back(
										DBModule::GetEditableObject(
											lexical_cast<RegistryKeyType>(recipient),
											*_env
									)	);
								}
							}
							catch(...)
							{

							}
						}
					}
				}

				// Messages
				_withMessages = map.getDefault<bool>(PARAMETER_WITH_MESSAGES, false);
				if(_withMessages)
				{
					for(size_t rank(0); map.isDefined(PARAMETER_MESSAGE_ID_+ lexical_cast<string>(rank)); ++rank)
					{
						string rankStr(lexical_cast<string>(rank));

						// Jump over removals (id is empty)
						if(map.get<string>(PARAMETER_MESSAGE_ID_+ rankStr).empty())
						{
							continue;
						}

						Message msg;
						msg.id = map.get<RegistryKeyType>(PARAMETER_MESSAGE_ID_+ rankStr); // explicit 0 value in case of new message
						msg.title = map.getDefault<string>(PARAMETER_MESSAGE_TITLE_+ rankStr);
						msg.content = map.getDefault<string>(PARAMETER_MESSAGE_CONTENT_+ rankStr);
						msg.level = static_cast<AlarmLevel>(map.getDefault<int>(PARAMETER_MESSAGE_LEVEL_+rankStr, 10));

						// Recipients
						BOOST_FOREACH(const Factory<AlarmRecipient>::Keys::value_type& key, Factory<AlarmRecipient>::GetKeys())
						{
							string keys(map.getDefault<string>(PARAMETER_MESSAGE_RECIPIENTS_+ lexical_cast<string>(rank) + "_" + key));
							if(!keys.empty())
							{
								vector<string> keysStrVector;
								split(keysStrVector, keys, is_any_of(VALUES_SEPARATOR));
								BOOST_FOREACH(const string& keyStr, keysStrVector)
								{
									vector<string> parts;
									split(parts, keyStr, is_any_of(VALUES_PARAMETERS_SEPARATOR));
									string parameter;
									if(parts.size() > 1)
									{
										parameter = parts[1];
									}
									try
									{
										msg.recipients.push_back(
											make_pair(
												key,
												make_pair(
													DBModule::GetEditableObject(
														lexical_cast<RegistryKeyType>(keyStr),
														*_env
													).get(),
													parameter
										)	)	);
									}
									catch(...)
									{									
									}
								}
							}
						}

						// Alternatives
						BOOST_FOREACH(const MessageType::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<MessageType>())
						{
							msg.alternatives.insert(
								make_pair(
									it.second.get(),
									map.getDefault<string>(PARAMETER_MESSAGE_ALTERNATIVES_+ lexical_cast<string>(rank) + "_" + lexical_cast<string>(it.first))
							)	);
						}

						_messages.push_back(msg);
						if(msg.id)
						{
							_messageIds.insert(msg.id);
						}
				}	}


				// Properties

				// Name
				if(map.isDefined(PARAMETER_NAME))
				{
					_name = iconv.convert(map.get<string>(PARAMETER_NAME));
				}

				// Sections
				if(map.isDefined(PARAMETER_SECTIONS))
				{
					_sections = Scenario::Sections();
					string sectionsStr(map.get<string>(PARAMETER_SECTIONS));
					trim(sectionsStr);
					if(!sectionsStr.empty())
					{
						vector<string> tokens;
						split(tokens, sectionsStr, is_any_of(","));
						BOOST_FOREACH(const string& token, tokens)
						{
							try
							{
								_sections->insert(lexical_cast<int>(token));
							}
							catch (bad_lexical_cast&)
							{						
							}
						}
					}
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

						// Uniqueness check
						Env env;
						ScenarioTableSync::SearchResult r(
							ScenarioTableSync::SearchTemplates(
								env,
								folderId,
								_name ? *_name : _scenario->getName(),
								dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario).get(),
								0,
								1
						)	);
						if(	!r.empty())
						{
							throw ActionException("Le nom spécifié est déjà utilisé par un autre scénario.");
						}
					}
				}

				// Sent scenario only
				if(_sscenario.get())
				{
					// Load of existing messages
					AlarmTableSync::SearchResult alarms(
						AlarmTableSync::Search(
							*_env,
							_sscenario->getKey(),
							0,
							2
					)	);
					if(alarms.size() == 1)
					{
						_message = static_pointer_cast<SentAlarm, Alarm>(*alarms.begin());
					}

					// Importable
					_setImportableUpdateFromParametersMap(*_env, map);

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
							if(map.isDefined(PARAMETER_START_TIME))
							{
								time_duration t(0,0,0);
								if(!map.get<string>(PARAMETER_START_TIME).empty())
								{
									t = duration_from_string(map.get<string>(PARAMETER_START_TIME));
								}
								_startDate = ptime(
									date_from_iso_string(map.get<string>(PARAMETER_START_DATE)),
									t
								);
							}
							else
							{
								_startDate = time_from_string(map.get<string>(PARAMETER_START_DATE));
							}
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
							if(map.isDefined(PARAMETER_END_TIME))
							{
								time_duration t(23,59,59);
								if(!map.get<string>(PARAMETER_END_TIME).empty())
								{
									t = duration_from_string(map.get<string>(PARAMETER_END_TIME));
								}
								_endDate = ptime(
									date_from_iso_string(map.get<string>(PARAMETER_END_DATE)),
									t
								);
							}
							else
							{
								_endDate = time_from_string(map.get<string>(PARAMETER_END_DATE));
							}
							*_endDate -= seconds(_endDate->time_of_day().seconds());
						}
					}

					// Event Start date
					if(map.isDefined(PARAMETER_EVENT_START_DATE))
					{
						if(map.get<string>(PARAMETER_EVENT_START_DATE).empty())
						{
							_eventStartDate = ptime();
						}
						else
						{
							if(map.isDefined(PARAMETER_EVENT_START_TIME))
							{
								time_duration t(0,0,0);
								if(!map.get<string>(PARAMETER_EVENT_START_TIME).empty())
								{
									t = duration_from_string(map.get<string>(PARAMETER_EVENT_START_TIME));
								}
								_eventStartDate = ptime(
									date_from_iso_string(map.get<string>(PARAMETER_EVENT_START_DATE)),
									t
								);
							}
							else
							{
								_eventStartDate = time_from_string(map.get<string>(PARAMETER_EVENT_START_DATE));
							}
						}
					}

					// Event End date
					if(map.isDefined(PARAMETER_EVENT_END_DATE))
					{
						if(map.get<string>(PARAMETER_EVENT_END_DATE).empty())
						{
							_eventEndDate = ptime();
						}
						else
						{
							if(map.isDefined(PARAMETER_EVENT_END_TIME))
							{
								time_duration t(23,59,59);
								if(!map.get<string>(PARAMETER_EVENT_END_TIME).empty())
								{
									t = duration_from_string(map.get<string>(PARAMETER_EVENT_END_TIME));
								}
								_eventEndDate = ptime(
									date_from_iso_string(map.get<string>(PARAMETER_EVENT_END_DATE)),
									t
								);
							}
							else
							{
								_eventEndDate = time_from_string(map.get<string>(PARAMETER_EVENT_END_DATE));
							}
							*_eventEndDate -= seconds(_endDate->time_of_day().seconds());
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
									(	(_enabled && *_enabled) ||
										(!_enabled && _sscenario->getIsEnabled())
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
								iconv.convert(map.get<string>(PARAMETER_VARIABLE + variable.second.code))
							);
							if(	variable.second.compulsory &&
								(	(_enabled && *_enabled) ||
									(!_enabled && _sscenario->getIsEnabled())
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



		ScenarioSaveAction::ScenarioSaveAction():
			FactorableTemplate<Action, ScenarioSaveAction>(),
			_withMessages(false),
			_creation(false)
		{}



		void ScenarioSaveAction::run(Request& request)
		{
			DBTransaction transaction;

			// Log message
			stringstream text;

			// Data source link
			if( _sscenario.get() && _scenarioDataSource.get() && !_dataSourceLinkId.empty())
			{
				Importable::DataSourceLinks links;
				links.insert(Importable::DataSourceLinks::value_type(_scenarioDataSource.get(), _dataSourceLinkId));
				_sscenario->setDataSourceLinksWithoutRegistration(links);
			}

			// Name
			if(_name)
			{
				DBLogModule::appendToLogIfChange(text, "Nom", _scenario->getName(), *_name);
				_scenario->setName(*_name);
			}

			// Sections
			if(_sections)
			{
				_scenario->setSections(*_sections);
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

				// Start date
				if(_eventStartDate)
				{
					DBLogModule::appendToLogIfChange(
						text,
						"Date de début événement",
						to_simple_string(_sscenario->getEventStart()),
						to_simple_string(*_eventStartDate)
					);
					_sscenario->setEventStart(*_eventStartDate);
				}

				// End date
				if(_eventEndDate)
				{
					DBLogModule::appendToLogIfChange(
						text,
						"Date de fin",
						to_simple_string(_sscenario->getEventEnd()),
						to_simple_string(*_eventEndDate)
					);
					_sscenario->setEventEnd(*_eventEndDate);
				}

				// Datasource
				_doImportableUpdate(*_sscenario, request);

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
					if(!values.empty())
					{
						ScenarioTableSync::WriteVariablesIntoMessages(*_sscenario);
					}
				}

			}

			// Save
			ScenarioTableSync::Save(_scenario.get(), transaction);

			// Messages
			if(_creation)
			{
				if(_source.get())
				{
					ScenarioTableSync::CopyMessages(
						_source->getTemplate() ? _source->getTemplate()->getKey() : _source->getKey(),
						*_scenario,
						transaction
					);
				}
				else if(_template.get())
				{
					// The action on the alarms
					ScenarioTableSync::CopyMessages(
						_template.get() ? _template->getKey() : _source->getKey(),
						*_scenario,
						transaction
					);
				}
			}
			if(_messageToCreate && _level && _sscenario.get())
			{
				shared_ptr<SentAlarm> message;

				AlarmTableSync::SearchResult msgs(
					AlarmTableSync::Search(
						*_env, _sscenario->getKey()
				)	);
				if(msgs.size() == 1)
				{
					message = static_pointer_cast<SentAlarm,Alarm>(msgs.front());
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
				}

				message->setShortMessage(_messageToCreateTitle ? *_messageToCreateTitle : "Unique message");
				message->setLongMessage(*_messageToCreate);
				message->setLevel(*_level);

				AlarmTableSync::Save(message.get(), transaction);

				if(_recipients)
				{
					BOOST_FOREACH(const shared_ptr<Registrable> recipient, *_recipients)
					{
						AlarmObjectLink link;
						link.setRecipientKey(_recipientType);
						link.setAlarm(message.get());
						link.setObject(recipient.get());
						AlarmObjectLinkTableSync::Save(&link, transaction);
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

			if(_message.get())
			{
				AlarmTableSync::Save(_message.get(), transaction);
			}

			if(_withMessages)
			{
				// Removals
				BOOST_FOREACH(const Alarm* alarm, _scenario->getMessages())
				{
					if(_messageIds.find(alarm->getKey()) == _messageIds.end())
					{
						AlarmTableSync::Remove(request.getSession(), alarm->getKey(), transaction, false);
					}
				}

				// Insertions / updates
				BOOST_FOREACH(const Messages::value_type& msg, _messages)
				{
					Env env;
					shared_ptr<Alarm> alarm;

					if(_sscenario.get())
					{
						if(msg.id)
						{
							alarm = AlarmTableSync::GetCastEditable<SentAlarm>(msg.id, env);
						}
						else
						{
							alarm.reset(new SentAlarm);
						}
					}
					else
					{
						if(msg.id)
						{
							alarm = AlarmTableSync::GetCastEditable<AlarmTemplate>(msg.id, env);
						}
						else
						{
							alarm.reset(new AlarmTemplate);
						}
					}
					alarm->setScenario(_scenario.get());
					alarm->setShortMessage(msg.title);
					alarm->setLongMessage(msg.content);
					alarm->setLevel(msg.level);
					AlarmTableSync::Save(alarm.get(), transaction);

					AlarmObjectLinkTableSync::SearchResult v(
						AlarmObjectLinkTableSync::Search(env, alarm->getKey())
					);
					set<RegistryKeyType> toRemove;
					typedef map<Message::Recipients::value_type, RegistryKeyType> AOLMap;
					AOLMap m;
					BOOST_FOREACH(const shared_ptr<AlarmObjectLink>& item, v)
					{
						toRemove.insert(item->getKey());
						m.insert(
							make_pair(
								make_pair(
									item->getRecipientKey(),
									make_pair(
										item->getObject(),
										item->getParameter()
								)	),
								item->getKey()
						)	);
					}

					// Insertions / updates
					BOOST_FOREACH(const Message::Recipients::value_type& it, msg.recipients)
					{
						// Insertion
						AOLMap::const_iterator itAOLMap(m.find(it));
						if(itAOLMap == m.end())
						{
							AlarmObjectLink aol;
							aol.setRecipientKey(it.first);
							aol.setObject(it.second.first);
							aol.setParameter(it.second.second);
							aol.setAlarm(alarm.get());
							AlarmObjectLinkTableSync::Save(&aol, transaction);
						}
						else
						{
							toRemove.erase(itAOLMap->second);
						}
					}

					// Removals
					BOOST_FOREACH(RegistryKeyType id, toRemove)
					{
						DBTableSyncTemplate<AlarmObjectLinkTableSync>::Remove(request.getSession(), id, transaction, false);
					}


					// Alternatives
					BOOST_FOREACH(const Message::Alternatives::value_type& it, msg.alternatives)
					{
						shared_ptr<MessageAlternative> alternative;
						bool toSave(false);
						MessageAlternativeTableSync::SearchResult vec(
							MessageAlternativeTableSync::Search(env, alarm->getKey(), it.first->getKey())
						);
						if(vec.empty())
						{
							alternative.reset(new MessageAlternative);
							toSave = true;
							alternative->set<Alarm>(*alarm);
							alternative->set<MessageType>(*it.first);
						}
						else
						{
							alternative = *vec.begin();
							toSave = (alternative->get<Content>() != it.second);
						}
						alternative->set<Content>(it.second);

						if(toSave)
						{
							MessageAlternativeTableSync::Save(alternative.get(), transaction);
						}
					}

				}

			}

			transaction.run();

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
}	}
