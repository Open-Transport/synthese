
/** ScenarioDisplayFunction class implementation.
	@file ScenarioDisplayFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "ScenarioDisplayFunction.hpp"
#include "Webpage.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
#include "AlarmTemplate.h"
#include "SentAlarm.h"
#include "ScenarioFolder.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "ScenarioTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,messages::ScenarioDisplayFunction>::FACTORY_KEY("scenario");

	namespace messages
	{
		const string ScenarioDisplayFunction::PARAMETER_MAIN_TEMPLATE("t");
		const string ScenarioDisplayFunction::PARAMETER_MESSAGE_TEMPLATE("m");
		const string ScenarioDisplayFunction::PARAMETER_VARIABLE_TEMPLATE("v");
		const string ScenarioDisplayFunction::PARAMETER_OUTPUT_FORMAT("of");

		const std::string ScenarioDisplayFunction::DATA_NAME("name");
		const std::string ScenarioDisplayFunction::DATA_IS_TEMPLATE("is_template");
		const std::string ScenarioDisplayFunction::DATA_RANK("rank");
		const std::string ScenarioDisplayFunction::DATA_SCENARIO("scenario");
		const std::string ScenarioDisplayFunction::DATA_SCENARIO_ID("scenario_id");
		const std::string ScenarioDisplayFunction::DATA_FOLDER_ID("folder_id");
		const std::string ScenarioDisplayFunction::DATA_FOLDER_NAME("folder_name");
		const std::string ScenarioDisplayFunction::DATA_VARIABLES("variables");
		const std::string ScenarioDisplayFunction::DATA_VARIABLE("variable");
		const std::string ScenarioDisplayFunction::DATA_MESSAGES("messages");
		const std::string ScenarioDisplayFunction::DATA_MESSAGE("message");
		const std::string ScenarioDisplayFunction::DATA_START_DATE("start_date");
		const std::string ScenarioDisplayFunction::DATA_END_DATE("end_date");
		const std::string ScenarioDisplayFunction::DATA_ACTIVE("active");

		const std::string ScenarioDisplayFunction::DATA_CODE("code");
		const std::string ScenarioDisplayFunction::DATA_HELP_MESSAGE("help_message");
		const std::string ScenarioDisplayFunction::DATA_REQUIRED("required");
		const std::string ScenarioDisplayFunction::DATA_VALUE("value");

		const string ScenarioDisplayFunction::FORMAT_JSON("json");
		const string ScenarioDisplayFunction::FORMAT_XML("xml");

		ParametersMap ScenarioDisplayFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_scenario.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _scenario->getKey());
			}
			if(_mainTemplate.get())
			{
				map.insert(PARAMETER_MAIN_TEMPLATE, _mainTemplate->getKey());
			}
			if(_messageTemplate.get())
			{
				map.insert(PARAMETER_MESSAGE_TEMPLATE, _messageTemplate->getKey());
			}
			if(_variableTemplate.get())
			{
				map.insert(PARAMETER_VARIABLE_TEMPLATE, _variableTemplate->getKey());
			}

			// Output format
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}

			return map;
		}



		void ScenarioDisplayFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Output format
			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);

			// Scenario
			try
			{
				_scenario = ScenarioTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					*_env,
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch(ObjectNotFoundException<Scenario>&)
			{
				throw RequestException("No such scenario");
			}

			// Main CMS template
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MAIN_TEMPLATE));
				if(id)
				{
					_mainTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such main CMS template : "+ e.getMessage());
			}

			// Message CMS template
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MESSAGE_TEMPLATE));
				if(id)
				{
					_messageTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such message CMS template : "+ e.getMessage());
			}

			// Variable CMS template
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_VARIABLE_TEMPLATE));
				if(id)
				{
					_variableTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such variable CMS template : "+ e.getMessage());
			}

			// Cleaning of template parameters for non CMS output
			if(!_mainTemplate.get())
			{
				_templateParameters.clear();
			}
		}



		util::ParametersMap ScenarioDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			bool isTemplate(dynamic_cast<const ScenarioTemplate*>(_scenario.get()) != NULL);

			ParametersMap pm(getTemplateParameters());

			// roid
			pm.insert(DATA_SCENARIO_ID, _scenario->getKey());

			// name
			pm.insert(DATA_NAME, _scenario->getName());

			// is template
			pm.insert(DATA_IS_TEMPLATE, isTemplate);

			// variables
			if(	isTemplate ||
				static_cast<const SentScenario*>(_scenario.get())->getTemplate()
			){
				const ScenarioTemplate::VariablesMap& variables(
					(	isTemplate ?
						static_cast<const ScenarioTemplate*>(_scenario.get()) :
						static_cast<const SentScenario*>(_scenario.get())->getTemplate()
					)->getVariables()
				);
				BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
				{
					shared_ptr<ParametersMap> variablePM(new ParametersMap(getTemplateParameters()));
					string value;
					if(!isTemplate)
					{
						const SentScenario::VariablesMap& values(static_cast<const SentScenario*>(_scenario.get())->getVariables());
						SentScenario::VariablesMap::const_iterator it(values.find(variable.first));
						if(it != values.end())
						{
							value = it->second;
						}
					}
					_displayVariable(*variablePM, variable.second, value);
					pm.insert(DATA_VARIABLE, variablePM);
				}
			}

			// messages
			vector<shared_ptr<Alarm> > v;

			if (isTemplate)
			{
				AlarmTemplateInheritedTableSync::SearchResult alarms(
					AlarmTemplateInheritedTableSync::Search(*_env, _scenario->getKey())
				);
				BOOST_FOREACH(const shared_ptr<AlarmTemplate>& alarm, alarms)
				{
					v.push_back(static_pointer_cast<Alarm, AlarmTemplate>(alarm));
				}
			}
			else
			{
				ScenarioSentAlarmInheritedTableSync::SearchResult alarms(
					ScenarioSentAlarmInheritedTableSync::Search(*_env, _scenario->getKey())
				);
				BOOST_FOREACH(const shared_ptr<SentAlarm>& alarm, alarms)
				{
					v.push_back(static_pointer_cast<Alarm, SentAlarm>(alarm));
				}
			}

			// Messages
			BOOST_FOREACH(const shared_ptr<Alarm>& alarm, v)
			{
				shared_ptr<ParametersMap> messagePM(new ParametersMap(getTemplateParameters()));
				alarm->toParametersMap(*messagePM, false);
				pm.insert(DATA_MESSAGE, messagePM);
			}

			if(isTemplate)
			{
				ScenarioFolder* folder(static_cast<const ScenarioTemplate*>(_scenario.get())->getFolder());

				if(folder)
				{
					// folder_id
					pm.insert(DATA_FOLDER_ID, folder->getKey());

					// folder_name
					pm.insert(DATA_FOLDER_NAME, folder->getName());
				}
			}
			else
			{
				const SentScenario& scenario(*static_cast<const SentScenario*>(_scenario.get()));

				// start date
				if(!scenario.getPeriodStart().is_not_a_date_time())
				{
					pm.insert(DATA_START_DATE, scenario.getPeriodStart());
				}

				// end date
				if(!scenario.getPeriodEnd().is_not_a_date_time())
				{
					pm.insert(DATA_END_DATE, scenario.getPeriodEnd());
				}

				// active
				pm.insert(DATA_ACTIVE, scenario.getIsEnabled());
			}

			// Output
			if(_mainTemplate.get()) // CMS output
			{
				// Variables integration
				if(_variableTemplate.get() && pm.hasSubMaps(DATA_VARIABLE))
				{
					stringstream s;
					size_t rank(0);
					BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmVariable, pm.getSubMaps(DATA_VARIABLE))
					{
						pmVariable->insert(DATA_RANK, rank++);
						_variableTemplate->display(s, request, *pmVariable);
					}
					pm.insert(DATA_VARIABLES, s.str());
				}

				// Messages integration
				if(_messageTemplate.get())
				{
					stringstream s;
					size_t rank(0);
					BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmMessage, pm.getSubMaps(DATA_MESSAGE))
					{
						pmMessage->insert(DATA_RANK, rank++);
						_messageTemplate->display(s, request, *pmMessage);
					}
					pm.insert(DATA_MESSAGES, s.str());
				}

				// Backward compatibility (deprecated tag)
				pm.insert(Request::PARAMETER_OBJECT_ID, _scenario->getKey());

				_mainTemplate->display(stream, request, pm);
			}
			else if(_outputFormat == FORMAT_XML) // XML output
			{
				pm.outputXML(
					stream,
					DATA_SCENARIO,
					true,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/17_messages/ScenarioDisplayFunction.xsd"
				);
			}
			else if(_outputFormat == FORMAT_JSON) // JSON output
			{
				pm.outputJSON(
					stream,
					DATA_SCENARIO
				);
			}

			return pm;
		}



		bool ScenarioDisplayFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ScenarioDisplayFunction::getOutputMimeType() const
		{
			std::string mimeType;
			if(_mainTemplate.get())
			{
				mimeType = _mainTemplate->getMimeType();
			}
			else if(_outputFormat == FORMAT_XML)
			{
				mimeType = "text/xml";
			}
			else if(_outputFormat == FORMAT_JSON)
			{
				mimeType = "application/json";
			}
			else // For empty result
			{
				mimeType = "text/plain";
			}
			return mimeType;
		}



		void ScenarioDisplayFunction::_displayVariable(
			ParametersMap& pm,
			const ScenarioTemplate::Variable& variable,
			const std::string& value
		) const	{

			// code
			pm.insert(DATA_CODE, variable.code);

			// help_message
			pm.insert(DATA_HELP_MESSAGE, variable.helpMessage);

			// required
			pm.insert(DATA_REQUIRED, variable.compulsory);

			// value
			pm.insert(DATA_VALUE, value);
		}
}	}
