
/** RequestWithInterface class implementation.
	@file RequestWithInterface.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_INTERFACE, 0));
				if (id > 0)
				{
					_interface = Env::GetOfficialEnv().get<Interface>(id);
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

		void RequestWithInterface::_copy(const Function& function)
		{
			const RequestWithInterface* rwi(dynamic_cast<const RequestWithInterface*>(&function));
			if(rwi)
			{
				_interface = rwi->_interface;
			}
		}

		boost::shared_ptr<const Interface> RequestWithInterface::getInterface() const
		{
			return _interface;
		}



		void RequestWithInterface::setInterface( boost::shared_ptr<const Interface> value )
		{
			_interface = value;
		}
	}
}
