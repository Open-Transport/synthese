
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
#include "Webpage.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "CMSModule.hpp"

#include <sstream>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace cms
	{
		const string WebPageInterfacePage::DATA_CONTENT("content");
		const string WebPageInterfacePage::DATA_TITLE("title");
		const string WebPageInterfacePage::DATA_ABSTRACT("abstract");
		const string WebPageInterfacePage::DATA_IMAGE("image");
		const string WebPageInterfacePage::DATA_PUBLICATION_DATE("date");
		const string WebPageInterfacePage::DATA_FORUM("forum");
		const string WebPageInterfacePage::DATA_IS_CURRENT("is_the_current_page");
		const string WebPageInterfacePage::DATA_DEPTH("depth");



		void WebPageInterfacePage::Display(
			std::ostream& stream,
			const Webpage& templatePage,
			const server::Request& request,
			const Webpage& page,
			bool edit,
			bool displayContent
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(Env::GetOfficialEnv().getSPtr(&templatePage));
			ParametersMap pm(
				dynamic_pointer_cast<const WebPageDisplayFunction>(request.getFunction()) ?
				dynamic_pointer_cast<const WebPageDisplayFunction>(request.getFunction())->getAditionnalParametersMap() :
				ParametersMap()
			);

			pm.insert(DATA_TITLE, page.getName());
			
			if(displayContent)
			{
				stringstream content;
				page.display(content, request);
				pm.insert(DATA_CONTENT, content.str());
			}
			pm.insert(DATA_ABSTRACT, page.getAbstract());
			pm.insert(DATA_IMAGE, page.getImage());
			pm.insert(
				DATA_PUBLICATION_DATE,
				page.getStartDate().is_not_a_date_time() ? string() : lexical_cast<string>(page.getStartDate())
			);
			pm.insert(Request::PARAMETER_OBJECT_ID, page.getKey());
			pm.insert(DATA_FORUM, page.getHasForum());

			shared_ptr<const Webpage> curPage(CMSModule::GetWebPage(request));
			pm.insert(DATA_IS_CURRENT, curPage.get() == &page);

			pm.insert(DATA_DEPTH, page.getDepth());

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.getFunction()->setUseTemplate(false);
			displayRequest.run(stream);
		}
	}
}
