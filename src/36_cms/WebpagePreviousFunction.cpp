
/** WebpagePreviousFunction class implementation.
	@file WebpagePreviousFunction.cpp
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
#include "WebpagePreviousFunction.hpp"
#include "Webpage.h"
#include "CMSModule.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::WebpagePreviousFunction>::FACTORY_KEY("previous");

	namespace cms
	{
		const string WebpagePreviousFunction::PARAMETER_PAGE_ID("page_id");

		ParametersMap WebpagePreviousFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_webpage.get())
			{
				map.insert(PARAMETER_PAGE_ID, _webpage->getKey());
			}
			return map;
		}

		void WebpagePreviousFunction::_setFromParametersMap(const ParametersMap& map)
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

		util::ParametersMap WebpagePreviousFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			const Webpage* page(_webpage.get() ? _webpage.get() : CMSModule::GetWebPage(request));
			Webpage* previous(page->getPreviousSibling());
			if(previous)
			{
				stream << previous->getKey();
			}
			return util::ParametersMap();
		}



		bool WebpagePreviousFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebpagePreviousFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
