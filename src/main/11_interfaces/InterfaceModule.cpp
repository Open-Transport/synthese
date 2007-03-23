
/** InterfaceModule class implementation.
	@file InterfaceModule.cpp

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

#include <string>

#include "01_util/Thread.h"

#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteSync.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfaceTableSync.h"
#include "11_interfaces/InterfacePageTableSync.h"
#include "11_interfaces/InterfaceModule.h"

using namespace std;

namespace synthese
{
	using namespace db;

	namespace interfaces
	{
		Interface::Registry	InterfaceModule::_interfaces;

		void InterfaceModule::initialize()
		{
		}

		Interface::Registry& 
			InterfaceModule::getInterfaces ()
		{
			return _interfaces;
		}

		map<uid, std::string> InterfaceModule::getInterfaceLabels()
		{
			map<uid, string> m;
			for (Interface::Registry::const_iterator it = _interfaces.begin(); it != _interfaces.end(); ++it)
				m.insert(make_pair(it->first, it->second->getName()));
			return m;
		}

		std::string InterfaceModule::getVariableFromMap(const VariablesMap& variables, const std::string& varName )
		{
			VariablesMap::const_iterator it = variables.find(varName);
			if (it == variables.end())
				return "";
			return it->second;
		}
	}
}

