
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

		const string WebPageDisplayFunction::PARAMETER_PAGE_ID("p");
		const string WebPageDisplayFunction::PARAMETER_USE_TEMPLATE("use_template");
		const string WebPageDisplayFunction::PARAMETER_SMART_URL("smart_url");
		const string WebPageDisplayFunction::PARAMETER_DONT_REDIRECT_IF_SMART_URL = "dont_redirect_if_smart_url";

		ParametersMap WebPageDisplayFunction::_getParametersMap() const
		{
			ParametersMap map(getTemplateParameters());

			// Page
			if(_page.get())
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
			_FunctionWithSite::_setFromParametersMap(map);

			if(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID))
			{
				try
				{
					_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
				}
				catch (ObjectNotFoundException<Webpage>)
				{
					throw RequestException("No such page");
				}
				if(	!_page->mustBeDisplayed()
				){
					throw Request::ForbiddenRequestException();
				}
			}
			else
			{
				_smartURL = map.get<string>(PARAMETER_SMART_URL);
				if(_smartURL.empty() || !getSite().get())
				{
					throw RequestException("Smart URL and site, or page ID must be specified");
				}
				_templateParameters.remove(PARAMETER_SMART_URL);
				if(_smartURL[0] == ':')
				{
					throw RequestException("Smart URLs starting with a colon are not allowed for direct access");
				}

				_page = Env::GetOfficialEnv().getSPtr(getSite()->getPageBySmartURL(_smartURL));
				if(!_page.get())
				{ // Attempt to find a page with a parameter
					vector<string> paths;
					iter_split(paths, _smartURL, last_finder("/"));
					if(paths.size() != 2)
					{
						throw Request::NotFoundException();
					}

					_page = Env::GetOfficialEnv().getSPtr(getSite()->getPageBySmartURL(paths[0]));

					if(!_page.get() || _page->getSmartURLDefaultParameterName().empty())
					{
						throw Request::NotFoundException();
					}

					_templateParameters.insert(_page->getSmartURLDefaultParameterName(), paths[1]);
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
			if(_page.get())
			{
				// If page has been fetched by its id and its smart URL is defined, then
				// redirect permanently to the smart url, unless it starts with a colon (used for aliases).
				if(!_dontRedirectIfSmartURL && _smartURL.empty() && !_page->getSmartURLPath().empty() && _page->getSmartURLPath()[0] != ':')
				{
					/// @todo handle default parameter of smart url
					stringstream url;
					url << "http://" << request.getHostName() << _page->getSmartURLPath();

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
			if(_page.get())
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
			_useTemplate(true),
			_dontRedirectIfSmartURL(false)
		{}
}	}
