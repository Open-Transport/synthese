
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
#include "11_interfaces/RedirRequest.h"
#include "RequestMissingParameterException.h"
#include "30_server/RequestException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<> const string util::FactorableTemplate<interfaces::RequestWithInterface,interfaces::RedirRequest>::FACTORY_KEY("redir");

	namespace interfaces
	{
		const std::string RedirRequest::PARAMETER_URL = "url";
		
		ParametersMap RedirRequest::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterface::_getParametersMap());

			map.insert(PARAMETER_URL, _url);
			return map;
		}

		void RedirRequest::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterface::_setFromParametersMap(map);

			_url = map.getString(PARAMETER_URL, true, FACTORY_KEY);
		}

		void RedirRequest::_run( std::ostream& stream ) const
		{
			try
			{
				const RedirectInterfacePage* page = _interface->getPage<RedirectInterfacePage>();
				VariablesMap vm;
				page->display(stream, vm, _url, _request);
			}
			catch (Exception& e)
			{
				
			}
		}

		void RedirRequest::setRedirURL( const std::string& url )
		{
			_url = url;
		}

		const std::string& RedirRequest::getRedirURL() const
		{
			return _url;
		}



		bool RedirRequest::_isAuthorized(
		) const {
			return true;
		}

		std::string RedirRequest::getOutputMimeType() const
		{
			return _interface->getPage<RedirectInterfacePage>()->getMimeType();
		}
	}
}
