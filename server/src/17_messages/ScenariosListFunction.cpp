
/** ScenariosListFunction class implementation.
	@file ScenariosListFunction.cpp
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

#include "ScenariosListFunction.hpp"

#include "MessagesSection.hpp"
#include "RequestException.h"
#include "Request.h"
#include "MessagesRight.h"
#include "ScenarioFolderTableSync.h"
#include "Webpage.h"
#include "ScenarioFolder.h"
#include "ScenarioTemplateTableSync.h"
#include "SentScenarioTableSync.h"
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

	template<> const string util::FactorableTemplate<Function,messages::ScenariosListFunction>::FACTORY_KEY("scenarii");

	namespace messages
	{
		const string ScenariosListFunction::PARAMETER_CMS_TEMPLATE_ID("p");
		const string ScenariosListFunction::PARAMETER_FOLDER_ID("f");
		const string ScenariosListFunction::PARAMETER_SHOW_TEMPLATES("t");
		const string ScenariosListFunction::PARAMETER_CURRENTLY_DISPLAYED = "currently_displayed";
		const string ScenariosListFunction::PARAMETER_SECTION_IN = "section_in";
		const string ScenariosListFunction::PARAMETER_SECTION_OUT = "section_out";
		const string ScenariosListFunction::PARAMETER_TEXT_SEARCH = "text_search";
		const string ScenariosListFunction::PARAMETER_ARCHIVES = "archives";

		const string ScenariosListFunction::TAG_SCENARIO = "scenario";


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

			// Text search
			if(_textSearch)
			{
				map.insert(PARAMETER_TEXT_SEARCH, *_textSearch);
			}

			// Archives only
			if(_archivesOnly)
			{
				map.insert(PARAMETER_ARCHIVES, true);
			}

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
			else
			{
				if(map.isDefined(PARAMETER_CURRENTLY_DISPLAYED))
				{
					_showCurrentlyDisplayed = map.get<bool>(PARAMETER_CURRENTLY_DISPLAYED);
				}
				else
				{
					_showCurrentlyDisplayed = optional<bool>();
				}
			}

			// CMS page for the display
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

			// Section in
			if(!map.getDefault<string>(PARAMETER_SECTION_IN).empty())
			{
				try
				{
					_sectionIn = *Env::GetOfficialEnv().getEditable<MessagesSection>(map.get<RegistryKeyType>(PARAMETER_SECTION_IN));
				}
				catch (bad_lexical_cast&)
				{
					throw RequestException("No such section");
				}
				catch(ObjectNotFoundException<MessagesSection>&)
				{
					throw RequestException("No such section");
				}
			}

			// Section out
			if(!map.getDefault<string>(PARAMETER_SECTION_OUT).empty())
			{
				try
				{
					_sectionOut = *Env::GetOfficialEnv().getEditable<MessagesSection>(map.get<RegistryKeyType>(PARAMETER_SECTION_OUT));
				}
				catch (bad_lexical_cast&)
				{
					throw RequestException("No such section");
				}
				catch(ObjectNotFoundException<MessagesSection>&)
				{
					throw RequestException("No such section");
				}
			}

			// Text search
			string textSearch(map.getDefault<string>(PARAMETER_TEXT_SEARCH));
			if(!textSearch.empty())
			{
				_textSearch = textSearch;
			}

			// Archives only
			if(map.isDefined(PARAMETER_ARCHIVES))
			{
				_archivesOnly = map.getDefault<bool>(PARAMETER_ARCHIVES, false);
			}
		}



		util::ParametersMap ScenariosListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;
			Scenarios scenarios;
			
			if(_showTemplates)
			{
				ScenarioTemplateTableSync::Search(
					*_env,
					std::back_inserter(scenarios),
					_parentFolder.get() ? _parentFolder->getKey() : 0
				);
			}
			else
			{
				SentScenarioTableSync::Search(
					*_env,
					std::back_inserter(scenarios),
					boost::optional<std::string>(),
					_archivesOnly,
					_showCurrentlyDisplayed
				);
			}

			// Applying text search filter
			if(_textSearch)
			{
				Scenarios newScenarios;
				BOOST_FOREACH(const boost::shared_ptr<Scenario>& scenario, scenarios)
				{
					// Try in the scenario name
					string scenarioName(scenario->getName());
					if(find_first(scenarioName, *_textSearch))
					{
						newScenarios.push_back(scenario);
						continue;
					}

					// Try in each message (short and long names)
					BOOST_FOREACH(const std::set<const Alarm*>::value_type& message, scenario->getMessages())
					{
						if( find_first(message->getShortMessage(), *_textSearch) ||
							find_first(message->getLongMessage(), *_textSearch)
						){
							newScenarios.push_back(scenario);
							break;
						}
					}
				}
				scenarios = newScenarios;
			}

			// TODO add an accesor virtul piure in scenario on sections!
			BOOST_FOREACH(const boost::shared_ptr<Scenario>& scenario, scenarios)
			{
				// Section filter
				std::set<MessagesSection*>& sections(scenario->getSections());
				if(	(	_sectionIn && sections.find(&*_sectionIn) == sections.end()) ||
					(	_sectionOut && sections.find(&*_sectionOut) != sections.end())
				){
					continue;
				}

				// Export of the scenario
				boost::shared_ptr<ParametersMap> scenarioPM(new ParametersMap);
				scenario->toParametersMap(*scenarioPM, true);
				if(dynamic_cast<const SentScenario*>(scenario.get()))
				{
					static_cast<const SentScenario*>(scenario.get())->toParametersMap(*scenarioPM);
				}
				pm.insert(TAG_SCENARIO, scenarioPM);
			}

			if(_cmsTemplate.get())
			{
				BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& item, pm.getSubMaps(TAG_SCENARIO))
				{
					_cmsTemplate->display(stream, request, *item);
				}
			}

			return pm;
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
}	}
