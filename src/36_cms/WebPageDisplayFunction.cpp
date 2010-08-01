
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
			ParametersMap map(_aditionnalParameters);
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			return map;
		}

		void WebPageDisplayFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_FunctionWithSite::_setFromParametersMap(map);

			_aditionnalParameters = map;
			_aditionnalParameters.remove(Request::PARAMETER_ACTION);
			BOOST_FOREACH(const ParametersMap::Map::value_type& it, map.getMap())
			{
				if(	it.first != Request::PARAMETER_FUNCTION &&
					it.first != Request::PARAMETER_ACTION &&
					(it.first.size() < Action_PARAMETER_PREFIX.size() || it.first.substr(0, Action_PARAMETER_PREFIX.size()) != Action_PARAMETER_PREFIX)
				){
					_aditionnalParameters.insert(it.first, it.second);
				}
			}

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

				_page = Env::GetOfficialEnv().getSPtr(getSite()->getPageBySmartURL(_smartURL));
				WebPageDisplayFunction f;
				f._aditionnalParameters = _aditionnalParameters;
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

					_aditionnalParameters.insert(_page->getSmartURLDefaultParameterName(), paths[1]);
				}
			}

			_useTemplate = map.getDefault<bool>(PARAMETER_USE_TEMPLATE, true);
		}

		void WebPageDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_page.get())
			{
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
					_page->display(stream, request);
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
			return "text/html";
		}



		WebPageDisplayFunction::WebPageDisplayFunction():
		_useTemplate(true)
		{

		}
	}
}
