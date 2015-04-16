
/** WebpageNextFunction class implementation.
	@file WebpageNextFunction.cpp
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
#include "WebpageNextFunction.hpp"
#include "CMSModule.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::WebpageNextFunction>::FACTORY_KEY("next");

	namespace cms
	{
		const string WebpageNextFunction::PARAMETER_PAGE_ID("page_id");

		ParametersMap WebpageNextFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_webpage.get())
			{
				map.insert(PARAMETER_PAGE_ID, _webpage->getKey());
			}
			return map;
		}

		void WebpageNextFunction::_setFromParametersMap(const ParametersMap& map)
		{
			optional<RegistryKeyType> pageId(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(pageId) try
			{
				_webpage = Env::GetOfficialEnv().get<Webpage>(*pageId);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}
		}

		util::ParametersMap WebpageNextFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			const Webpage* page(_webpage.get() ? _webpage.get() : CMSModule::GetWebPage(request));
			Webpage* next(page->getNextSibling());
			if(next)
			{
				stream << next->getKey();
			}
			return util::ParametersMap();
		}



		bool WebpageNextFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebpageNextFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
