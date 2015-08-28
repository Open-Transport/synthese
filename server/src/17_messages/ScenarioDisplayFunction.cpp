
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
#include "MessagesSection.hpp"
#include "SentScenario.h"
#include "Alarm.h"
#include "ScenarioFolder.h"
#include "SentScenarioTableSync.h"
#include "ScenarioTemplateTableSync.h"

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

		const string ScenarioDisplayFunction::FORMAT_JSON("json");
		const string ScenarioDisplayFunction::FORMAT_XML("xml");

		const string ScenarioDisplayFunction::DATA_MESSAGES = "messages";
		const string ScenarioDisplayFunction::DATA_VARIABLES = "variables";
		const string ScenarioDisplayFunction::DATA_RANK = "rank";

		const string ScenarioDisplayFunction::TAG_SCENARIO = "scenario";



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

			util::RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			util::RegistryTableType tableId(util::decodeTableId(id));
			
			// Scenario
			if (tableId == ScenarioTemplateTableSync::TABLE.ID)
			{
				try
				{
					_scenario = ScenarioTemplateTableSync::Get(id, *_env, UP_LINKS_LOAD_LEVEL);
				}
				catch(ObjectNotFoundException<ScenarioTemplate>& e)
				{
					throw RequestException("No such scenario");
				}

			}
			else
			{
				try
				{
					_scenario = SentScenarioTableSync::Get(id, *_env, UP_LINKS_LOAD_LEVEL);
				}
				catch(ObjectNotFoundException<SentScenario>& e)
				{
					throw RequestException("No such scenario");
				}
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

			ParametersMap pm;

			_scenario->toParametersMap(pm, true);
			if(dynamic_cast<const SentScenario*>(_scenario.get()))
			{
				static_cast<const SentScenario*>(_scenario.get())->toParametersMap(pm);
			}
			
			// Output
			if(_mainTemplate.get()) // CMS output
			{
				// Variables integration
				if(_variableTemplate.get() && pm.hasSubMaps(ScenarioTemplate::TAG_VARIABLE))
				{
					stringstream s;
					size_t rank(0);
					BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmVariable, pm.getSubMaps(ScenarioTemplate::TAG_VARIABLE))
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
					BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmMessage, pm.getSubMaps(ScenarioTemplate::TAG_MESSAGE))
					{
						pmMessage->insert(DATA_RANK, rank++);
						_messageTemplate->display(s, request, *pmMessage);
					}
					pm.insert(DATA_MESSAGES, s.str());
				}

				// Backward compatibility (deprecated tag)
				pm.insert(Request::PARAMETER_OBJECT_ID, _scenario->getKey());
				pm.merge(getTemplateParameters());

				_mainTemplate->display(stream, request, pm);
			}
			else if(_outputFormat == FORMAT_XML) // XML output
			{
				pm.outputXML(
					stream,
					TAG_SCENARIO,
					true,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/17_messages/ScenarioDisplayFunction.xsd"
				);
			}
			else if(_outputFormat == FORMAT_JSON) // JSON output
			{
				pm.outputJSON(
					stream,
					TAG_SCENARIO
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
}	}
