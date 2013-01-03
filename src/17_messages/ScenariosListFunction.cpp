
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

#include "RequestException.h"
#include "Request.h"
#include "MessagesRight.h"
#include "ScenariosListFunction.hpp"
#include "ScenarioFolderTableSync.h"
#include "Webpage.h"
#include "ScenarioFolder.h"
#include "ScenarioTableSync.h"
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
					_showCurrentlyDisplayed = indeterminate;
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

			// Section in
			if(!map.getDefault<string>(PARAMETER_SECTION_IN).empty())
			{
				try
				{
					_sectionIn = map.get<int>(PARAMETER_SECTION_IN);
				}
				catch (bad_lexical_cast&)
				{
					throw RequestException("No such section");
				}
			}

			// Section out
			if(!map.getDefault<string>(PARAMETER_SECTION_OUT).empty())
			{
				try
				{
					_sectionOut = map.get<int>(PARAMETER_SECTION_OUT);
				}
				catch (bad_lexical_cast&)
				{
					throw RequestException("No such section");
				}
			}
		}



		util::ParametersMap ScenariosListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;
			ScenarioTableSync::SearchResult scenarios;
			
			if(_showTemplates)
			{
				scenarios = ScenarioTableSync::SearchTemplates(
					*_env,
					_parentFolder.get() ? _parentFolder->getKey() : 0
				);

			}
			else
			{
				ptime now(second_clock::local_time());
				scenarios = ScenarioTableSync::SearchSentScenarios(
					*_env,
					boost::optional<std::string>(),
					indeterminate(_showCurrentlyDisplayed) ?
						optional<ScenarioTableSync::StatusSearch>() :
						(	_showCurrentlyDisplayed ?
							ScenarioTableSync::BROADCAST_RUNNING :
							ScenarioTableSync::FUTURE_BROADCAST
						),
					now
				);
			}

			BOOST_FOREACH(const shared_ptr<Scenario>& scenario, scenarios)
			{
				// Section filter
				const Scenario::Sections& sections(scenario->getSections());
				if(	(	_sectionIn && sections.find(*_sectionIn) == sections.end()) ||
					(	_sectionOut && sections.find(*_sectionOut) != sections.end())
				){
					continue;
				}

				// Export of the scenario
				shared_ptr<ParametersMap> scenarioPM(new ParametersMap);
				scenario->toParametersMap(*scenarioPM);
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
