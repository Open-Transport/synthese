
/** SettingsModule class implementation.
    @file SettingsModule.cpp

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

#include "SettingsModule.h"

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace settings;

	template<> const std::string FactorableTemplate<ModuleClass,SettingsModule>::FACTORY_KEY("63_settings");

	namespace server
	{
		template<> const std::string ModuleClassTemplate<SettingsModule>::NAME = "Prefrences";

		// Empty functions
		template<> void ModuleClassTemplate<SettingsModule>::Start() { }
		template<> void ModuleClassTemplate<SettingsModule>::End() { }
		template<> void ModuleClassTemplate<SettingsModule>::PreInit() { }
		template<> void ModuleClassTemplate<SettingsModule>::Init() { }
		template<> void ModuleClassTemplate<SettingsModule>::InitThread() { }
		template<> void ModuleClassTemplate<SettingsModule>::CloseThread() { }
	
	}
}
