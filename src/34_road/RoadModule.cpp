
/** RoadModule class implementation.
	@file RoadModule.cpp

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

#include "RoadModule.h"

using namespace std;

namespace synthese
{
	using namespace road;
	using namespace server;

	namespace graph
	{
		template<> const GraphIdType GraphModuleTemplate<RoadModule>::GRAPH_ID(1);
	}

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,RoadModule>::FACTORY_KEY("34_road");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<RoadModule>::NAME("Voirie");
		
		template<> void ModuleClassTemplate<RoadModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<RoadModule>::Init()
		{
		}
		
		template<> void ModuleClassTemplate<RoadModule>::End()
		{
		}
	}

	namespace road
	{
	}
}