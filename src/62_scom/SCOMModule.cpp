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

#include "SCOMSocketReader.h"
#include "SCOMData.h"

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
		SCOMSocketReader* SCOMModule::_sr;
		SCOMData* SCOMModule::_sd;

		std::string SCOMModule::Test ()
		{
			return "Hello world!";
		}
	}

	namespace server
	{

		template<> const std::string ModuleClassTemplate<SCOMModule>::NAME("Temps rel SCOM et bornes IP");

		// Start the listener and affect it a SCOMData object
		template<> void ModuleClassTemplate<SCOMModule>::Start()
		{
			Log::GetInstance().debug("SCOM : Start");

			scom::SCOMModule::_sd = new scom::SCOMData();
			scom::SCOMModule::_sr = new scom::SCOMSocketReader(scom::SCOMModule::_sd);
			scom::SCOMModule::_sr->Start();
		}

		template<> void ModuleClassTemplate<SCOMModule>::End()
		{
			Log::GetInstance().debug("SCOM : End");

			scom::SCOMModule::_sr->Stop();

			delete scom::SCOMModule::_sr;
			delete scom::SCOMModule::_sd;
		}

		// Empty functions
		template<> void ModuleClassTemplate<SCOMModule>::PreInit() { }
		template<> void ModuleClassTemplate<SCOMModule>::Init() { }
		template<> void ModuleClassTemplate<SCOMModule>::InitThread() { }
		template<> void ModuleClassTemplate<SCOMModule>::CloseThread() { }
	}
}
