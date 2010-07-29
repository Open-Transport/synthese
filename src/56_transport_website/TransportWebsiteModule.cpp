
/** PlacesListModule class implementation.
	@file PlacesListModule.cpp

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

#include "PlacesListModule.h"
#include "GraphConstants.h"
#include "FunctionWithSite.h"
#include "WebPageDisplayFunction.h"
#include "Site.h"
#include "Request.h"
#include "Env.h"
#include "WebPage.h"

using namespace std;
using namespace boost;


namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace server;
	using namespace transportwebsite;
	
	template<> const std::string util::FactorableTemplate<ModuleClass,PlacesListModule>::FACTORY_KEY("36_places_list");
	
	namespace server
	{
		template<> const string ModuleClassTemplate<PlacesListModule>::NAME("Site web transport public");
		
		template<> void ModuleClassTemplate<PlacesListModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<PlacesListModule>::Init()
		{
		}
		
		template<> void ModuleClassTemplate<PlacesListModule>::End()
		{
		}
	}

	namespace transportwebsite
	{
		PlacesListModule::UserClassNames PlacesListModule::GetAccessibilityNames()
		{
			UserClassNames result;
			result.push_back(make_pair(USER_PEDESTRIAN, "Piéton"));
			result.push_back(make_pair(USER_HANDICAPPED, "PMR"));
			result.push_back(make_pair(USER_BIKE, "Vélo"));
			return result;
		}



		boost::shared_ptr<const Site> PlacesListModule::GetSite( const server::Request& request )
		{
			{
				shared_ptr<const FunctionWithSiteBase> function(
					dynamic_pointer_cast<const FunctionWithSiteBase>(
						request.getFunction()
				)	);
				if(function.get())
				{
					return function->getSite();
				}
			}

			{
				shared_ptr<const WebPageDisplayFunction> function(
					dynamic_pointer_cast<const WebPageDisplayFunction>(
						request.getFunction()
				)	);
				if(function.get() && function->getPage())
				{
					return Env::GetOfficialEnv().getSPtr<Site>(function->getPage()->getRoot());
				}
			}

			return shared_ptr<const Site>();
		}



		boost::shared_ptr<const WebPage> PlacesListModule::GetWebPage( const server::Request& request )
		{
			shared_ptr<const WebPageDisplayFunction> function(
				dynamic_pointer_cast<const WebPageDisplayFunction>(
					request.getFunction()
			)	);
			if(function.get())
			{
				if(function->getAditionnalParametersMap().getDefault<RegistryKeyType>(WebPageDisplayFunction::PARAMETER_PAGE_ID, 0))
				{
					return Env::GetOfficialEnv().get<WebPage>(function->getAditionnalParametersMap().get<RegistryKeyType>(WebPageDisplayFunction::PARAMETER_PAGE_ID));
				}
				else
				{
					return function->getPage();
				}
			}
			return shared_ptr<const WebPage>();
		}
	}
}
