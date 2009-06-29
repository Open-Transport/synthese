
/** AdminModule class implementation.
	@file AdminModule.cpp

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

#include "AdminModule.h"
#include "AdminInterfaceElement.h"
#include "AdminRequest.h"

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
	}
	
	namespace admin
	{
		const std::string AdminModule::TABLE_COL_ID = "id";
		const std::string AdminModule::ICON_PATH_INTERFACE_VARIABLE = "icon_path";



		void AdminModule::ChangePageInRequest(
			server::Request& request,
			const std::string& oldPage,
			const std::string& newPage
		){
			AdminRequest* ar(dynamic_cast<AdminRequest*>(request._getFunction().get()));
			if(ar != NULL)
			{
				if(ar->getPage()->getFactoryKey() == oldPage)
				{
					ar->setPage(
						shared_ptr<AdminInterfaceElement>(Factory<AdminInterfaceElement>::create(newPage))
					);
				}
			}
		}
	}
}

