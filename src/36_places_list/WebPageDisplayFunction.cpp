
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
#include "WebPage.h"
#include "Site.h"
#include "WebPageInterfacePage.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace interfaces;
	using namespace transportwebsite;

	template<> const string util::FactorableTemplate<FunctionWithSite,WebPageDisplayFunction>::FACTORY_KEY("WebPageDisplayFunction");
	
	namespace transportwebsite
	{
		ParametersMap WebPageDisplayFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSite::_getParametersMap());
			if(_page.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _page->getKey());
			}
			return map;
		}

		void WebPageDisplayFunction::_setFromParametersMap(const ParametersMap& map)
		{
			FunctionWithSite::_setFromParametersMap(map);
			try
			{
				_page = Env::GetOfficialEnv().get<WebPage>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (...)
			{
				throw RequestException("No such page");
			}

			if(_page->getSite() != _site.get())
			{
				throw RequestException("Inconsistent page");
			}

			if(_site->getInterface() == NULL || !_site->getInterface()->hasPage<WebPageInterfacePage>())
			{
				throw RequestException("Inappropriate site");
			}

		}

		void WebPageDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			VariablesMap variables;
			_site->getInterface()->getPage<WebPageInterfacePage>()->display(stream, *_page, variables, &request);

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
	



		void WebPageDisplayFunction::setPage( boost::shared_ptr<const WebPage> value )
		{
			_page = value;
		}



		boost::shared_ptr<const WebPage> WebPageDisplayFunction::getPage() const
		{
			return _page;
		}
	}
}
