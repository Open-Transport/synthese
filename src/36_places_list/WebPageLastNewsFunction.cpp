
/** WebPageLastNewsFunction class implementation.
	@file WebPageLastNewsFunction.cpp
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
#include "WebPageLastNewsFunction.hpp"
#include "WebPage.h"
#include "WebPageTableSync.h"
#include "PlacesListModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,transportwebsite::WebPageLastNewsFunction>::FACTORY_KEY("last_news");
	
	namespace transportwebsite
	{
		const string WebPageLastNewsFunction::PARAMETER_ROOT_ID("root");
		const string WebPageLastNewsFunction::PARAMETER_DISPLAY_PAGE_ID("display_page");
		const string WebPageLastNewsFunction::PARAMETER_MAX_NUMBER("number");
		
		ParametersMap WebPageLastNewsFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_root.get())
			{
				map.insert(PARAMETER_ROOT_ID, _root->getKey());
				if(_number)
				{
					map.insert(PARAMETER_MAX_NUMBER, *_number);
				}
			}
			return map;
		}

		void WebPageLastNewsFunction::_setFromParametersMap(const ParametersMap& map)
		{
			optional<RegistryKeyType> rootId(map.get<RegistryKeyType>(PARAMETER_ROOT_ID));
			if(rootId)
			try
			{
				_root = Env::GetOfficialEnv().get<WebPage>(*rootId);
			}
			catch (ObjectNotFoundException<WebPage>&)
			{
				throw RequestException("No such root page");
			}

			_number = map.getOptional<size_t>(PARAMETER_MAX_NUMBER);
		}

		void WebPageLastNewsFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			shared_ptr<const Site> site(PlacesListModule::GetSite(request));

			WebPageTableSync::SearchResult pages(
				WebPageTableSync::Search(
					Env::GetOfficialEnv(),
					_root.get() ? optional<RegistryKeyType>() : site->getKey(),
					_root.get() ? _root->getKey() : optional<RegistryKeyType>(0)
			)	);
			BOOST_FOREACH(shared_ptr<WebPage> page, pages)
			{
				stream << page->getAbstract();
			}

		}
		
		
		
		bool WebPageLastNewsFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageLastNewsFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
