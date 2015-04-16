
/** WebPageLinksFunction class implementation.
	@file WebPageLinksFunction.cpp
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
#include "WebPageLinksFunction.hpp"
#include "AdminParametersException.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace admin;

	template<> const string util::FactorableTemplate<Function,cms::WebPageLinksFunction>::FACTORY_KEY("links");

	namespace cms
	{
		const string WebPageLinksFunction::PARAMETER_PAGE_ID("page_id");
		const string WebPageLinksFunction::PARAMETER_DISPLAY_TEMPLATE_ID("display_page_id");

		ParametersMap WebPageLinksFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(_displayPage.get())
			{
				map.insert(PARAMETER_DISPLAY_TEMPLATE_ID, _displayPage->getKey());
			}
			return map;
		}

		void WebPageLinksFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw AdminParametersException("No such page");
			}
			try
			{
				_displayPage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_DISPLAY_TEMPLATE_ID));
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw AdminParametersException("No such template page");
			}
		}



		util::ParametersMap WebPageLinksFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			size_t number(0);
			BOOST_FOREACH(const WebpageLinks::Type::value_type& link, _page->get<WebpageLinks>())
			{
				if(!link->mustBeDisplayed())
				{
					continue;
				}

				if(_displayPage.get())
				{
					ParametersMap pm(getTemplateParameters());
					link->toParametersMap(pm, true);
					_displayPage->display(stream, request, pm);
				}
				++number;
			}

			if(!_displayPage.get())
			{
				stream << number;
			}

			return util::ParametersMap();
		}



		bool WebPageLinksFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageLinksFunction::getOutputMimeType() const
		{
			return _displayPage.get() ? _displayPage->getMimeType() : "text/plain";
		}
}	}
