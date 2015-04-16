
/** InterfaceModule class implementation.
	@file InterfaceModule.cpp

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

#include <string>

#include "InterfaceModule.h"

#include "threads/Thread.h"

#include "InterfaceTableSync.h"
#include "Interface.h"
#include "InterfacePageTableSync.h"

#include "Env.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace interfaces;
	using namespace server;

	template<> const string util::FactorableTemplate<ModuleClass,InterfaceModule>::FACTORY_KEY("11_interfaces");

	namespace server
	{
		template<> const string ModuleClassTemplate<InterfaceModule>::NAME("Moteur d'interfaces");

		template<> void ModuleClassTemplate<InterfaceModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<InterfaceModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<InterfaceModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<InterfaceModule>::End()
		{
		}



		template<> void ModuleClassTemplate<InterfaceModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<InterfaceModule>::CloseThread(
		){
		}
	}

	namespace interfaces
	{
		std::string InterfaceModule::getVariableFromMap(const VariablesMap& variables, const std::string& varName )
		{
			VariablesMap::const_iterator it = variables.find(varName);
			if (it == variables.end())
				return "";
			return it->second;
		}
	}
}
