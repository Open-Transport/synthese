
/** HikingModule class implementation.
	@file HikingModule.cpp

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

#include "HikingModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace hiking;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,HikingModule>::FACTORY_KEY("58_hiking");
	}

	namespace server
	{
		template<>
		const string ModuleClassTemplate<HikingModule>::NAME("Randonnées");

		template<> void ModuleClassTemplate<HikingModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<HikingModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<HikingModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<HikingModule>::End()
		{
		}



		template<> void ModuleClassTemplate<HikingModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<HikingModule>::CloseThread(
			
			){
		}
	}

	namespace hiking
	{
		void HikingModule::initialize()
		{

		}
	}
}
