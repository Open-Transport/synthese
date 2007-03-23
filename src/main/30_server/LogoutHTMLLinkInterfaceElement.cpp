
/** LogoutHTMLLinkInterfaceElement class implementation.
	@file LogoutHTMLLinkInterfaceElement.cpp

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

#include <sstream>

#include "01_util/Html.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/Interface.h"
#include "11_interfaces/RedirRequest.h"
#include "11_interfaces/SimplePageRequest.h"

#include "30_server/LogoutAction.h"
#include "30_server/LogoutHTMLLinkInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	namespace server
	{
		void LogoutHTMLLinkInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 3)
				throw InterfacePageException("Not enough parameters for logout HTML link");
			_redirectionURL = vel.front();
			_page_key = vel.front();
			_content = vel.front();
			if (!vel.isEmpty())
				_icon = vel.front();
		}

		string LogoutHTMLLinkInterfaceElement::getValue(const interfaces::ParametersVector& parameters, interfaces::VariablesMap& variables, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stringstream stream;
			std::string url = _redirectionURL->getValue(parameters, variables, rootObject, request);
			std::string requestKey = _page_key->getValue(parameters, variables, rootObject, request);
			std::string content = _content->getValue(parameters, variables, NULL, request);
			std::string icon;
			if (_icon != NULL)
				icon = _icon->getValue(parameters, variables, NULL, request);

			if (!requestKey.empty())
			{
				try
				{
					SimplePageRequest* redirRequest = Factory<Request>::create<SimplePageRequest>();
					redirRequest->copy(request);
					redirRequest->setPage(_page->getInterface()->getPage(requestKey));
					redirRequest->setAction(Factory<Action>::create<LogoutAction>());
					stream << Html::getLinkButton(redirRequest->getURL(), content, "", icon);
					delete redirRequest;
				}
				catch (InterfacePageException e)
				{
					Log::GetInstance().debug("No such interface page "+ requestKey);
				}
			}
			if (url.size())
			{
				RedirRequest* redirRequest = Factory<Request>::create<RedirRequest>();
				redirRequest->copy(request);
				redirRequest->setUrl(url);
				redirRequest->setAction(Factory<Action>::create<LogoutAction>());
				stream << Html::getLinkButton(redirRequest->getURL(), content, "", icon);
				delete redirRequest;
			}
			return stream.str();
		}

		LogoutHTMLLinkInterfaceElement::~LogoutHTMLLinkInterfaceElement()
		{
			delete _redirectionURL;
			delete _page_key;
			delete _content;
			delete _icon;
		}

		LogoutHTMLLinkInterfaceElement::LogoutHTMLLinkInterfaceElement()
			: ValueInterfaceElement()
			, _redirectionURL(NULL)
			, _page_key(NULL)
			, _content(NULL)
			, _icon(NULL)
		{

		}
	}
}
