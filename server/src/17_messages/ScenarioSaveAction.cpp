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
#include "MessageApplicationPeriodTableSync.hpp"
#include "MessageTypeTableSync.hpp"
#include "MessagesModule.h"
#include "MessagesSectionTableSync.hpp"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "ScenarioCalendarTableSync.hpp"
#include "SentScenarioTableSync.h"
#include "ScenarioTemplateTableSync.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
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
#include "BroadcastPoint.hpp"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::algorithm;
using namespace boost::property_tree;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace dblog;
	using namespace impex;
	using namespace messages;

	template<> const string util::FactorableTemplate<Action,messages::ScenarioSaveAction>::FACTORY_KEY("scenario_save");

	namespace messages
	{
		// Parameters for action determination
		const string ScenarioSaveAction::PARAMETER_CREATE_TEMPLATE = Action_PARAMETER_PREFIX + "t";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_TO_COPY = Action_PARAMETER_PREFIX + "mt";
		const string ScenarioSaveAction::PARAMETER_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";
		const string ScenarioSaveAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "sid";
		const string ScenarioSaveAction::PARAMETER_SCENARIO_DATASOURCE_ID = Action_PARAMETER_PREFIX + "is";

		// Parameters for properties
		const string ScenarioSaveAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "nam";
		const string ScenarioSaveAction::PARAMETER_VARIABLE = Action_PARAMETER_PREFIX + "var";
		const string ScenarioSaveAction::PARAMETER_ENABLED = Action_PARAMETER_PREFIX + "ena";
		const string ScenarioSaveAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sda";
		const string ScenarioSaveAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "eda";
		const string ScenarioSaveAction::PARAMETER_START_TIME = Action_PARAMETER_PREFIX + "_start_time";
		const string ScenarioSaveAction::PARAMETER_END_TIME = Action_PARAMETER_PREFIX + "_end_time";
		const string ScenarioSaveAction::PARAMETER_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";
		const string ScenarioSaveAction::PARAMETER_SECTIONS = Action_PARAMETER_PREFIX + "_field_sections";
		const string ScenarioSaveAction::PARAMETER_ARCHIVED = Action_PARAMETER_PREFIX + "_field_archived";

		// Parameters for unused properties
		const string ScenarioSaveAction::PARAMETER_EVENT_START_DATE = Action_PARAMETER_PREFIX + "_event_start_date";
		const string ScenarioSaveAction::PARAMETER_EVENT_END_DATE = Action_PARAMETER_PREFIX + "_event_end_date";
		const string ScenarioSaveAction::PARAMETER_EVENT_START_TIME = Action_PARAMETER_PREFIX + "_event_start_time";
		const string ScenarioSaveAction::PARAMETER_EVENT_END_TIME = Action_PARAMETER_PREFIX + "_event_end_time";

		// Parameters for A/B/G/H message update full method
		const string ScenarioSaveAction::PARAMETER_JSON = Action_PARAMETER_PREFIX + "_json";

		// Parameters for A/B/G/H message update simplified method
		const string ScenarioSaveAction::PARAMETER_CREATED_MESSAGE_TITLE = Action_PARAMETER_PREFIX + "_created_message_title";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_TO_CREATE = Action_PARAMETER_PREFIX + "me";
		const string ScenarioSaveAction::PARAMETER_ENCODING = Action_PARAMETER_PREFIX + "_encoding";
		const string ScenarioSaveAction::PARAMETER_LEVEL = Action_PARAMETER_PREFIX + "le";
		const string ScenarioSaveAction::PARAMETER_TAGS = Action_PARAMETER_PREFIX + "tags";

		const string ScenarioSaveAction::PARAMETER_DISPLAY_DURATION = Action_PARAMETER_PREFIX + "ddur";
		const string ScenarioSaveAction::PARAMETER_DIGITIZED_VERSION = Action_PARAMETER_PREFIX + "dv";
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_ID = Action_PARAMETER_PREFIX + "re";
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_DATASOURCE_ID = Action_PARAMETER_PREFIX + "rs";
		const string ScenarioSaveAction::PARAMETER_RECIPIENT_TYPE = Action_PARAMETER_PREFIX + "rt";
		const string ScenarioSaveAction::PARAMETER_RECIPIENTS_ = Action_PARAMETER_PREFIX + "_recipients_";
		const string ScenarioSaveAction::PARAMETER_MESSAGE_SECTION = Action_PARAMETER_PREFIX + "msection";
		
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
			//////////////////////////////////////////////////////////////////////////
			// Action determination

			// Update or a scenario identified by data source
			// (data source id is memorized in case of creation)
			if(	map.getDefault<RegistryKeyType>(PARAMETER_SCENARIO_DATASOURCE_ID, 0) &&
				!map.getDefault<string>(PARAMETER_SCENARIO_ID).empty()
			){
				// Data source
				try
				{
					_scenarioDataSource = DataSourceTableSync::Get(
						map.get<RegistryKeyType>(PARAMETER_SCENARIO_DATASOURCE_ID),
						*_env
					);
				}
				catch(ObjectNotFoundException<DataSource>& e)
				{
					throw ActionException("No such data source for scenario id : "+ e.getMessage());
				}

				// Id
				_dataSourceLinkId = map.get<string>(PARAMETER_SCENARIO_ID);

				ImportableTableSync::ObjectBySource<ScenarioTemplateTableSync> scenarios(*_scenarioDataSource, *_env);
				ImportableTableSync::ObjectBySource<ScenarioTemplateTableSync>::Set scenarioSet(scenarios.get(_dataSourceLinkId));
				if(!scenarioSet.empty())
				{
					_scenario = _env->getEditableSPtr(*scenarioSet.begin());
					_tscenario = static_pointer_cast<ScenarioTemplate, Scenario>(_scenario);
					_sscenario.reset();
				}
				else
				{
					ImportableTableSync::ObjectBySource<SentScenarioTableSync> scenarios(*_scenarioDataSource, *_env);
					ImportableTableSync::ObjectBySource<SentScenarioTableSync>::Set scenarioSet(scenarios.get(_dataSourceLinkId));
					if(!scenarioSet.empty())
					{
						_scenario = _env->getEditableSPtr(*scenarioSet.begin());
						_sscenario = static_pointer_cast<SentScenario, Scenario>(_scenario);
						_tscenario.reset();
					}
				}
				
			}
			// Update, identified by id (actions G/H)
			else if(map.isDefined(PARAMETER_SCENARIO_ID))
			{
				setScenarioId(map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID));
			}

			// If not update, then creation
			if(!_scenario.get())
			{
				_creation = true;
				
				// Template creation
				if(map.getDefault<bool>(PARAMETER_CREATE_TEMPLATE,false))
				{
					// By copying an event (action E)
					if(map.getOptional<RegistryKeyType>(PARAMETER_MESSAGE_TO_COPY))
					{
						try
						{
							_source = SentScenarioTableSync::GetCast<SentScenario>(
								map.get<RegistryKeyType>(PARAMETER_MESSAGE_TO_COPY),
								*_env
							);
							ScenarioTemplate* scenarioTemplate(new ScenarioTemplate);
							scenarioTemplate->set<Name>(_source->get<Name>());
							
							_tscenario.reset(scenarioTemplate);
							_tscenario->set<Sections>(_source->get<Sections>());
						}
						catch(...)
						{
							throw ActionException("specified scenario not found");
						}
					}
					// By copying an other template (action F)
					else if(map.getOptional<RegistryKeyType>(PARAMETER_TEMPLATE))
					{
						try
						{
							_template = ScenarioTemplateTableSync::GetCast<ScenarioTemplate>(
								map.get<RegistryKeyType>(PARAMETER_TEMPLATE),
								*_env
							);
							ScenarioTemplate* scenarioTemplate(new ScenarioTemplate);
							scenarioTemplate->set<Name>(map.get<string>(PARAMETER_NAME));
							scenarioTemplate->setFolder(_template->getFolder());
							
							_tscenario.reset(scenarioTemplate);
						}
						catch(...)
						{
							throw ActionException("specified scenario template not found");
						}
					}
					else // From scratch (action B)
					{
						boost::shared_ptr<ScenarioFolder> folder;
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
						ScenarioTemplate* scenarioTemplate(new ScenarioTemplate);
						scenarioTemplate->set<Name>(map.get<string>(PARAMETER_NAME));
						scenarioTemplate->setFolder(folder.get());
						_tscenario.reset(scenarioTemplate);
					}

					// Name check
					if(_tscenario->getName().empty())
					{
						throw ActionException("Le scénario doit avoir un nom.");
					}
			
					_scenario = static_pointer_cast<Scenario,ScenarioTemplate>(_tscenario);
				}
				else
				{
					// Sent scenario creation
					// Copy an other sent scenario (action E)
					if(map.getDefault<RegistryKeyType>(PARAMETER_MESSAGE_TO_COPY, 0))
					{
						try
						{
							_source = SentScenarioTableSync::GetCast<SentScenario>(
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
						// Copy of a template (action D)
						try
						{
							_template = ScenarioTemplateTableSync::GetCast<ScenarioTemplate>(
								map.get<RegistryKeyType>(PARAMETER_TEMPLATE),
								*_env,
								UP_LINKS_LOAD_LEVEL
							);
							SentScenario* sentScenario(new SentScenario);
							sentScenario->set<Name>(_template->get<Name>());
							sentScenario->setTemplate(_template.get());
							_sscenario.reset(sentScenario);
						}
						catch(ObjectNotFoundException<ScenarioTemplate>& e)
						{
							throw ActionException("scenario template", e, *this);
						}
					}
					else
					{	// Creation from scratch (action A)
						_sscenario.reset(new SentScenario);
					}
					_scenario = static_pointer_cast<Scenario, SentScenario>(_sscenario);
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// Load of attached objects
			if(_scenario->getKey())
			{
				ScenarioCalendarTableSync::SearchResult calendars(ScenarioCalendarTableSync::Search(*_env, _scenario->getKey()));
				
				Alarms messages;
				AlarmTableSync::Search(*_env, std::back_inserter(messages), _scenario->getKey());

				BOOST_FOREACH(const boost::shared_ptr<ScenarioCalendar>& calendar, calendars)
				{
					MessageApplicationPeriodTableSync::Search(*_env, calendar->getKey());
				}

				BOOST_FOREACH(const boost::shared_ptr<Alarm> message, messages)
				{
					MessageAlternativeTableSync::Search(*_env, message->getKey());
					AlarmObjectLinkTableSync::Search(*_env, message->getKey());
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// Properties

			// Encoding
			IConv iconv(map.getDefault<string>(PARAMETER_ENCODING, "UTF-8"), "UTF-8");

			// Name
			if(map.isDefined(PARAMETER_NAME))
			{
				_name = iconv.convert(map.get<string>(PARAMETER_NAME));
			}

			// Sections
			if(map.isDefined(PARAMETER_SECTIONS))
			{
				_sections = std::set<MessagesSection*>();
				string sectionsStr(map.get<string>(PARAMETER_SECTIONS));
				trim(sectionsStr);
				if(!sectionsStr.empty())
				{
					vector<string> tokens;
					split(tokens, sectionsStr, is_any_of(VALUES_SEPARATOR));
					BOOST_FOREACH(const string& token, tokens)
					{
						try
						{
							RegistryKeyType sectionId(lexical_cast<RegistryKeyType>(token));
							if(sectionId)
							{
								_sections->insert(
									Env::GetOfficialEnv().getEditable<MessagesSection>(sectionId).get()
								);
							}
						}
						catch (bad_lexical_cast&)
						{						
						}
						catch(ObjectNotFoundException<MessagesSection>&)
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
						_folder = boost::shared_ptr<ScenarioFolder>();
					}
					else
					{
						_folder = ScenarioFolderTableSync::GetEditable(folderId, *_env);
					}
				}
			}

			// Event only
			if(_sscenario.get())
			{
				// Importable
				_setImportableUpdateFromParametersMap(*_env, map);

				// Enabled
				if(map.isDefined(PARAMETER_ENABLED))
				{
					_enabled = map.get<bool>(PARAMETER_ENABLED);
				}

				// Archived
				if(map.isDefined(PARAMETER_ARCHIVED))
				{
					_archived = map.get<bool>(PARAMETER_ARCHIVED);
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
								from_string(map.get<string>(PARAMETER_START_DATE)),
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
								from_string(map.get<string>(PARAMETER_END_DATE)),
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
								from_string(map.get<string>(PARAMETER_EVENT_START_DATE)),
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
								from_string(map.get<string>(PARAMETER_EVENT_END_DATE)),
								t
							);
						}
						else
						{
							_eventEndDate = time_from_string(map.get<string>(PARAMETER_EVENT_END_DATE));
						}
						//*_eventEndDate -= seconds(_endDate->time_of_day().seconds());
					}
				}

			}

			//////////////////////////////////////////////////////////////////////////
			// Calendars and messages

			// Full method
			if(map.isDefined(PARAMETER_JSON))
			{
				_messagesAndCalendars = ptree();
				istringstream ss(map.get<string>(PARAMETER_JSON));
				read_json(ss, *_messagesAndCalendars);
			}

			else // Simplified method
			{
				// Load of existing messages
				Alarms alarms;
				AlarmTableSync::Search(*_env, std::back_inserter(alarms), _scenario->getKey(), 0, 2);

				if(alarms.size() == 1)
				{
					_message = *alarms.begin();
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
					_tags = map.getDefault<string>(PARAMETER_TAGS);
					_display_duration = static_cast<size_t>(map.getDefault<int>(PARAMETER_DISPLAY_DURATION));
					_digitizedVersion = map.getDefault<string>(PARAMETER_DIGITIZED_VERSION);
					if (map.isDefined(PARAMETER_MESSAGE_SECTION))
					{
						RegistryKeyType id = map.getDefault<RegistryKeyType>(PARAMETER_MESSAGE_SECTION);
						_messageSection = MessagesSectionTableSync::Get(id, *_env);
					}

					_recipients = Recipients::value_type();

					// New recipient lists (but cannot read data source id)
					BOOST_FOREACH(const string& key, Factory<AlarmRecipient>::GetKeys())
					{
						string value(map.getDefault<string>(PARAMETER_RECIPIENTS_ + key));
						if(!value.empty())
						{
							vector<string> recipients;
							boost::algorithm::split(recipients, value, is_any_of(","));
							BOOST_FOREACH(const string& recipient, recipients)
							{
								vector<string> recipientParams;
								boost::algorithm::split(recipientParams, recipient, is_any_of("|"));

								Recipients::value_type::value_type rec;
								try
								{
									rec.first = key;
									rec.second.first = lexical_cast<RegistryKeyType>(recipientParams[0]);
									if(recipientParams.size() > 1)
									{
										rec.second.second = recipientParams[1];
									}

									_recipients->push_back(rec);
								}
								catch(...)
								{

								}
							}
						}
					}

					if(!map.getDefault<string>(PARAMETER_RECIPIENT_ID).empty())
					{
						// Recipient data source
						if(map.getDefault<RegistryKeyType>(PARAMETER_RECIPIENT_DATASOURCE_ID, 0))
						{
							_recipientDataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_RECIPIENT_DATASOURCE_ID), *_env);
						}

						// Recipient type
						_recipientType = map.get<string>(PARAMETER_RECIPIENT_TYPE);
						boost::shared_ptr<AlarmRecipient> recipientType(Factory<AlarmRecipient>::create(_recipientType));

						vector<string> recipients;
						string recipientStr(map.get<string>(PARAMETER_RECIPIENT_ID));
						boost::algorithm::split(recipients, recipientStr, is_any_of(","));
						BOOST_FOREACH(const string& recipient, recipients)
						{
							vector<string> recipientParams;
							boost::algorithm::split(recipientParams, recipient, is_any_of("|"));
							Recipients::value_type::value_type rec;

							try
							{
								rec.first = _recipientType;
								if(!lexical_cast<RegistryKeyType>(recipientParams[0]))
								{
									rec.second.first = 0;
								}
								else if(_recipientDataSource.get())
								{
									rec.second.first =
										DBModule::GetEditableObject(
											recipientType->getObjectIdBySource(
												*_recipientDataSource,
												recipientParams[0],
												*_env
											),
											*_env
										)->getKey()
									;
								}
								else
								{
									rec.second.first = lexical_cast<RegistryKeyType>(recipientParams[0]);
								}

								if(recipientParams.size() > 1)
								{
									rec.second.second = recipientParams[1];
								}

								_recipients->push_back(rec);
							}
							catch(...)
							{

							}
						}
					}
				}
			}
		}



		ScenarioSaveAction::ScenarioSaveAction():
			FactorableTemplate<Action, ScenarioSaveAction>(),
			_creation(false)
		{}



		void ScenarioSaveAction::run(Request& request)
		{
			DBTransaction transaction;
			bool notifyBroadcastPoints = false;
			ptime now(second_clock::local_time());

			// Log message
			stringstream text;

			//////////////////////////////////////////////////////////////////////////
			// Properties

			// Data source link
			if( _sscenario.get() && _scenarioDataSource.get() && !_dataSourceLinkId.empty())
			{
				Importable::DataSourceLinks links;
				links.insert(Importable::DataSourceLinks::value_type(_scenarioDataSource.get(), _dataSourceLinkId));
				_sscenario->setDataSourceLinksWithoutRegistration(links);
			}

			if(_tscenario.get())
			{
				// Name
				if(_name)
				{
					DBLogModule::appendToLogIfChange(text, "Nom", _scenario->getName(), *_name);
					_tscenario->set<Name>(*_name);
				}

				// Sections
				if(_sections)
				{
					_tscenario->set<Sections>(*_sections);
				}
				
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
				// Name
				if(_name)
				{
					DBLogModule::appendToLogIfChange(text, "Nom", _scenario->getName(), *_name);
					_sscenario->set<Name>(*_name);
				}

				// Sections
				if(_sections)
				{
					_sscenario->set<Sections>(*_sections);
				}
				else if(_creation)
				{
					if (_source.get())
					{
						_sscenario->set<Sections>(_source->get<Sections>());
					}
					else if (_template.get())
					{
						_sscenario->set<Sections>(_template->get<Sections>());

					}

				}
				// Enabled
				if(_enabled)
				{
					DBLogModule::appendToLogIfChange(
						text,
						"Affichage ",
						_sscenario->getIsEnabled() ? "activé" : "désactivé",
						*_enabled ? "activé" : "désactivé"
					);
					if (_sscenario->getIsEnabled() != *_enabled)
					{
						_sscenario->setIsEnabled(*_enabled);
						if (_sscenario->belongsToAnAutomaticSection())
						{
							_sscenario->setManualOverride(true);
						}
					}
				}

				// Archived
				if(_archived)
				{
					DBLogModule::appendToLogIfChange(
						text,
						"Archive ",
						_sscenario->getArchived() ? "archivé" : "non archivé",
						*_archived ? "archivé" : "non archivé"
						);
					_sscenario->setArchived(*_archived);
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

			}

			// Save
			if (_sscenario.get())
			{
				SentScenarioTableSync::Save(_sscenario.get(), transaction);
			}
			else
			{
				ScenarioTemplateTableSync::Save(_tscenario.get(), transaction);
			}

			//////////////////////////////////////////////////////////////////////////
			// Messages
			
			// Copy from template (for creation)
			if(_creation && (_source.get() || _template.get()))
			{
				const Scenario& tpl(
					_template.get() ?
					*static_pointer_cast<const Scenario, const ScenarioTemplate>(_template) :
					*static_pointer_cast<const Scenario, const SentScenario>(_source)
				);
				
				if (_sscenario.get())
				{
					ScenarioTemplateTableSync::CopyMessages(tpl.getKey(), *_sscenario, transaction);
				}
				else
				{
					// Unimplemented ScenarioTemplateTableSync::CopyMessages(tpl.getKey(), *_tscenario, transaction);
				}				
			}

			else if(_messagesAndCalendars)
			{	// A/B/G/H action, full method
				// Objects to remove if not found
				std::set<ScenarioCalendar*> existingCalendars(_scenario->getCalendars());
				std::set<const Alarm*> existingMessages(_scenario->getMessages());

				// If an existing SentScenario is being updated, notify broadcast points prior any modification of an applicable message
				if((false == _creation) && _sscenario.get())
				{
					notifyBroadcastPoints = true;

					BOOST_FOREACH(const Alarm* existingMessage, existingMessages)
					{
						if(true == existingMessage->isApplicable(now))
						{
							// Run the before message update trigger on each broadcast point
							BOOST_FOREACH(
								const BroadcastPoint::BroadcastPoints::value_type& bp,
								BroadcastPoint::GetBroadcastPoints()
							){
								bp->beforeMessageUpdate(*existingMessage);
							}
						}
					}
				}

				// Loop on calendars
				BOOST_FOREACH(const ptree::value_type& calendarNode, _messagesAndCalendars->get_child("calendar"))
				{
					RegistryKeyType calendarId(calendarNode.second.get("id", RegistryKeyType(0)));
					boost::shared_ptr<ScenarioCalendar> calendar;

					if(	calendarId &&
						_env->getRegistry<ScenarioCalendar>().contains(calendarId)
					){
						calendar = _env->getEditable<ScenarioCalendar>(calendarId);

						// Check if the calendar is linked to the event
						std::set<ScenarioCalendar*>::iterator it(existingCalendars.find(calendar.get()));
						if(it == existingCalendars.end())
						{
							calendar.reset();
						}
						else
						{
							existingCalendars.erase(it);
						}
					}
					
					// Calendar was not found, create it
					if(!calendar.get())
					{
						calendar.reset(new ScenarioCalendar);
						calendar->set<ScenarioPointer>(*_scenario);
					}

					// Update of the calendar properties
					calendar->set<Name>(calendarNode.second.get("name", string()));

					// Save
					ScenarioCalendarTableSync::Save(calendar.get(), transaction);

					// Application periods
					ScenarioCalendar::ApplicationPeriods existingPeriods(calendar->getApplicationPeriods());

					// Loop on received periods
					BOOST_FOREACH(const ptree::value_type& periodNode, calendarNode.second.get_child("period"))
					{
						RegistryKeyType periodId(periodNode.second.get("id", RegistryKeyType(0)));
						boost::shared_ptr<MessageApplicationPeriod> period;

						if(	periodId &&
							_env->getRegistry<MessageApplicationPeriod>().contains(periodId)
						){
							period = _env->getEditable<MessageApplicationPeriod>(periodId);

							// Check if the period is linked to the event
							ScenarioCalendar::ApplicationPeriods::iterator it(existingPeriods.find(period.get()));
							if(it == existingPeriods.end())
							{
								period.reset();
							}
							else
							{
								existingPeriods.erase(it);
							}
						}

						// Period was not found, create it
						if(!period.get())
						{
							period.reset(new MessageApplicationPeriod);
							period->set<ScenarioCalendar>(*calendar);
						}

						// Update of the period properties
						string startDateStr(periodNode.second.get("start_date", string()));
						period->set<StartTime>(startDateStr.empty() ? ptime(not_a_date_time) : time_from_string(startDateStr));
						string endDateStr(periodNode.second.get("end_date", string()));
						period->set<EndTime>(endDateStr.empty() ? ptime(not_a_date_time) : time_from_string(endDateStr));
						string startHourStr(periodNode.second.get("start_hour", string()));
						period->set<StartHour>(startHourStr.empty() ? time_duration(not_a_date_time) : duration_from_string(startHourStr));
						string endHourStr(periodNode.second.get("end_hour", string()));
						period->set<EndHour>(endHourStr.empty() ? time_duration(not_a_date_time) : duration_from_string(endHourStr));

						// Save
						MessageApplicationPeriodTableSync::Save(period.get(), transaction);
					}

					// Removals
					BOOST_FOREACH(MessageApplicationPeriod* period, existingPeriods)
					{
						MessageApplicationPeriodTableSync::Remove(request.getSession().get(), period->getKey(), transaction, false);
					}

					// Loop on messages
					BOOST_FOREACH(const ptree::value_type& messageNode, calendarNode.second.get_child("message"))
					{
						RegistryKeyType messageId(messageNode.second.get("id", RegistryKeyType(0)));
						boost::shared_ptr<Alarm> message;

						if(	messageId &&
							(_env->getRegistry<Alarm>().contains(messageId))
						){
							message = _env->getEditable<Alarm>(messageId);

							// Check if the period is linked to the event
							std::set<const Alarm*>::iterator it(existingMessages.find(message.get()));
							if(it == existingMessages.end())
							{
								message.reset();
							}
							else
							{
								existingMessages.erase(it);
							}
						}

						// Period was not found, create it
						if(!message.get())
						{
							message.reset(new Alarm);
							message->setScenario(_scenario.get());
						}

						// Update of the message properties
						message->setCalendar(calendar.get());
						message->setShortMessage(messageNode.second.get("title", string()));
						message->setLevel(static_cast<AlarmLevel>(messageNode.second.get("level", 0)));
						message->setLongMessage(messageNode.second.get("content", string()));
						message->setDisplayDuration(messageNode.second.get("displayDuration", 0));
						message->setDigitizedVersion(messageNode.second.get("digitized_version", string()));
						std::string tagsString = messageNode.second.get("tags", string());
						std::set<string> tags;
						boost::algorithm::split(tags, tagsString, is_any_of(","), token_compress_on );
						message->setTags(tags);
						message->setRepeatInterval(messageNode.second.get("repeat_interval", 0));
						message->setWithAck(messageNode.second.get("with_ack", false));
						message->setMultipleStops(messageNode.second.get("multiple_stops", false));
						message->setPlayTts(messageNode.second.get("play_tts", false));
						message->setLight(messageNode.second.get("light", false));
						message->setDirectionSignCode(messageNode.second.get("direction_sign_code", 0));
						message->setStartStopPoint(messageNode.second.get("start_stop_point", 0));
						message->setEndStopPoint(messageNode.second.get("end_stop_point", 0));
						BOOST_FOREACH(const ptree::value_type& sectionNode, messageNode.second.get_child("section"))
						{
							RegistryKeyType sectionId(sectionNode.second.get("id", RegistryKeyType(0)));
							if (sectionId)
							{
								boost::shared_ptr<const MessagesSection> section;
								section = MessagesSectionTableSync::Get(sectionId, *_env);
								if (section)
								{
									message->setSection(section.get());
								}
							}
						}

						// Save
						AlarmTableSync::Save((message).get(), transaction);

						// Alternatives
						Alarm::MessageAlternatives existingAlternatives(message->getMessageAlternatives());

						// Loop on alternatives
						BOOST_FOREACH(const ptree::value_type& alternativeNode, messageNode.second.get_child("alternative"))
						{
							RegistryKeyType alternativeId(alternativeNode.second.get("id", RegistryKeyType(0)));
							boost::shared_ptr<MessageAlternative> alternative;

							if(	alternativeId &&
								_env->getRegistry<MessageAlternative>().contains(alternativeId)
							){
								alternative = _env->getEditable<MessageAlternative>(alternativeId);

								// Check if the alternative is linked to the event
								Alarm::MessageAlternatives::iterator it(
									existingAlternatives.find(
										&*alternative->get<MessageType>()
								)	);
								if(it == existingAlternatives.end())
								{
									alternative.reset();
								}
								else
								{
									existingAlternatives.erase(it);
								}
							}

							// Period was not found, create it
							if(!alternative.get())
							{
								alternative.reset(new MessageAlternative);
								alternative->set<Alarm>(*message);
							}

							// Properties
							try
							{
								alternative->set<MessageType>(
									*MessageTypeTableSync::GetEditable(
										alternativeNode.second.get("type_id", RegistryKeyType(0)),
										*_env
								)	);
							}
							catch(...)
							{

							}
							alternative->set<Content>(alternativeNode.second.get("content", string()));

							// Save
							MessageAlternativeTableSync::Save(alternative.get(), transaction);
						}

						// Removals
						BOOST_FOREACH(const Alarm::MessageAlternatives::value_type& alternative, existingAlternatives)
						{
							MessageAlternativeTableSync::Remove(request.getSession().get(), alternative.second->getKey(), transaction, false);
						}

						// Links
						BOOST_FOREACH(boost::shared_ptr<AlarmRecipient> linkType, Factory<AlarmRecipient>::GetNewCollection())
						{
							// Recipients
							Alarm::LinkedObjects::mapped_type existingLinks(message->getLinkedObjects(linkType->getFactoryKey()));

							// Recipient nodes may be missing, so we need to test their presence before requesting their children
							std::string factoryKey = linkType->getFactoryKey();
							boost::optional<const ptree&> recipientNode = messageNode.second.get_child_optional(linkType->getFactoryKey() + "_recipient");

							// Loop on recipients
							if(recipientNode)
							{
								BOOST_FOREACH(const ptree::value_type& linkNode, recipientNode.get())
								{
									RegistryKeyType linkId(linkNode.second.get("id", RegistryKeyType(0)));
									boost::shared_ptr<AlarmObjectLink> link;

									if(	linkId &&
										_env->getRegistry<AlarmObjectLink>().contains(linkId)
									){
										link = _env->getEditable<AlarmObjectLink>(linkId);

										// Check if the link is linked to the event
										Alarm::LinkedObjects::mapped_type::iterator it(existingLinks.find(link.get()));
										if(it == existingLinks.end())
										{
											link.reset();
										}
										else
										{
											existingLinks.erase(it);
										}
									}

									// Link was not found, create it
									if(!link.get())
									{
										link.reset(new AlarmObjectLink);
										link->setAlarm(message.get());
										link->setRecipient(linkType->getFactoryKey());
									}

									// Properties
									link->setObjectId(linkNode.second.get("recipient_id", RegistryKeyType(0)));
									link->setParameter(linkNode.second.get("parameter", string()));

									// Save
									AlarmObjectLinkTableSync::Save(link.get(), transaction);
								}
							}

							// Removals
							BOOST_FOREACH(const AlarmObjectLink* link, existingLinks)
							{
								AlarmObjectLinkTableSync::Remove(request.getSession().get(), link->getKey(), transaction, false);
							}
						}
					}
				}

				// Removals
				BOOST_FOREACH(ScenarioCalendar* calendar, existingCalendars)
				{
					ScenarioCalendarTableSync::Remove(request.getSession().get(), calendar->getKey(), transaction, false);
				}
				BOOST_FOREACH(const Alarm* message, existingMessages)
				{
					AlarmTableSync::Remove(request.getSession().get(), message->getKey(), transaction, false);
				}
			}

			else if(_messageToCreate && _level) // A/B/G/H action, simplified method
			{
				boost::shared_ptr<Alarm> message;

				Alarms msgs;
				AlarmTableSync::Search(*_env, std::back_inserter(msgs), _scenario->getKey());

				if(msgs.size() == 1)
				{
					message = msgs.front();
					if(_recipients)
					{
						// Links
						BOOST_FOREACH(boost::shared_ptr<AlarmRecipient> linkType, Factory<AlarmRecipient>::GetNewCollection())
						{
							// Recipients
							Alarm::LinkedObjects::mapped_type existingLinks(message->getLinkedObjects(linkType->getFactoryKey()));
							
							// Removals
							BOOST_FOREACH(const AlarmObjectLink* link, existingLinks)
							{
								AlarmObjectLinkTableSync::Remove(request.getSession().get(), link->getKey(), transaction, false);
							}
						}
					}
				}
				else
				{
					message.reset(new Alarm);
					message->setScenario(_scenario.get());
				}

				message->setShortMessage(_messageToCreateTitle ? *_messageToCreateTitle : "Unique message");
				message->setLongMessage(*_messageToCreate);
				message->setLevel(*_level);
				std::set<string> tags;
				boost::algorithm::split(tags, _tags, is_any_of(","), token_compress_on );
				message->setTags(tags);
				if (_display_duration) message->setDisplayDuration(*_display_duration);
				message->setDigitizedVersion(_digitizedVersion);
				message->setSection(_messageSection.get());

				// Save
				if (message)
				{
					AlarmTableSync::Save(message.get(), transaction);
				}
				
				if(_recipients)
				{
					BOOST_FOREACH(const Recipients::value_type::value_type& recipient, *_recipients)
					{
						AlarmObjectLink link;
						link.setRecipient(recipient.first);
						link.setAlarm(message.get());
						link.setObjectId(recipient.second.first);
						if(recipient.second.second)
						{
							link.setParameter(*recipient.second.second);
						}
						AlarmObjectLinkTableSync::Save(&link, transaction);
					}
				}
			}

			if(_creation)
			{
				// Store the roid of the newly created scenario so that it can be returned to the requester
				request.setActionWillCreateObject();
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

			transaction.run();

			// Notify broadcast points of message modifications
			if(true == notifyBroadcastPoints)
			{
				BOOST_FOREACH(const Alarm* message, _sscenario->getMessages())
				{
					// Note : The message must be reread from the registry, because during the update the recipients are added to the registry object
					// and not to the instance we are processing (this is highly counter-intuitive)
					boost::shared_ptr<Alarm> registryMessagePtr = Env::GetOfficialEnv().getEditableRegistry<Alarm>().getEditable(message->getKey());
					Alarm* registryMessage = registryMessagePtr.get();

					if(true == registryMessage->isApplicable(now))
					{

						// Run the display start trigger on each broadcast point
						BOOST_FOREACH(
							const BroadcastPoint::BroadcastPoints::value_type& bp,
							BroadcastPoint::GetBroadcastPoints()
						){
							bp->afterMessageUpdate(*registryMessage);
						}
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



		bool ScenarioSaveAction::isAuthorized(
			const Session* session
		) const {
			// Making some checks about Messages section rights
/*			bool result = session && session->hasProfile();
			bool atLeastOneSectionWithRights = false;

			if (!_sections)
			{
				if (_scenario)
				{
					if (!_scenario->getSections().empty())
					{
						BOOST_FOREACH(const Scenario::Sections::value_type& section, _scenario->getSections())
						{
							atLeastOneSectionWithRights = atLeastOneSectionWithRights || session->getUser()->getProfile()->isAuthorized<MessagesRight>(
								WRITE,
								UNKNOWN_RIGHT_LEVEL,
								MessagesRight::MESSAGES_SECTION_FACTORY_KEY + "/" + lexical_cast<string>(section->getKey())
							);
						}
					}
					else
					{
						atLeastOneSectionWithRights = true;
					}
				}
			}
			else
			{
				BOOST_FOREACH(const Scenario::Sections::value_type& section, *_sections)
				{
					bool sectionToCheck = true;
					if (_scenario)
					{
						BOOST_FOREACH(const Scenario::Sections::value_type& ssect, _scenario->getSections())
						{
							sectionToCheck = sectionToCheck && (section->getKey() != ssect->getKey());
						}
						if (sectionToCheck)
						{
							atLeastOneSectionWithRights = atLeastOneSectionWithRights || session->getUser()->getProfile()->isAuthorized<MessagesRight>(
										WRITE,
										UNKNOWN_RIGHT_LEVEL,
										MessagesRight::MESSAGES_SECTION_FACTORY_KEY + "/" + lexical_cast<string>(section->getKey())
										);
						}
					}
					else
					{
						atLeastOneSectionWithRights = atLeastOneSectionWithRights || session->getUser()->getProfile()->isAuthorized<MessagesRight>(
									WRITE,
									UNKNOWN_RIGHT_LEVEL,
									MessagesRight::MESSAGES_SECTION_FACTORY_KEY + "/" + lexical_cast<string>(section->getKey())
									);
					}
				}
			}
			result = result && atLeastOneSectionWithRights;
			return result;
			*/
			return true;
		}



		void ScenarioSaveAction::setScenarioId(
			const util::RegistryKeyType id
			) throw(ActionException)
		{
			util::RegistryTableType tableId(util::decodeTableId(id));
			if (tableId == ScenarioTemplateTableSync::TABLE.ID)
			{
				try
				{
					_tscenario = ScenarioTemplateTableSync::GetEditable(id, *_env, UP_LINKS_LOAD_LEVEL);
					_scenario = _tscenario;
				}
				catch(ObjectNotFoundException<SentScenario>& e)
				{
					throw ActionException(PARAMETER_SCENARIO_ID, e, *this);
				}
				
			}
			else
			{
				try
				{
					_sscenario = SentScenarioTableSync::GetEditable(id, *_env, UP_LINKS_LOAD_LEVEL);
					_scenario = _sscenario;
				}
				catch(ObjectNotFoundException<SentScenario>& e)
				{
					throw ActionException(PARAMETER_SCENARIO_ID, e, *this);
				}
			}
		}

		void ScenarioSaveAction::setMessagesAndCalendars(
			boost::optional<boost::property_tree::ptree> messagesAndCalendars
		) {
			_messagesAndCalendars = messagesAndCalendars;
		}

		void ScenarioSaveAction::setScenario(
			boost::shared_ptr<Scenario>	scenario
		) {
			_scenario = scenario;
		}

		void ScenarioSaveAction::setSScenario(
			boost::shared_ptr<SentScenario>	sscenario
		) {
			_sscenario = sscenario;
		}

		void ScenarioSaveAction::setCreation(bool creation)
		{
			_creation = creation;
		}
}	}
