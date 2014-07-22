/** SCOMModule class implementation.
	@file SCOMModule.cpp

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

#include "SCOMModule.h"

#include "ModuleClass.h"
#include "FactorableTemplate.h"

namespace synthese
{
	using namespace util;
	using namespace scom;
	using namespace server;

	// Here comes the official name of the module
	template<> const std::string FactorableTemplate<ModuleClass,SCOMModule>::FACTORY_KEY("62_scom");

	namespace scom
	{
		// Here comes the variables, class and constants specific to the module
	}

	namespace server
	{
		// Here comes a readable name for the module, in French
		template<> const std::string ModuleClassTemplate<SCOMModule>::NAME("Temps rel SCOM et bornes IP");

		// The next function are to be implemented in each module.

		template<> void ModuleClassTemplate<SCOMModule>::PreInit()
		{
			Log::GetInstance().debug("SCOM : Preinit");
		}

		template<> void ModuleClassTemplate<SCOMModule>::Init()
		{
			Log::GetInstance().debug("SCOM : Init");
		}

		template<> void ModuleClassTemplate<SCOMModule>::Start()
		{
			Log::GetInstance().debug("SCOM : Start");
		}

		template<> void ModuleClassTemplate<SCOMModule>::End()
		{
			Log::GetInstance().debug("SCOM : End");
		}

		template<> void ModuleClassTemplate<SCOMModule>::InitThread()
		{
			Log::GetInstance().debug("SCOM : Init thread");
		}

		template<> void ModuleClassTemplate<SCOMModule>::CloseThread()
		{
			Log::GetInstance().debug("SCOM : Close thread");
		}
	}
}
