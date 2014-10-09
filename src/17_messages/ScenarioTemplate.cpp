////////////////////////////////////////////////////////////////////////////////
/// ScenarioTemplate class implementation.
///	@file ScenarioTemplate.cpp
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

#include "ScenarioTemplate.h"

#include "AlarmTemplate.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "MessagesSection.hpp"
#include "MessagesSectionTableSync.hpp"
#include "ParametersMap.h"
#include "Registry.h"
#include "Request.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "ScenarioTableSync.h"
#include "SentAlarm.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace util;
	using namespace impex;

	namespace messages
	{
		const std::string ScenarioTemplate::DATA_SCENARIO_ID = "scenario_id";
		const std::string ScenarioTemplate::DATA_NAME = "name";
		const std::string ScenarioTemplate::DATA_FOLDER_ID = "folder_id";
		const std::string ScenarioTemplate::DATA_FOLDER_NAME = "folder_name";
		const std::string ScenarioTemplate::DATA_IS_TEMPLATE = "is_template";
		const std::string ScenarioTemplate::DATA_CODE = "code";
		const std::string ScenarioTemplate::DATA_HELP_MESSAGE = "help_message";
		const std::string ScenarioTemplate::DATA_REQUIRED = "required";

		const std::string ScenarioTemplate::TAG_VARIABLE = "variable";
		const std::string ScenarioTemplate::TAG_MESSAGE = "message";
		const std::string ScenarioTemplate::TAG_SECTION = "section";
		const std::string ScenarioTemplate::TAG_CALENDAR = "calendar";



		ScenarioTemplate::ScenarioTemplate(
			const ScenarioTemplate& source,
			const std::string& name
		):	Registrable(0),
			Scenario(name),
			_folder(source._folder),
			_variables(source._variables)
		{}



		ScenarioTemplate::ScenarioTemplate(
			const std::string name,
			ScenarioFolder* folder
		):	Registrable(0),
			Scenario(name),
			_folder(folder)
		{}



		ScenarioTemplate::ScenarioTemplate(
			util::RegistryKeyType key
		):	util::Registrable(key)
			, Scenario()
			, _folder(NULL)
		{}



		ScenarioTemplate::ScenarioTemplate(
			const SentScenario& source,
			const std::string& name
		):	Registrable(0),
			Scenario(name),
			_folder(NULL)
		{}



		ScenarioTemplate::~ScenarioTemplate()
		{}



		ScenarioFolder* ScenarioTemplate::getFolder() const
		{
			return _folder;
		}



		void ScenarioTemplate::setFolder(ScenarioFolder* value )
		{
			_folder = value;
		}



		const ScenarioTemplate::VariablesMap& ScenarioTemplate::getVariables(
		) const {
			return _variables;
		}



		void ScenarioTemplate::setVariablesMap(
			const VariablesMap& value
		){
			_variables = value;
		}



		void ScenarioTemplate::GetVariablesInformations(
			const std::string& text,
			ScenarioTemplate::VariablesMap& result
		){
			for(string::const_iterator it(text.begin()); it != text.end(); ++it)
			{
				// jump over other characters than $
				if (*it != '$') continue;

				++it;

				// $$$ = $
				if (it != text.end() && *it== '$' && it+1 != text.end() && *(it+1) == '$')
				{
					++it;
					continue;
				}

				// this is a variable definition
				ScenarioTemplate::Variable v;

				if(it == text.end())
				{
					break;
				}

				// compulsory variable
				if (*it == '$')
				{
					++it;
					v.compulsory = true;
				}
				else
				{
					v.compulsory = false;
				}

				// variable code
				string::const_iterator it2(it);
				for(; it != text.end() && *it != '|' && *it != '$'; ++it);
				if (it == text.end())
				{
					break;
				}
				v.code = text.substr(it2-text.begin(), it-it2);

				// variable information
				if (*it == '|')
				{
					++it;
					it2 = it;
					for(; it != text.end() && *it != '$'; ++it);
					if (it == text.end())
					{
						break;
					}
					v.helpMessage = text.substr(it2-text.begin(), it-it2);
				}

				// storage
				ScenarioTemplate::VariablesMap::iterator vmit(result.find(v.code));
				if(vmit == result.end())
				{
					result.insert(make_pair(v.code, v));
				}
				else
				{
					vmit->second.helpMessage += v.helpMessage;
					if(v.compulsory) vmit->second.compulsory = true;
				}
			}
		}



		std::string ScenarioTemplate::WriteTextFromVariables(
			const std::string& text,
			const SentScenario::VariablesMap& variables
		){
			stringstream stream;

			for(string::const_iterator it(text.begin()); it != text.end(); ++it)
			{
				// jump over other characters than $
				if (*it != '$')
				{
					stream << *it;
					continue;
				}

				++it;

				// $$$ = $
				if (it != text.end() && *it== '$' && it+1 != text.end() && *(it+1) == '$')
				{
					stream << "$";
					++it;
					continue;
				}

				// compulsory variable
				if (*it == '$')
				{
					++it;
				}

				// variable code
				string code;
				string::const_iterator it2(it);
				for(; it != text.end() && *it != '|' && *it != '$'; ++it);
				if (it == text.end())
				{
					break;
				}
				code = text.substr(it2-text.begin(), it-it2);

				// variable information
				if (*it == '|')
				{
					++it;
					it2 = it;
					for(; it != text.end() && *it != '$'; ++it);
					if (it == text.end())
					{
						break;
					}
				}

				// writing
				SentScenario::VariablesMap::const_iterator vmit(variables.find(code));
				if(vmit != variables.end())
				{
					stream << vmit->second;
				}
			}
			return stream.str();
		}



		bool ScenarioTemplate::CheckCompulsoryVariables(
			const ScenarioTemplate::VariablesMap& variables,
			const SentScenario::VariablesMap& values
		){
			BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
			{
				if(!variable.second.compulsory) continue;

				if(values.find(variable.second.code) == values.end()) return false;
			}
			return true;
		}



		void ScenarioTemplate::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{
			// Inter synthese package
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + ScenarioTableSync::COL_IS_TEMPLATE,
				true
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_ENABLED,
				false
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_NAME,
				getName()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_PERIODSTART,
				ptime(not_a_date_time)
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_PERIODEND,
				ptime(not_a_date_time)
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_FOLDER_ID,
				getFolder() ? getFolder()->getKey() : RegistryKeyType(0)
			);
			stringstream vars;
			pm.insert(
				prefix + ScenarioTableSync::COL_VARIABLES,
				vars.str()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_TEMPLATE,
				RegistryKeyType(0)
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_DATASOURCE_LINKS,
				synthese::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			bool first(true);
			stringstream sectionsStr;
			BOOST_FOREACH(const MessagesSection* section, this->getSections())
			{
				if(first)
				{
					first = false;
				}
				else
				{
					sectionsStr << ScenarioTableSync::SECTIONS_SEPARATOR;
				}
				sectionsStr << section->getKey();
			}
			pm.insert(
				prefix + ScenarioTableSync::COL_SECTIONS,
				sectionsStr.str()
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_EVENT_START,
				ptime(not_a_date_time)
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_EVENT_END,
				ptime(not_a_date_time)
			);
			pm.insert(
				prefix + ScenarioTableSync::COL_ARCHIVED,
				false
			);

			// roid
			pm.insert(DATA_SCENARIO_ID, getKey());
			pm.insert(Request::PARAMETER_OBJECT_ID, getKey()); // Deprecated

			// name
			pm.insert(DATA_NAME, getName());

			// is template
			pm.insert(DATA_IS_TEMPLATE, true);

			// variables
			BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, getVariables())
			{
				shared_ptr<ParametersMap> variablePM(new ParametersMap);
				// code
				variablePM->insert(DATA_CODE, variable.first);

				// help_message
				variablePM->insert(DATA_HELP_MESSAGE, variable.second.helpMessage);

				// required
				variablePM->insert(DATA_REQUIRED, variable.second.compulsory);

				pm.insert(TAG_VARIABLE, variablePM);
			}


			// Calendars
			bool oneMessageWithoutCalendar(false);
			BOOST_FOREACH(const ScenarioCalendar* calendar, getCalendars())
			{
				// Calendar export
				shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				calendar->toParametersMap(*calendarPM, withAdditionalParameters, withFiles, prefix);
				pm.insert(TAG_CALENDAR, calendarPM);

				// Messages loop
				BOOST_FOREACH(const Alarm* alarm, getMessages())
				{
					// Mark message without calendar and avoit it
					if(!alarm->getCalendar())
					{
						oneMessageWithoutCalendar = true;
						continue;
					}

					// Jump over messages of other calendars
					if(alarm->getCalendar() != calendar)
					{
						continue;
					}

					// Message export
					shared_ptr<ParametersMap> messagePM(new ParametersMap);
					alarm->toParametersMap(*messagePM, false, true, string());
					calendarPM->insert(TAG_MESSAGE, messagePM);
				}
			}

			// Fake calendar for old style messages
			if(	oneMessageWithoutCalendar ||
				(getCalendars().empty() && !getMessages().empty())
			){
				// Fake calendar export
				shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				calendarPM->insert(Key::FIELD.name, 0);
				calendarPM->insert(Name::FIELD.name, string());
				calendarPM->insert(ScenarioPointer::FIELD.name, getKey());
				pm.insert(TAG_CALENDAR, calendarPM);

				// Old style messages loop
				BOOST_FOREACH(const Alarm* alarm, getMessages())
				{
					// Jump over new style messages
					if(alarm->getCalendar())
					{
						continue;
					}

					// Message export
					shared_ptr<ParametersMap> messagePM(new ParametersMap);
					alarm->toParametersMap(*messagePM, false, true, string());
					calendarPM->insert(TAG_MESSAGE, messagePM);
				}
			}

			// Sections
			BOOST_FOREACH(const MessagesSection* section, getSections())
			{
				shared_ptr<ParametersMap> sectionPM(new ParametersMap);
				section->toParametersMap(*sectionPM, true);
				pm.insert(TAG_SECTION, sectionPM);
			}

			// Folder
			if(getFolder())
			{
				// folder_id
				pm.insert(DATA_FOLDER_ID, getFolder()->getKey());

				// folder_name
				pm.insert(DATA_FOLDER_NAME, getFolder()->getName());
			}
		}

		bool ScenarioTemplate::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Name
			if(record.isDefined(ScenarioTableSync::COL_NAME))
			{
				string value(
					record.get<string>(ScenarioTableSync::COL_NAME)
				);
				if(value != getName())
				{
					setName(value);
					result = true;
				}
			}

			// Sections
			const string txtSections(record.get<string>(ScenarioTableSync::COL_SECTIONS));
			Scenario::Sections sections;
			if(!txtSections.empty())
			{
				vector<string> tokens;
				split(tokens, txtSections, is_any_of(ScenarioTableSync::SECTIONS_SEPARATOR));
				BOOST_FOREACH(const string& token, tokens)
				{
					try
					{
						sections.insert(
							MessagesSectionTableSync::Get(
								lexical_cast<RegistryKeyType>(token),
								env
							).get()
						);
					}
					catch (bad_lexical_cast&)
					{
					}
					catch(ObjectNotFoundException<MessagesSection>&)
					{
					}
				}
			}
			setSections(sections);

			//Folder
			if(record.isDefined(ScenarioTableSync::COL_FOLDER_ID))
			{
				ScenarioFolder* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						ScenarioTableSync::COL_FOLDER_ID,
						0
				)	);
				if(id > 0)
				{
					try
					{
						value = ScenarioFolderTableSync::GetEditable(id, env).get();
					}
					catch(ObjectNotFoundException<ScenarioFolder>&)
					{
						Log::GetInstance().warn("No such scenario folder in scenario template "+ lexical_cast<string>(getKey()));
					}
				}
				if(value != getFolder())
				{
					setFolder(value);
					result = true;
				}
			}

			// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
			if(record.isDefined(ScenarioTableSync::COL_DATASOURCE_LINKS))
			{
				Importable::DataSourceLinks value(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(
						record.get<string>(ScenarioTableSync::COL_DATASOURCE_LINKS),
						env
				)	);
				if(value != getDataSourceLinks())
				{
					if(&env == &Env::GetOfficialEnv())
					{
						setDataSourceLinksWithRegistration(value);
					}
					else
					{
						setDataSourceLinksWithoutRegistration(value);
					}
					result = true;
				}
			}

			return result;
		}
}	}
