
/** WebPagePositionFunction class implementation.
	@file WebPagePositionFunction.cpp
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
#include "WebPage.h"
#include "WebPagePositionFunction.hpp"
#include "WebPageInterfacePage.h"
#include "PlacesListModule.h"

#include <deque>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	
	template<> const string util::FactorableTemplate<Function,cms::WebPagePositionFunction>::FACTORY_KEY("position");
	
	namespace cms
	{
		const string WebPagePositionFunction::PARAMETER_PAGE_ID("page_id");
		const string WebPagePositionFunction::PARAMETER_DISPLAY_PAGE_ID("display_page_id");
		const string WebPagePositionFunction::PARAMETER_MIN_DEPTH("min_depth");
		const string WebPagePositionFunction::PARAMETER_MAX_DEPTH("max_depth");
		
		ParametersMap WebPagePositionFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(_displayPage.get())
			{
				map.insert(PARAMETER_DISPLAY_PAGE_ID, _displayPage->getKey());
			}
			map.insert(PARAMETER_MIN_DEPTH, _minDepth);
			map.insert(PARAMETER_MAX_DEPTH, _maxDepth);
			return map;
		}

		void WebPagePositionFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = Env::GetOfficialEnv().get<WebPage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch (ObjectNotFoundException<WebPage>&)
			{
				throw RequestException("No such web page");
			}
			optional<RegistryKeyType> displayPageId(map.getOptional<RegistryKeyType>(PARAMETER_DISPLAY_PAGE_ID));
			if(displayPageId) try
			{
				_displayPage = Env::GetOfficialEnv().get<WebPage>(*displayPageId);
			}
			catch (ObjectNotFoundException<WebPage>&)
			{
				throw RequestException("No such display page");
			}
			_minDepth = map.getDefault<size_t>(PARAMETER_MIN_DEPTH, 1);
			optional<int> depth = map.getOptional<int>(PARAMETER_MAX_DEPTH);
			if(depth)
			{
				if(depth > 0)
				{
					_maxDepth = depth;
				}
				else
				{
					_maxDepth = _page->getDepth() + *depth;
				}
			}
		}

		void WebPagePositionFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			shared_ptr<const WebPage> currentPage(PlacesListModule::GetWebPage(request));
			deque<const WebPage*> pages;
			for(const WebPage* page(_page.get()); page != NULL; page = page->getParent())
			{
				pages.push_front(page);
			}
			size_t depth(0);
			bool first(true);
			BOOST_FOREACH(const WebPage* curPage, pages)
			{
				++depth;
				if(depth < _minDepth)
				{
					continue;
				}
				if(_maxDepth && depth > *_maxDepth)
				{
					break;
				}
				if(_displayPage.get())
				{
					WebPageInterfacePage::Display(stream, *_displayPage, request, *curPage, false);
				}
				else
				{
					if(!first)
					{
						stream << ",";
					}
					stream << curPage->getKey();
					first = false;
				}
			}
		}
		
		
		
		bool WebPagePositionFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPagePositionFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
