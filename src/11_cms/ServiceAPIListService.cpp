
/** ServiceAPIListService class implementation.
	@file ServiceAPIListService.cpp
	@author Hugues Romain
	@date 2011

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

#include "Action.h"
#include "Conversion.h"
#include "RequestException.h"
#include "Request.h"
#include "ServiceAPIListService.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::ServiceAPIListService>::FACTORY_KEY("ServiceAPIListService");

	namespace cms
	{

	const string ServiceAPIListService::PARAMETER_ACTION_OR_FUNCTION = "action_or_function";

	const string ServiceAPIListService::ATTR_GROUPS = "groups";
	const string ServiceAPIListService::ATTR_GROUP_NAME = "group_name";
	const string ServiceAPIListService::ATTR_API_SERVICES = "services";
	const string ServiceAPIListService::ATTR_PARAM_GROUPS = "param_groups";
	const string ServiceAPIListService::ATTR_PARAM_GROUP_NAME = "param_group_name";
	const string ServiceAPIListService::ATTR_NAME = "name";
	const string ServiceAPIListService::ATTR_TITLE = "title";
	const string ServiceAPIListService::ATTR_DESCRIPTION = "description";
	const string ServiceAPIListService::ATTR_DEPRECATED = "deprecated";
	const string ServiceAPIListService::ATTR_PARAMETERS = "parameters";
	const string ServiceAPIListService::ATTR_PARAM_KEY = "param_key";
	const string ServiceAPIListService::ATTR_PARAM_DESCRIPTION = "param_description";
	const string ServiceAPIListService::ATTR_PARAM_MANDATORY = "param_mandatory";

	FunctionAPI ServiceAPIListService::getAPI() const
	{
		FunctionAPI api(
			"Admin",
			"Return the list all the SYNTHESE services",
			"This example displays the list of all the services "
			"and their respective parameters:\n"
			"<?ServiceAPIListService&\n"
			"  action_or_function=function&\n"
			"  template=\n"
			"  <{groups\n"
			"     &template=<h1><@group_name@></h1>\n"
			"     <{services\n"
			"       &template=<h2><@name@> <@deprecated@></h2> <@title@> <br><pre><@description@></pre><br>\n"
			"       <{param_groups\n"
			"          &template=<h3><@param_group_name@></h3>\n"
			"          <{parameters\n"
			"            &template=<li><b><@param_key@></b> <@param_description@></li>\n"
			"          }>\n"
			"       }>\n"
			"       <br>\n"
			"     }>\n"
			"     <br>\n"
			"  }>\n"
		);



		api.addParams(PARAMETER_ACTION_OR_FUNCTION,
					  "Must be 'function' or 'action' to return the respective service set.", false);
		return api;
	}


		ParametersMap ServiceAPIListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void ServiceAPIListService::_setFromParametersMap(const ParametersMap& map)
		{
			_actionOrfunction = map.getDefault<string>(PARAMETER_ACTION_OR_FUNCTION, string("function"));
		}



		util::ParametersMap ServiceAPIListService::run(
				std::ostream& stream,
				const Request& request
				) const {
			GroupedAPI groupedAPI;

			if(_actionOrfunction == "function")
			{
				BOOST_FOREACH(boost::shared_ptr<Function> item, Factory<Function>::GetNewCollection())
				{
					FunctionAPI api(item->getAPI());

					if(groupedAPI.find(api.getGroup()) == groupedAPI.end())
					{
						groupedAPI[api.getGroup()] = APIKey();
					}
					groupedAPI[api.getGroup()][item->getFactoryKey()] = api;
				}
			}
			else
			{
				BOOST_FOREACH(boost::shared_ptr<Action> item, Factory<Action>::GetNewCollection())
				{
					FunctionAPI api(item->getAPI());

					if(groupedAPI.find(api.getGroup()) == groupedAPI.end())
					{
						groupedAPI[api.getGroup()] = APIKey();
					}
					groupedAPI[api.getGroup()][item->getFactoryKey()] = api;
				}
			}
			return getParametersMap(groupedAPI);
		}

		ParametersMap ServiceAPIListService::getParametersMap(const GroupedAPI& groupedAPI) const
		{
			ParametersMap map;

			BOOST_FOREACH(const GroupedAPI::value_type &groupAPI, groupedAPI)
			{
				boost::shared_ptr<ParametersMap> groupedPM(new ParametersMap);
				groupedPM->insert(ATTR_GROUP_NAME, groupAPI.first);

				BOOST_FOREACH(const APIKey::value_type &apiKey, groupAPI.second)
				{
					boost::shared_ptr<ParametersMap> servicePM(new ParametersMap);

					servicePM->insert(ATTR_NAME, apiKey.first);
					const FunctionAPI &api(apiKey.second);
					servicePM->insert(ATTR_TITLE, api.getTitle());
					servicePM->insert(ATTR_DESCRIPTION,
									  Conversion::ToXMLAttrString(api.getDescription()));
					if(api.isDeprecated())
					{
						servicePM->insert(ATTR_DEPRECATED, api.isDeprecated());
					}

					std::map<std::string, std::vector<FunctionAPIParam> > apiParams();

					BOOST_FOREACH(const FunctionAPI::APIParamsMap::value_type& it, api.getParams())
					{
						boost::shared_ptr<ParametersMap> paramGroupPM(new ParametersMap);
						paramGroupPM->insert(ATTR_PARAM_GROUP_NAME, it.first);

						BOOST_FOREACH(FunctionAPIParam param, it.second)
						{
							boost::shared_ptr<ParametersMap> paramPM(new ParametersMap);
							paramPM->insert(ATTR_PARAM_KEY, param.getKey());
							paramPM->insert(ATTR_PARAM_DESCRIPTION, param.getDescription());
							paramPM->insert(ATTR_PARAM_MANDATORY, param.isMandatory());
							paramGroupPM->insert(ATTR_PARAMETERS, paramPM);
						}
						servicePM->insert(ATTR_PARAM_GROUPS, paramGroupPM);
					}
					groupedPM->insert(ATTR_API_SERVICES, servicePM);
				}

				map.insert(ATTR_GROUPS, groupedPM);
			}

			return map;
		}

		bool ServiceAPIListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ServiceAPIListService::getOutputMimeType() const
		{
			return "text/plain";
		}

}	}
