
/** WebPageInterfacePage class implementation.
	@file WebPageInterfacePage.cpp
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

#include "WebPageInterfacePage.h"
#include "WebPage.h"

#include <sstream>

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, transportwebsite::WebPageInterfacePage>::FACTORY_KEY("web_page");
	}

	namespace transportwebsite
	{
		WebPageInterfacePage::WebPageInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, WebPageInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void WebPageInterfacePage::display(
			std::ostream& stream,
			const WebPage& webPage,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
		
			pv.push_back(webPage.getTitle()); //0
			pv.push_back(webPage.getContent1()); //1

			stringstream include1;
			if(request)
			{
				WebPage::DisplayInclude(include1, webPage.getInclude1(), *request);
			}
			pv.push_back(include1.str()); //2

			pv.push_back(webPage.getContent2()); //3

			stringstream include2;
			if(request)
			{
				WebPage::DisplayInclude(include2, webPage.getInclude2(), *request);
			}
			pv.push_back(include2.str()); //4

			pv.push_back(webPage.getContent3()); //5


			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&webPage)
				, request
			);
		}
	}
}
