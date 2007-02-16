
/** SimplePageRequest class implementation.
	@file SimplePageRequest.cpp

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

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/DisplayableElement.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/SimplePageRequest.h"

#include "30_server/RequestException.h"

namespace synthese
{
	using namespace server;

	namespace interfaces
	{
		const std::string SimplePageRequest::PARAMETER_PAGE = "page";

		SimplePageRequest::SimplePageRequest()
			: RequestWithInterface()
		{}

		void SimplePageRequest::run( std::ostream& stream ) const
		{
			ParametersVector pv;
			for (Request::ParametersMap::const_iterator it = _parameters.begin(); it != _parameters.end(); ++it)
				pv.push_back(it->second);
			_page->display(stream, pv, NULL, this);
		}

		void SimplePageRequest::setFromParametersMap(const Request::ParametersMap& map )
		{
			RequestWithInterface::setFromParametersMap(map);

			_parameters = map;

			Request::ParametersMap::iterator it = _parameters.find(PARAMETER_PAGE);
			if (it == _parameters.end())
				throw RequestException("Page parameter not found in simple page query");
			if (_interface == NULL)
				throw RequestException("Interface was not defined");
			try
			{
				_page = _interface->getPage(it->second);
				_parameters.erase(it);
			}
			catch (InterfacePageException e)
			{
				throw RequestException("No such interface page : "+ it->second);
			}
		}

		server::Request::ParametersMap SimplePageRequest::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			return map;
		}

		void SimplePageRequest::setPage( const interfaces::InterfacePage* page )
		{
			_page = page;
		}
	}
}
