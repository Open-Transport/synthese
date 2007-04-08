
/** RequestWithInterface class implementation.
	@file RequestWithInterface.cpp

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

#include <sstream>

#include "01_util/Conversion.h"

#include "05_html/HTMLForm.h"

#include "11_interfaces/RequestWithInterface.h"
#include "11_interfaces/InterfaceModule.h"
#include "11_interfaces/RedirectInterfacePage.h"

#include "30_server/RequestException.h"
#include "30_server/Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace html;

	namespace interfaces
	{

		const std::string RequestWithInterface::PARAMETER_INTERFACE = "i";

		RequestWithInterface::RequestWithInterface()
			: Function()
			, _interface(NULL)
		{

		}


		/** @todo to be moved elsewhere because the interface is not necessarily an html interface */
		bool RequestWithInterface::_runAfterSucceededAction(ostream& stream)
		{
			if (_interface != NULL)
			{
				const RedirectInterfacePage* page = _interface->getPage<RedirectInterfacePage>();
				_request->deleteAction();
				VariablesMap vm;
				page->display(stream, vm, _request);
			}

			return true;
		}

		void RequestWithInterface::_setFromParametersMap( const ParametersMap& map )
		{
			try
			{
				// Interface
				ParametersMap::const_iterator it = map.find(PARAMETER_INTERFACE);
				if (it != map.end())
				{
					_interface = InterfaceModule::getInterfaces().get(Conversion::ToLongLong(it->second));
				}
			}
			catch (Interface::RegistryKeyException e)
			{
				throw RequestException("Specified interface not found (" + Conversion::ToString(e.getKey()) +")");
			}
		}

		ParametersMap RequestWithInterface::_getParametersMap() const
		{
			ParametersMap map;
			if (_interface != NULL)
				map.insert(make_pair(PARAMETER_INTERFACE, Conversion::ToString(_interface->getKey())));
			
			return map;
		}

		void RequestWithInterface::_copy( const Function* function)
		{
			const RequestWithInterface* rwi = dynamic_cast<const RequestWithInterface*>(function);
			_interface = rwi->_interface;
		}

		const Interface* RequestWithInterface::getInterface() const
		{
			return _interface;
		}
	}
}
