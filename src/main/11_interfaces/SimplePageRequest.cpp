
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

#include "SimplePageRequest.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/InterfacePageException.h"

#include "30_server/RequestException.h"

#include "01_util/Factory.h"

using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace interfaces
	{
		const std::string SimplePageRequest::PARAMETER_PAGE = "page";

		void SimplePageRequest::_run( std::ostream& stream ) const
		{
			if (_page == NULL)
				return;

			ParametersVector pv;
			for (ParametersMap::const_iterator it = _parameters.begin(); it != _parameters.end(); ++it)
				pv.push_back(it->second);
			    

			VariablesMap vm;

			_page->display(stream, pv, vm, NULL, _request);
		}

		void SimplePageRequest::_setFromParametersMap(const ParametersMap& map )
		{
			RequestWithInterface::_setFromParametersMap(map);

			_parameters = map;

			ParametersMap::iterator it = _parameters.find(PARAMETER_PAGE);
			if (it == _parameters.end() || it->second.empty())
				return;

			if (!_interface.get())
				throw RequestException("Interface was not defined");

			// Drop registered pages
			if (Factory<InterfacePage>::contains(it->second))
				throw RequestException("Forbidden interface page");

			try
			{
				_page = _interface->getPage(it->second);
				_parameters.erase(it);
			}
			catch (InterfacePageException& e)
			{
				throw RequestException("No such interface page : "+ it->second);
			}
		}

		ParametersMap SimplePageRequest::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterface::_getParametersMap());

			if (_page != NULL)
				map.insert(make_pair(PARAMETER_PAGE, _page->getCode()));
			
			return map;
		}

		void SimplePageRequest::setPage(shared_ptr<const InterfacePage> page )
		{
			_page = page;
		}

		bool SimplePageRequest::_runBeforeDisplayIfNoSession( std::ostream& stream )
		{
			return false;
		}
	}
}
