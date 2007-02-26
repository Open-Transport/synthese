
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

namespace synthese
{
	namespace interfaces
	{


		RequestWithInterfaceAndRequiredSession::RequestWithInterfaceAndRequiredSession()
			: RequestWithInterface()
		{

		}

		bool RequestWithInterfaceAndRequiredSession::runBeforeDisplayIfNoSession( std::ostream& stream )
		{
			_actionException = true;
			if (_errorMessage.size())
				_errorMessage = "Session invalide : " + _errorMessage + ".";
			_errorLevel = REQUEST_ERROR_FATAL;
			if (_interface && _interface->getNoSessionDefaultPageCode().size())
			{
				try
				{
					const InterfacePage* page = _interface->getPage(_interface->getNoSessionDefaultPageCode());
					ParametersVector pv;
					page->display(stream, pv, NULL, this);
				}
				catch (InterfacePageException e)
				{
					
				}
			}
			return true;			
		}
	}
}
