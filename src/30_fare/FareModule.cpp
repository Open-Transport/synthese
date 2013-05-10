
/** FareModule class implementation.
    @file FareModule.cpp

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

#include "FareModule.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace fare;
	using namespace security;


	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,FareModule>::FACTORY_KEY = "30_fare";
	}


	namespace server
	{
		template<> const string ModuleClassTemplate<FareModule>::NAME = "Tarification";

		template<> void ModuleClassTemplate<FareModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<FareModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<FareModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<FareModule>::End()
		{
		}



		template<> void ModuleClassTemplate<FareModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<FareModule>::CloseThread(
		){
		}
	
	}

	namespace fare
	{
}	}
