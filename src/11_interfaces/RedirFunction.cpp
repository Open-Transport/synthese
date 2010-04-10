
/** RedirFunction class implementation.
	@file RedirFunction.cpp

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

#include "Interface.h"
#include "RedirFunction.h"
#include "RequestException.h"
#include "Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<> const string util::FactorableTemplate<interfaces::RequestWithInterface,interfaces::RedirFunction>::FACTORY_KEY("redir");

	namespace interfaces
	{
		const std::string RedirFunction::PARAMETER_URL = "url";
		
		ParametersMap RedirFunction::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterface::_getParametersMap());

			map.insert(PARAMETER_URL, _url);
			return map;
		}

		void RedirFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterface::_setFromParametersMap(map);

			_url = map.get<string>(PARAMETER_URL);
		}

		void RedirFunction::run( std::ostream& stream, const Request& request ) const
		{
			throw Request::RedirectException(_url);
		}

		void RedirFunction::setRedirURL( const std::string& url )
		{
			_url = url;
		}

		const std::string& RedirFunction::getRedirURL() const
		{
			return _url;
		}



		bool RedirFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string RedirFunction::getOutputMimeType() const
		{
			return string();
		}
	}
}
