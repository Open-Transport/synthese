
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
#include "01_util/Html.h"

#include "11_interfaces/RequestWithInterface.h"
#include "11_interfaces/InterfaceModule.h"
#include "11_interfaces/RedirectInterfacePage.h"

#include "30_server/RequestException.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace interfaces
	{

		const std::string RequestWithInterface::PARAMETER_INTERFACE = "i";

		RequestWithInterface::RequestWithInterface()
			: Request()
			, _interface(NULL)
		{

		}


		bool RequestWithInterface::runAfterAction(ostream& stream)
		{
			_action = NULL;

			if (_interface != NULL)
			{
				const RedirectInterfacePage* page = _interface->getPage<RedirectInterfacePage>();
				page->display(stream, this);
			}

			return true;
		}

		void RequestWithInterface::setFromParametersMap( const ParametersMap& map )
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

		RequestWithInterface::ParametersMap RequestWithInterface::getParametersMap() const
		{
			ParametersMap map;
			if (_interface != NULL)
				map.insert(make_pair(PARAMETER_INTERFACE, Conversion::ToString(_interface->getKey())));
			return map;
		}

		void RequestWithInterface::copy( const Request* request )
		{
			Request::copy(request);
			const RequestWithInterface* requestWithInterface;
			if (requestWithInterface = dynamic_cast<const RequestWithInterface*>(request))
				_interface = requestWithInterface->_interface;
		}

		std::string RequestWithInterface::getHTMLFormHeader( const std::string& name ) const
		{
			stringstream s;
			s << Request::getHTMLFormHeader(name);
			if (_interface != NULL)
				s << Html::getHiddenInput(PARAMETER_INTERFACE, Conversion::ToString(_interface->getKey()));
			return s.str();
		}

		std::string RequestWithInterface::getQueryString() const
		{
			stringstream s;
			s << Request::getQueryString();
			if (_interface != NULL)
				s << PARAMETER_SEPARATOR << PARAMETER_INTERFACE << PARAMETER_ASSIGNMENT << _interface->getKey();
			return s.str();
		}
	}
}
