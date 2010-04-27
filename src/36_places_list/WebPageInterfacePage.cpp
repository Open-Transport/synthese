
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
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace transportwebsite
	{
		const string WebPageInterfacePage::DATA_CONTENT("content");
		const string WebPageInterfacePage::DATA_TITLE("title");



		void WebPageInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const WebPage> templatePage,
			const server::Request& request,
			boost::shared_ptr<const WebPage> page,
			bool edit
		){
			if(templatePage.get())
			{
				StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
				displayRequest.getFunction()->setPage(templatePage);
				ParametersMap pm;

				pm.insert(DATA_TITLE, page->getName());
				stringstream content;
				page->display(content, request);
				pm.insert(DATA_CONTENT, content.str());
				pm.insert(Request::PARAMETER_OBJECT_ID, page->getKey());

				displayRequest.getFunction()->setAditionnalParametersMap(pm);
				displayRequest.run(stream);
			}
			else
			{
				page->display(stream, request);
			}
		}
	}
}
