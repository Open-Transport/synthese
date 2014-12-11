//////////////////////////////////////////////////////////////////////////
/// RegulationModule class implementation.
///	@file RegulationModule.cpp
///	@author Marc Jambert
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "RegulationModule.hpp"
#include "AlertProcessingThread.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace regulation;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,RegulationModule>::FACTORY_KEY("62_regulation");
	}

	namespace server
	{
		template<>
		const string ModuleClassTemplate<RegulationModule>::NAME("Regulation");

		template<> void ModuleClassTemplate<RegulationModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<RegulationModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<RegulationModule>::Start()
		{
            util::Log::GetInstance().info("************************* pouetStart");
            static AlertProcessingThread thread;
            thread.start();
		}

		template<> void ModuleClassTemplate<RegulationModule>::End()
		{
		}



		template<> void ModuleClassTemplate<RegulationModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<RegulationModule>::CloseThread(
			
			){
		}
	}

	namespace regulation
	{
		void RegulationModule::initialize()
		{
            util::Log::GetInstance().info("************************* pouet");
            static AlertProcessingThread thread;
            thread.start();
            
		}
	}
}
