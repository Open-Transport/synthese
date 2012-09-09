
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

#include "RequestException.h"
#include "Request.h"
#include "WebPageDisplayFunction.h"
#include "Env.h"
#include "Interface.h"
#include "Webpage.h"
#include "Website.hpp"
#include "Action.h"
#include "CMSModule.hpp"

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
	using namespace interfaces;
	using namespace cms;

	template<> const string util::FactorableTemplate<FunctionWithSite<false>,WebPageDisplayFunction>::FACTORY_KEY("page");

	namespace cms
	{
		const string WebPageDisplayFunction::DATA_CONTENT = "content";

		const string WebPageDisplayFunction::PARAMETER_PAGE_ID = "p";
		const string WebPageDisplayFunction::PARAMETER_SITE_ID = "si";
		const string WebPageDisplayFunction::PARAMETER_USE_TEMPLATE = "use_template";
		const string WebPageDisplayFunction::PARAMETER_HOST_NAME = "host_name";
		const string WebPageDisplayFunction::PARAMETER_SMART_URL = "smart_url";
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
					throw RequestException("Webpage not defined");
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
				}

				// Page
				if(_smartURL[0] == ':')
				{
					throw RequestException("Smart URLs starting with a colon are not allowed for direct access");
				}
				_page = site->getPageBySmartURL(_smartURL);

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
			}

			_useTemplate = map.getDefault<bool>(PARAMETER_USE_TEMPLATE, true);

			// Do not redirect if smart URL
			_dontRedirectIfSmartURL = map.getDefault<bool>(PARAMETER_DONT_REDIRECT_IF_SMART_URL, false);
			if(_dontRedirectIfSmartURL)
			{
				_templateParameters.insert(PARAMETER_DONT_REDIRECT_IF_SMART_URL, _dontRedirectIfSmartURL);
			}
		}



		util::ParametersMap WebPageDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_page)
			{
				// If page has been fetched by its id and its smart URL is defined, then
				// redirect permanently to the smart url, unless it starts with a colon (used for aliases).
				if(!_dontRedirectIfSmartURL && _smartURL.empty() && !_page->get<SmartURLPath>().empty() && _page->get<SmartURLPath>()[0] != ':')
				{
					/// @todo handle default parameter of smart url
					stringstream url;
					url << "http://" << request.getHostName() << _page->get<SmartURLPath>();

					ParametersMap pm(getTemplateParameters());
					pm.remove(PARAMETER_PAGE_ID);
					pm.remove(FunctionWithSiteBase::PARAMETER_SITE);
					stringstream uri;
					pm.outputURI(uri);
					string parameters(uri.str());
					if(!parameters.empty())
					{
						url << Request::PARAMETER_STARTER << parameters;
					}

					throw Request::RedirectException(url.str(), true);
				}

				if(_useTemplate && _page->getTemplate())
				{
					ParametersMap pm(getTemplateParameters());

					// Page data
					_page->toParametersMap(pm);

					// Generated content
					stringstream content;
					_page->display(content, request, pm);
					pm.insert(DATA_CONTENT, content.str());

					_page->getTemplate()->display(stream, request, pm);
				}
				else
				{
					_page->display(stream, request, getTemplateParameters());
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
			_useTemplate(true),
			_dontRedirectIfSmartURL(false)
		{}
}	}
