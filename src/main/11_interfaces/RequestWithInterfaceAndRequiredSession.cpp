
/** RequestWithInterfaceAndRequiredSession class implementation.
	@file RequestWithInterfaceAndRequiredSession.cpp

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
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/RequestWithInterfaceAndRequiredSession.h"

#include "30_server/Request.h"

namespace synthese
{
	using namespace server;

	namespace interfaces
	{


		RequestWithInterfaceAndRequiredSession::RequestWithInterfaceAndRequiredSession()
			: RequestWithInterface()
		{

		}

		bool RequestWithInterfaceAndRequiredSession::_runBeforeDisplayIfNoSession( std::ostream& stream )
		{
			_request->_setActionException(true);
			if (!_request->getErrorMessage().empty())
				_request->_setErrorMessage("Session invalide : " + _request->getErrorMessage() + ".");
			_request->_setErrorLevel(Request::REQUEST_ERROR_FATAL);
			if (_interface && !_interface->getNoSessionDefaultPageCode().empty())
			{
				try
				{
					const InterfacePage* page = _interface->getPage(_interface->getNoSessionDefaultPageCode());
					ParametersVector pv;
					VariablesMap vm;
					page->display(stream, pv, vm, NULL, _request);
				}
				catch (InterfacePageException e)
				{
					
				}
			}
			return true;			
		}
	}
}
