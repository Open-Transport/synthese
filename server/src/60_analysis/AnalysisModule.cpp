//////////////////////////////////////////////////////////////////////////
/// AnalysisModule class implementation.
///	@file AnalysisModule.cpp
///	@author Gaël Sauvanet
///	@date 2012
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

#include "AnalysisModule.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace analysis;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,AnalysisModule>::FACTORY_KEY("60_analysis");
	}

	namespace server
	{
		template<>
		const string ModuleClassTemplate<AnalysisModule>::NAME("Analyses");

		template<> void ModuleClassTemplate<AnalysisModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<AnalysisModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<AnalysisModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<AnalysisModule>::End()
		{
		}



		template<> void ModuleClassTemplate<AnalysisModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<AnalysisModule>::CloseThread(
			
			){
		}
	}

	namespace analysis
	{
		void AnalysisModule::initialize()
		{

		}
	}
}
