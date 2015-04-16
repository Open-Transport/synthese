
/** Tree Module class implementation.
	@file TreeModule.cpp

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

#include "TreeModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace tree;


	template<> const std::string util::FactorableTemplate<ModuleClass, TreeModule>::FACTORY_KEY("20_tree");

	namespace tree
	{
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<TreeModule>::NAME("Arborescences");

		template<> void ModuleClassTemplate<TreeModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<TreeModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<TreeModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<TreeModule>::End()
		{
		}



		template<> void ModuleClassTemplate<TreeModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<TreeModule>::CloseThread(
			
			){
		}
	}

	namespace tree
	{
		void TreeModule::initialize()
		{

		}
	}
}
