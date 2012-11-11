
/** AdminModule class implementation.
	@file AdminModule.cpp

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

#include "AdminModule.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace admin;

	namespace util
	{
		template<> const string	FactorableTemplate<ModuleClass,AdminModule>::FACTORY_KEY("14_admin");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<AdminModule>::NAME("Console d'administration");

		template<> void ModuleClassTemplate<AdminModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<AdminModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<AdminModule>::End()
		{
		}



		template<> void ModuleClassTemplate<AdminModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<AdminModule>::CloseThread(
		){
		}
	}

	namespace admin
	{
		const std::string AdminModule::TABLE_COL_ID = "id";
		const std::string AdminModule::CSS_TIME_INPUT = "time_input";
		const std::string AdminModule::CSS_2DIGIT_INPUT = "two_digit_input";
}	}
