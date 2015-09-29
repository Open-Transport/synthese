
/** WebPagePositionFunction class implementation.
	@file WebPagePositionFunction.cpp
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
#include "Webpage.h"
#include "WebPagePositionFunction.hpp"
#include "CMSModule.hpp"

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
		const string WebPagePositionFunction::PARAMETER_RAW_DATA = "raw_data";

		const string WebPagePositionFunction::TAG_PAGE = "page";



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
			map.insert(PARAMETER_MAX_DEPTH, _maxDepth.get_value_or(0));
			return map;
		}



		void WebPagePositionFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Raw data
			_rawData = map.getDefault<bool>(PARAMETER_RAW_DATA, false);

			// Page
			try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such web page");
			}

			// Display page
			if(!_rawData)
			{
				optional<RegistryKeyType> displayPageId(map.getOptional<RegistryKeyType>(PARAMETER_DISPLAY_PAGE_ID));
				if(displayPageId) try
				{
					_displayPage = Env::GetOfficialEnv().get<Webpage>(*displayPageId);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such display page");
				}
			}

			// Min depth
			_minDepth = map.getDefault<size_t>(PARAMETER_MIN_DEPTH, 1);

			// Max depth
			optional<int> depth = map.getOptional<int>(PARAMETER_MAX_DEPTH);
			if(depth)
			{
				if(*depth > 0)
				{
					_maxDepth = depth;
				}
				else
				{
					_maxDepth = _page->getDepth() + *depth;
				}
			}
		}



		util::ParametersMap WebPagePositionFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			// Get the tree branch
			deque<const Webpage*> pages;
			for(const Webpage* page(_page.get()); page != NULL; page = page->getParent())
			{
				pages.push_front(page);
			}

			// Populate the parameters map
			size_t depth(0);
			bool first(true);
			BOOST_FOREACH(const Webpage* curPage, pages)
			{
				// Jump or break depending on the depth
				++depth;
				if(depth < _minDepth)
				{
					continue;
				}
				if(_maxDepth && depth > *_maxDepth)
				{
					break;
				}

				// Export the page
				if(_rawData)
				{
					boost::shared_ptr<ParametersMap> pagePM(new ParametersMap);
					curPage->toParametersMap(*pagePM, true, false);
					pm.insert(TAG_PAGE, pagePM);
				}
				else if(_displayPage.get())
				{
					ParametersMap pagePM(getTemplateParameters());
					curPage->toParametersMap(pagePM, true, false);
					_displayPage->display(stream, request, pagePM);
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

			return pm;
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



		WebPagePositionFunction::WebPagePositionFunction():
			_minDepth(1),
			_rawData(false)
		{}
}	}
