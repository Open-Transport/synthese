
/** WebPageDisplayFunction class implementation.
	@file WebPageDisplayFunction.cpp
	@author Hugues
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

#include "WebPageDisplayFunction.h"

#include "Action.h"
#include "CMSModule.hpp"
#include "DelayedEvaluationParametersMap.hpp"
#include "Env.h"
#include "Request.h"
#include "RequestException.h"
#include "ServerConstants.h"
#include "URI.hpp"
#include "Webpage.h"
#include "Website.hpp"

#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function, WebPageDisplayFunction>::FACTORY_KEY("page");

	namespace cms
	{
		const string WebPageDisplayFunction::DATA_CONTENT = "content";
		const string WebPageDisplayFunction::ATTR_EQUIV_URL = "equiv_url";

		const string WebPageDisplayFunction::PARAMETER_PAGE_ID = "p";
		const string WebPageDisplayFunction::PARAMETER_SITE_ID = "si";
		const string WebPageDisplayFunction::PARAMETER_USE_TEMPLATE = "use_template";
		const string WebPageDisplayFunction::PARAMETER_HOST_NAME = "host_name";
		const string WebPageDisplayFunction::PARAMETER_SMART_URL = "smart_url";
		const string WebPageDisplayFunction::PARAMETER_RAW_DATA = "raw_data";
		const string WebPageDisplayFunction::PARAMETER_DONT_REDIRECT_IF_SMART_URL = "dont_redirect_if_smart_url";

		ParametersMap WebPageDisplayFunction::_getParametersMap() const
		{
			ParametersMap map(getTemplateParameters());

			// Page
			if(_page)
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}

			// Do not redirect if smart URL
			if(_dontRedirectIfSmartURL)
			{
				map.insert(PARAMETER_DONT_REDIRECT_IF_SMART_URL, _dontRedirectIfSmartURL);
			}

			return map;
		}



		void WebPageDisplayFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Case page id
			RegistryKeyType pageId(map.getDefault<RegistryKeyType>(PARAMETER_PAGE_ID, 0));
			if(pageId)
			{
				try
				{
					_page = Env::GetOfficialEnv().get<Webpage>(
						pageId
					).get();
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such page : "+ lexical_cast<string>(pageId));
				}
				if(	!_page->mustBeDisplayed()
				){
					throw Request::ForbiddenRequestException();
				}
			}
			else // Case Website + page name
			{
				// Smart URL
				_smartURL = map.get<string>(PARAMETER_SMART_URL);
				if(_smartURL.empty())
				{
					_smartURL = "/";
				}

				// Local variables
				const Website* site(NULL);

				// Case site id
				RegistryKeyType siteId(
					map.getDefault<RegistryKeyType>(PARAMETER_SITE_ID, 0)
				);
				if(siteId)
				{
					try
					{
						site = Env::GetOfficialEnv().get<Website>(siteId).get();
					}
					catch(ObjectNotFoundException<Website>&)
					{
						throw RequestException("No such site : "+ lexical_cast<string>(siteId));
					}
				}
				else 
				{
					string hostName(map.getDefault<string>(PARAMETER_HOST_NAME));

					// Site
					site = CMSModule::GetSiteByURL(hostName, _smartURL);
					if(!site)
					{
						throw RequestException("No such site");
					}
						
					// Smart URL
					_smartURL = _smartURL.substr(site->get<ClientURL>().size());

					_templateParameters.remove(PARAMETER_HOST_NAME);
					_templateParameters.remove(PARAMETER_SMART_URL);
				}

				// Page
				if(!_smartURL.empty() && _smartURL[0] == ':')
				{
					throw RequestException("Smart URLs starting with a colon are not allowed for direct access");
				}
				_page = site->getPageBySmartURL(_smartURL);
				if(!_page)
				{
					_smartURL = _smartURL + "/";
					_page = site->getPageBySmartURL(_smartURL);
				}

				if(!_page)
				{ // Attempt to find a page with a parameter
					vector<string> paths;
					iter_split(paths, _smartURL, last_finder("/"));
					if(paths.size() != 2)
					{
						throw Request::NotFoundException();
					}

					_page = site->getPageBySmartURL(paths[0]);

					if(!_page || _page->get<SmartURLDefaultParameterName>().empty())
					{
						throw Request::NotFoundException();
					}

					_templateParameters.insert(_page->get<SmartURLDefaultParameterName>(), paths[1]);
				}
				_templateParameters.insert(PARAMETER_PAGE_ID, _page->getKey());
				_templateParameters.remove(Request::PARAMETER_ERROR_MESSAGE);
			}

			// Use template around the page
			_useTemplate = map.getDefault<bool>(PARAMETER_USE_TEMPLATE, true);

			// Do not redirect if smart URL
			_dontRedirectIfSmartURL = map.getDefault<bool>(PARAMETER_DONT_REDIRECT_IF_SMART_URL, false);
			if(_dontRedirectIfSmartURL)
			{
				_templateParameters.insert(PARAMETER_DONT_REDIRECT_IF_SMART_URL, _dontRedirectIfSmartURL);
			}

			// Raw mode (no content evaluation)
			_rawData = map.getDefault<bool>(PARAMETER_RAW_DATA, false);

			// Equivalent URL
			stringstream uri;
			map.outputURI(uri);
			_equivURI = uri.str();
		}



		util::ParametersMap WebPageDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			if(_page)
			{
				// Raw mode (no content evaluation)
				if(_rawData)
				{
					ParametersMap pm;
					_page->toParametersMap(pm);
					return pm;
				}

				// If page has been fetched by its id and its smart URL is defined, then
				// redirect permanently to the smart url, unless it starts with a colon (used for aliases).
				if(!_dontRedirectIfSmartURL && _smartURL.empty() && !_page->get<SmartURLPath>().empty() && _page->get<SmartURLPath>()[0] != ':')
				{
					/// @todo handle default parameter of smart url
					stringstream url;
					url << "http://" << request.getHostName() << _page->getRoot()->get<ClientURL>() << _page->get<SmartURLPath>();

					ParametersMap pm(getTemplateParameters());
					pm.remove(PARAMETER_PAGE_ID);
					pm.remove(PARAMETER_CLIENT_ADDRESS);
					stringstream uri;
					pm.outputURI(uri);
					string parameters(uri.str());
					if(!parameters.empty())
					{
						url << Request::PARAMETER_STARTER << parameters;
					}

					throw Request::RedirectException(url.str(), true);
				}

				ParametersMap pm(getTemplateParameters());
				pm.remove(PARAMETER_CLIENT_ADDRESS);
				stringstream url;
				url << "http://" << request.getHostName();
				bool started(false);
				if(!_page->get<SmartURLPath>().empty())
				{
					url << _page->getRoot()->get<ClientURL>() << _page->get<SmartURLPath>();
				}
				else
				{
					url <<
						request.getClientURL() << Request::PARAMETER_STARTER <<
						Request::PARAMETER_SERVICE << URI::PARAMETER_ASSIGNMENT << FACTORY_KEY
					;
					started = true;
				}
			
				stringstream uri;
				pm.outputURI(uri);
				string parameters(uri.str());
				if(!parameters.empty())
				{
					if(!started)
					{
						url << Request::PARAMETER_STARTER;
					}
					else
					{
						url << URI::PARAMETER_SEPARATOR;
					}
					url << parameters;
				}
				pm.insert(ATTR_EQUIV_URL, url.str());

				// Page data
				_page->toParametersMap(pm);

				if(_useTemplate && _page->getTemplate())
				{
					DelayedEvaluationParametersMap::Fields fields;
					BOOST_FOREACH(const ParametersMap::Map::value_type& it, pm.getMap())
					{
						fields.insert(
							make_pair(
								it.first,
								DelayedEvaluationParametersMap::Field(it.second)
						)	);
					}

					// The page content will be evaluated when it will be displayed
					// Variables initialized by the template are available at the page
					// evaluation
					stringstream content;
					fields.insert(
						make_pair(
							DATA_CONTENT,
							DelayedEvaluationParametersMap::Field(_page->get<WebpageContent>().getCMSScript())
					)	);
					
					ParametersMap variables;
					DelayedEvaluationParametersMap depm(
						fields,
						request,
						pm,
						*_page,
						variables
					);
					
					// Display the template
					_page->getTemplate()->get<WebpageContent>().getCMSScript().display(
						stream,
						request,
						depm,
						*_page,
						variables
					);
				}
				else
				{
					_page->display(stream, request, pm);
				}
			}

			return util::ParametersMap();
		}



		bool WebPageDisplayFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageDisplayFunction::getOutputMimeType() const
		{
			if(_page)
			{
				if(_useTemplate && _page->getTemplate())
				{
					return _page->getTemplate()->getMimeType();
				}
				else
				{
					return _page->getMimeType();
				}
			}
			return "text/html";
		}



		WebPageDisplayFunction::WebPageDisplayFunction():
			_page(NULL),
			_rawData(false),
			_useTemplate(true),
			_dontRedirectIfSmartURL(false)
		{}
		


		boost::posix_time::time_duration WebPageDisplayFunction::getMaxAge() const
		{
			return _page->getMaxAge();
		}
}	}
