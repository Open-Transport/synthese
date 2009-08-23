
/** SimplePageFunction class implementation.
	@file SimplePageFunction.cpp

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

#include "SimplePageFunction.h"

#include "Interface.h"
#include "InterfacePage.h"
#include "InterfacePageException.h"
#include "NonPredefinedInterfacePage.h"
#include "RequestException.h"
#include "Factory.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<> const string util::FactorableTemplate<interfaces::RequestWithInterface, interfaces::SimplePageFunction>::FACTORY_KEY("page");

	namespace interfaces
	{
		const string SimplePageFunction::PARAMETER_PAGE_CLASS("pcl");
		const string SimplePageFunction::PARAMETER_PAGE = "page";

		void SimplePageFunction::_run( ostream& stream ) const
		{
			if (_page == NULL)
				return;

			ParametersVector pv;
			const ParametersMap::Map& pmap(_parameters.getMap());
			for (ParametersMap::Map::const_iterator it = pmap.begin(); it != pmap.end(); ++it)
				pv.push_back(it->second);
			    

			VariablesMap vm;

			_page->_display(stream, pv, vm, NULL, _request);
		}

		void SimplePageFunction::_setFromParametersMap(const ParametersMap& map )
		{
			RequestWithInterface::_setFromParametersMap(map);

			_parameters = map;

			string classKey(_parameters.getString(PARAMETER_PAGE_CLASS,false,FACTORY_KEY));
			string key(_parameters.getString(PARAMETER_PAGE,false,FACTORY_KEY));

			if (classKey.empty())
				classKey = NonPredefinedInterfacePage::FACTORY_KEY;
			
			if (!_interface.get())
				throw RequestException("Interface was not defined");

			try
			{
				_page = _interface->getPage(classKey,key);
			}
			catch (InterfacePageException& e)
			{
				throw RequestException("No such interface page : "+ classKey + "/" + key);
			}

			if (!_page->getDirectDisplayAllowed())
				throw RequestException("Forbidden page : "+ classKey + "/" + key);
		}

		ParametersMap SimplePageFunction::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterface::_getParametersMap());

			if (_page != NULL)
			{
				map.insert(PARAMETER_PAGE, _page->getPageCode());
				map.insert(PARAMETER_PAGE_CLASS, _page->getFactoryKey());
			}
			return map;
		}

		void SimplePageFunction::setPage(const InterfacePage* page )
		{
			_page = page;
		}

		bool SimplePageFunction::_runBeforeDisplayIfNoSession( ostream& stream )
		{
			return false;
		}



		bool SimplePageFunction::_isAuthorized(
		) const {
			return true;
		}

		std::string SimplePageFunction::getOutputMimeType() const
		{
			return (_page == NULL) ? "text/plain" : _page->getMimeType();
		}
	}
}
