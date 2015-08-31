//////////////////////////////////////////////////////////////////////////
/// SpikeModule class implementation.
///	@file SpikeModule.cpp
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

#include "SpikeModule.hpp"

#include "01_util/threads/Thread.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace spike;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,SpikeModule>::FACTORY_KEY("99_spike");
	}

	namespace server
	{
		template<>
		const string ModuleClassTemplate<SpikeModule>::NAME("Spike");

		template<> void ModuleClassTemplate<SpikeModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<SpikeModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<SpikeModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<SpikeModule>::End()
		{
		}



		template<> void ModuleClassTemplate<SpikeModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<SpikeModule>::CloseThread(
			
			){
		}
	}

	namespace spike
	{

		void SpikeModule::initialize()
		{
		}
	}
}
