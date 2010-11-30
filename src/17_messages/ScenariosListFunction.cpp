
/** ScenariosListFunction class implementation.
	@file ScenariosListFunction.cpp
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
#include "MessagesRight.h"
#include "ScenariosListFunction.hpp"
#include "ScenarioFolderTableSync.h"
#include "StaticFunctionRequest.h"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "ScenarioFolder.h"
#include "SentScenarioInheritedTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "ScenarioTemplate.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,messages::ScenariosListFunction>::FACTORY_KEY("ScenariosListFunction");
	
	namespace messages
	{
		const string ScenariosListFunction::PARAMETER_CMS_TEMPLATE_ID("p");
		const string ScenariosListFunction::PARAMETER_FOLDER_ID("f");
		const string ScenariosListFunction::PARAMETER_SHOW_TEMPLATES("t");

		const string ScenariosListFunction::DATA_NAME("name");
		const string ScenariosListFunction::DATA_START_DATE("start_date");
		const string ScenariosListFunction::DATA_END_DATE("end_date");
		const string ScenariosListFunction::DATA_FOLDER_ID("folder_id");

		
		ParametersMap ScenariosListFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_parentFolder.get())
			{
				map.insert(PARAMETER_FOLDER_ID, _parentFolder->getKey());
			}
			if(_cmsTemplate.get())
			{
				map.insert(PARAMETER_CMS_TEMPLATE_ID, _cmsTemplate->getKey());
			}
			map.insert(PARAMETER_SHOW_TEMPLATES, _showTemplates);
			return map;
		}



		void ScenariosListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_showTemplates = map.get<bool>(PARAMETER_SHOW_TEMPLATES);

			if(_showTemplates)
			{
				optional<RegistryKeyType> id(
					map.getOptional<RegistryKeyType>(PARAMETER_FOLDER_ID)
				);

				if (id)
				{
					try
					{
						_parentFolder = ScenarioFolderTableSync::Get(*id, *_env);
					}
					catch (ObjectNotFoundException<ScenarioFolder>&)
					{
						throw RequestException("Bad folder ID");
					}
				}
			}

			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_CMS_TEMPLATE_ID));
				if(id)
				{
					_cmsTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such CMS template : "+ e.getMessage());
			}
		}

		void ScenariosListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			if(_showTemplates)
			{
				ScenarioTemplateInheritedTableSync::SearchResult scenarios(
					ScenarioTemplateInheritedTableSync::Search(
						*_env,
						_parentFolder.get() ? _parentFolder->getKey() : 0
				)	);

				BOOST_FOREACH(shared_ptr<ScenarioTemplate> scenario, scenarios)
				{
					_displayScenarioTemplate(stream, request, *scenario);
				}
			}
			else
			{
				ptime now(second_clock::local_time());
				SentScenarioInheritedTableSync::SearchResult scenarios(
					SentScenarioInheritedTableSync::Search(
						*_env,
						boost::optional<std::string>(),
						_showCurrentlyDisplayed ? SentScenarioInheritedTableSync::BROADCAST_RUNNING : SentScenarioInheritedTableSync::FUTURE_BROADCAST,
						now
				)	);

				BOOST_FOREACH(shared_ptr<SentScenario> message, scenarios)
				{
					_displaySentScenario(stream, request, *message);
				}
			}
		}
		
		
		
		bool ScenariosListFunction::isAuthorized(
			const Session* session
		) const {
			return true;
			//return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(READ);
		}



		std::string ScenariosListFunction::getOutputMimeType() const
		{
			return _cmsTemplate.get() ? _cmsTemplate->getMimeType() : "text/plain";
		}



		ScenariosListFunction::ScenariosListFunction():
			_showTemplates(true),
			_showCurrentlyDisplayed(true)
		{

		}



		void ScenariosListFunction::_displaySentScenario(
			std::ostream& stream,
			const server::Request& request,
			const SentScenario& scenario
		) const	{

			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(_cmsTemplate);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			// roid
			pm.insert(Request::PARAMETER_OBJECT_ID, scenario.getKey());

			// name
			pm.insert(DATA_NAME, scenario.getName());

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

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void ScenariosListFunction::_displayScenarioTemplate(
			std::ostream& stream,
			const server::Request& request,
			const ScenarioTemplate& scenario
		) const	{

			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(_cmsTemplate);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			// roid
			pm.insert(Request::PARAMETER_OBJECT_ID, scenario.getKey());

			// name
			pm.insert(DATA_NAME, scenario.getName());

			// scenario template
			if(scenario.getFolder())
			{
				pm.insert(DATA_FOLDER_ID, scenario.getFolder()->getKey());
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
}	}
