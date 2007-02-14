
/** RedirRequest class implementation.
	@file RedirRequest.cpp

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

#include "01_util/Conversion.h"

#include "11_interfaces/RedirectInterfacePage.h"
#include "11_interfaces/Interface.h"

#include "30_server/RequestException.h"

#include "RedirRequest.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace interfaces
	{
		const std::string RedirRequest::PARAMETER_URL = "url";
		
		RedirRequest::RedirRequest()
			: RequestWithInterface()
		{}

		Request::ParametersMap RedirRequest::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_URL, _url));
			return map;
		}

		void RedirRequest::setFromParametersMap(const Request::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it;

			it = map.find(PARAMETER_URL);
			if (it == map.end())
				throw RequestException("URL to redirect not specified");
		}

		void RedirRequest::run( std::ostream& stream ) const
		{
			try
			{
				const RedirectInterfacePage* page = _interface->getPage<RedirectInterfacePage>();
				page->display(stream, _url, this);
			}
			catch (Exception e)
			{
				
			}
		}

		RedirRequest::~RedirRequest()
		{
		}

		void RedirRequest::setUrl( const std::string& url )
		{
			_url = url;
		}

		const std::string& RedirRequest::getURL() const
		{
			return _url;
		}
	}
}
