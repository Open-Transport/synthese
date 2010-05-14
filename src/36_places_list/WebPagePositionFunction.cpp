
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
#include "HTMLModule.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
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
	using namespace html;

	template<> const string util::FactorableTemplate<Function,transportwebsite::WebPagePositionFunction>::FACTORY_KEY("position");
	
	namespace transportwebsite
	{
		const string WebPagePositionFunction::PARAMETER_PAGE_ID("page_id");
		const string WebPagePositionFunction::PARAMETER_MIN_DEPTH("min_depth");
		const string WebPagePositionFunction::PARAMETER_MAX_DEPTH("max_depth");
		const string WebPagePositionFunction::PARAMETER_BEGINNING("beginning");
		const string WebPagePositionFunction::PARAMETER_ENDING("ending");
		const string WebPagePositionFunction::PARAMETER_BEGINNING_SELECTED("beginning_selected");
		const string WebPagePositionFunction::PARAMETER_ENDING_SELECTED("ending_selected");

		ParametersMap WebPagePositionFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			map.insert(PARAMETER_MIN_DEPTH, _minDepth);
			map.insert(PARAMETER_MAX_DEPTH, _maxDepth);
			map.insert(PARAMETER_BEGINNING, _beginning);
			map.insert(PARAMETER_BEGINNING_SELECTED, _beginningSelected);
			map.insert(PARAMETER_ENDING, _ending);
			map.insert(PARAMETER_ENDING_SELECTED, _endingSelected);
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
			_minDepth = map.getDefault<size_t>(PARAMETER_MIN_DEPTH, 1);
			_maxDepth = map.getOptional<size_t>(PARAMETER_MAX_DEPTH);
			_beginning = map.getDefault<string>(PARAMETER_BEGINNING);
			_beginningSelected = map.getDefault<string>(PARAMETER_BEGINNING_SELECTED, _beginning);
			_ending = map.getDefault<string>(PARAMETER_ENDING);
			_endingSelected = map.getDefault<string>(PARAMETER_ENDING_SELECTED, _ending);
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
			size_t depth(1);
			StaticFunctionRequest<WebPageDisplayFunction> openPage(request, false);
			BOOST_FOREACH(const WebPage* curPage, pages)
			{
				if(depth < _minDepth)
				{
					continue;
				}
				if(_maxDepth && depth > *_maxDepth)
				{
					break;
				}
				openPage.getFunction()->setPage(Env::GetOfficialEnv().getSPtr(curPage));
				stream <<
					(curPage == currentPage.get() ? _beginningSelected : _beginning) <<
					HTMLModule::getHTMLLink(openPage.getURL(), curPage->getName()) <<
					(curPage == currentPage.get() ? _endingSelected : _ending);
				++depth;
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
