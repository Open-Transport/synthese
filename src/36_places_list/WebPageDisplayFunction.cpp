
/** WebPageDisplayFunction class implementation.
	@file WebPageDisplayFunction.cpp
	@author Hugues
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "WebPageDisplayFunction.h"
#include "Env.h"
#include "Interface.h"
#include "WebPage.h"
#include "Site.h"
#include "WebPageInterfacePage.h"
#include "TransportWebsiteRight.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace interfaces;
	using namespace transportwebsite;

	template<> const string util::FactorableTemplate<Function,WebPageDisplayFunction>::FACTORY_KEY("page");
	
	namespace transportwebsite
	{
		const string WebPageDisplayFunction::PARAMETER_PAGE_ID("p");
		const string WebPageDisplayFunction::PARAMETER_USE_TEMPLATE("use_template");

		ParametersMap WebPageDisplayFunction::_getParametersMap() const
		{
			ParametersMap map(_aditionnalParameters);
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			return map;
		}

		void WebPageDisplayFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = Env::GetOfficialEnv().get<WebPage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch (ObjectNotFoundException<WebPage>)
			{
				throw RequestException("No such page");
			}

			_useTemplate = map.getDefault<bool>(PARAMETER_USE_TEMPLATE, true);
			_aditionnalParameters = map;

			if(	!_page->mustBeDisplayed()
			){
				throw RequestException("The page is not displayed right now");
			}
		}

		void WebPageDisplayFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_page.get())
			{
				if(_useTemplate && _page->getTemplate())
				{
					WebPageInterfacePage::Display(
						stream,
						*_page->getTemplate(),
						request,
						*_page,
						false
					);
				}
				else
				{
					_page->display(stream, request);
				}
			}
		}
		
		
		
		bool WebPageDisplayFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageDisplayFunction::getOutputMimeType() const
		{
			return "text/html";
		}



		WebPageDisplayFunction::WebPageDisplayFunction():
		_useTemplate(true)
		{

		}
	}
}
