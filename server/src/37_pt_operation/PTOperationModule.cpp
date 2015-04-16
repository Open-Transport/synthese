
/** 37_pt_operation module class implementation.
	@file PTOperationModule.cpp
	@author Hugues Romain
	@date 2011
	@since 3.2.1

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

#include "PTOperationModule.hpp"
#include "Vehicle.hpp"
#include "ServiceComposition.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace pt_operation;
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,PTOperationModule>::FACTORY_KEY("37_pt_operation");
	}

	namespace graph
	{
		template<> const GraphIdType GraphModuleTemplate<PTOperationModule>::GRAPH_ID(3);
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<PTOperationModule>::NAME("Exploitation transport public");

		template<> void ModuleClassTemplate<PTOperationModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<PTOperationModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<PTOperationModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<PTOperationModule>::End()
		{
		}



		template<> void ModuleClassTemplate<PTOperationModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<PTOperationModule>::CloseThread(
			
			){
		}
	}

	namespace pt_operation
	{
}	}
