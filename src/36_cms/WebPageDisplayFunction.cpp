
/** WebPageDisplayFunction class implementation.
	@file WebPageDisplayFunction.cpp
	@author Hugues
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
#include "WebPageDisplayFunction.h"
#include "Env.h"
#include "Interface.h"
#include "Webpage.h"
#include "Website.hpp"
#include "WebPageInterfacePage.h"
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
		const string WebPageDisplayFunction::PARAMETER_PAGE_ID("p");
		const string WebPageDisplayFunction::PARAMETER_USE_TEMPLATE("use_template");
		const string WebPageDisplayFunction::PARAMETER_SMART_URL("smart_url");

		ParametersMap WebPageDisplayFunction::_getParametersMap() const
		{
			ParametersMap map(_savedParameters);
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
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
				_savedParameters.remove(PARAMETER_SMART_URL);

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

					_savedParameters.insert(_page->getSmartURLDefaultParameterName(), paths[1]);
				}
				_savedParameters.insert(PARAMETER_PAGE_ID, _page->getKey());
			}

			_useTemplate = map.getDefault<bool>(PARAMETER_USE_TEMPLATE, true);
		}

		void WebPageDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_page.get())
			{
				// If page has been fetched by its id and its smart URL is defined, then
				// redirect permanently to the smart url
				if(_smartURL.empty() && !_page->getSmartURLPath().empty())
				{
					/// @todo handle default parameter of smart url
					stringstream url;
					url << "http://" << request.getHostName() << _page->getSmartURLPath();

					ParametersMap pm(_savedParameters);
					pm.remove(PARAMETER_PAGE_ID);
					pm.remove(FunctionWithSiteBase::PARAMETER_SITE);
					string parameters(pm.getURI());
					if(!parameters.empty())
					{
						url << Request::PARAMETER_STARTER << parameters;
					}

					throw Request::RedirectException(url.str(), true);
				}

				if(_useTemplate && _page->getTemplate())
				{
					WebPageInterfacePage::Display(
						stream,
						*_page->getTemplate(),
						request,
						*_page,
						false
					);
				}
				else
				{
					_page->display(stream, request, request.getFunction()->getSavedParameters());
				}
			}
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
		_useTemplate(true)
		{

		}



		void WebPageDisplayFunction::addParameters( const server::ParametersMap& value )
		{
			BOOST_FOREACH(const ParametersMap::Map::value_type& param, value.getMap())
			{
				_savedParameters.insert(param.first, param.second);
			}
		}
}	}
