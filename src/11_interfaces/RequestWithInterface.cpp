
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

#include "HTMLForm.h"

#include "RequestWithInterface.h"
#include "Interface.h"

#include "RequestException.h"
#include "Request.h"

using namespace std;
using namespace boost;

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
		{

		}



		void RequestWithInterface::_setFromParametersMap( const ParametersMap& map )
		{
			try
			{
				// Interface
				uid id(map.getUid(PARAMETER_INTERFACE, false, "RWI"));
				if (id != UNKNOWN_VALUE)
				{
					_interface = Env::GetOfficialEnv().getRegistry<Interface>().get(id);
				}
			}
			catch (ObjectNotFoundException<Interface>& e)
			{
				throw RequestException("Specified interface not found : "+ e.getMessage());
			}
		}

		ParametersMap RequestWithInterface::_getParametersMap() const
		{
			ParametersMap map;
			if (_interface.get())
				map.insert(PARAMETER_INTERFACE, _interface->getKey());
			
			return map;
		}

		void RequestWithInterface::_copy(shared_ptr<const Function> function)
		{
			shared_ptr<const RequestWithInterface> rwi = static_pointer_cast<const RequestWithInterface, const Function>(function);
			_interface = rwi->_interface;
		}

		shared_ptr<const Interface> RequestWithInterface::getInterface() const
		{
			return _interface;
		}

		synthese::server::ParametersMap RequestWithInterface::getFixedParametersMap() const
		{
			return _getParametersMap();
		}



		void RequestWithInterface::setInterface( boost::shared_ptr<const Interface> value )
		{
			_interface = value;
		}
	}
}
