
/** ScenarioDisplayFunction class implementation.
	@file ScenarioDisplayFunction.cpp
	@author Hugues Romain
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

#include "RequestException.h"
#include "Request.h"
#include "ScenarioDisplayFunction.hpp"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"
#include "AlarmTemplate.h"
#include "SentAlarm.h"
#include "MessagesObjectsCMSExporters.hpp"
#include "ScenarioFolder.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "ScenarioTableSync.h"
#include "StaticFunctionRequest.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,messages::ScenarioDisplayFunction>::FACTORY_KEY("ScenarioDisplayFunction");
	
	namespace messages
	{
		const string ScenarioDisplayFunction::PARAMETER_MAIN_TEMPLATE("t");
		const string ScenarioDisplayFunction::PARAMETER_MESSAGE_TEMPLATE("m");
		const string ScenarioDisplayFunction::PARAMETER_VARIABLE_TEMPLATE("v");
		
		const std::string ScenarioDisplayFunction::DATA_NAME("name");
		const std::string ScenarioDisplayFunction::DATA_FOLDER_ID("folder_id");
		const std::string ScenarioDisplayFunction::DATA_FOLDER_NAME("folder_name");
		const std::string ScenarioDisplayFunction::DATA_VARIABLES("variables");
		const std::string ScenarioDisplayFunction::DATA_MESSAGES("messages");
		const std::string ScenarioDisplayFunction::DATA_START_DATE("start_date");
		const std::string ScenarioDisplayFunction::DATA_END_DATE("end_date");
		const std::string ScenarioDisplayFunction::DATA_ACTIVE("active");

		const std::string ScenarioDisplayFunction::DATA_CODE("code");
		const std::string ScenarioDisplayFunction::DATA_HELP_MESSAGE("help_message");
		const std::string ScenarioDisplayFunction::DATA_REQUIRED("required");
		const std::string ScenarioDisplayFunction::DATA_VALUE("value");

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
			return map;
		}



		void ScenarioDisplayFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_scenario = ScenarioTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					*_env,
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch(ObjectNotFoundException<Scenario>& e)
			{
				throw RequestException("No such scenario");
			}

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
		}



		void ScenarioDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			bool isTemplate(dynamic_cast<const ScenarioTemplate*>(_scenario.get()));

			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(_mainTemplate);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			// roid
			pm.insert(Request::PARAMETER_OBJECT_ID, _scenario->getKey());

			// name
			pm.insert(DATA_NAME, _scenario->getName());

			// variables
			if(	isTemplate ||
				static_cast<const SentScenario*>(_scenario.get())->getTemplate()
			){
				stringstream s;
				const ScenarioTemplate::VariablesMap& variables(
					(	isTemplate ?
						static_cast<const ScenarioTemplate*>(_scenario.get()) :
						static_cast<const SentScenario*>(_scenario.get())->getTemplate()
					)->getVariables()
				);
				BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
				{
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
					_displayVariable(s, request, variable.second, value);
				}
				pm.insert(DATA_VARIABLES, s.str());
			}

			// messages
			if(_messageTemplate.get())
			{
				vector<shared_ptr<Alarm> > v;

				if (isTemplate)
				{
					AlarmTemplateInheritedTableSync::SearchResult alarms(
						AlarmTemplateInheritedTableSync::Search(*_env, _scenario->getKey())
					);
					BOOST_FOREACH(shared_ptr<AlarmTemplate> alarm, alarms)
					{
						v.push_back(static_pointer_cast<Alarm, AlarmTemplate>(alarm));
					}
				}
				else
				{
					ScenarioSentAlarmInheritedTableSync::SearchResult alarms(
						ScenarioSentAlarmInheritedTableSync::Search(*_env, _scenario->getKey())
					);
					BOOST_FOREACH(shared_ptr<SentAlarm> alarm, alarms)
					{
						v.push_back(static_pointer_cast<Alarm, SentAlarm>(alarm));
					}
				}

				BOOST_FOREACH(shared_ptr<Alarm> alarm, v)
				{
					MessagesObjectsCMSExporters::DisplayMessage(
						stream,
						request,
						_messageTemplate,
						*alarm
					);
				}
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
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
		
		
		
		bool ScenarioDisplayFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ScenarioDisplayFunction::getOutputMimeType() const
		{
			return _mainTemplate.get() ? _mainTemplate->getMimeType() : "text/plain";
		}



		void ScenarioDisplayFunction::_displayVariable(
			std::ostream& stream,
			const server::Request& request,
			const ScenarioTemplate::Variable& variable,
			const std::string& value
		) const	{

			if(!_variableTemplate.get())
			{
				return;
			}

			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(_variableTemplate);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			// code
			pm.insert(DATA_CODE, variable.code);

			// help_message
			pm.insert(DATA_HELP_MESSAGE, variable.helpMessage);

			// required
			pm.insert(DATA_REQUIRED, variable.compulsory);

			// value
			pm.insert(DATA_VALUE, value);

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
}	}
