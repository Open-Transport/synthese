
/** CMSModule class implementation.
	@file CMSModule.cpp

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

#include "CMSModule.hpp"
#include "Website.hpp"
#include "WebPage.h"

namespace synthese
{
	using namespace server;
	using namespace cms;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,CMSModule>::FACTORY_KEY("36_cms");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<CMSModule>::NAME("CMS");

		template<> void ModuleClassTemplate<CMSModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<CMSModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<CMSModule>::End()
		{
		}
	}

	namespace cms
	{



		boost::shared_ptr<const Website> CMSModule::GetSite(
			const server::Request& request
		){
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
					return Env::GetOfficialEnv().getSPtr<Website>(function->getPage()->getRoot());
				}
			}

			return shared_ptr<const Website>();
		}



		boost::shared_ptr<const WebPage> CMSModule::GetWebPage(
			const server::Request& request
		){
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
