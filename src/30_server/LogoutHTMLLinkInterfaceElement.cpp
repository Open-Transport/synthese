
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

#include "LogoutHTMLLinkInterfaceElement.h"

#include "LogoutAction.h"
#include "ActionFunctionRequest.h"

#include "HTMLForm.h"

#include "ValueElementList.h"
#include "InterfacePage.h"
#include "InterfacePageException.h"
#include "Interface.h"
#include "RedirFunction.h"
#include "SimplePageFunction.h"
#include "NonPredefinedInterfacePage.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace util;
	using namespace interfaces;
	using namespace html;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, LogoutHTMLLinkInterfaceElement>::FACTORY_KEY("logouthtmllink");
	}

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

		string LogoutHTMLLinkInterfaceElement::display(
			ostream& stream
			, const interfaces::ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* rootObject /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			std::string url = _redirectionURL->getValue(parameters, variables, rootObject, request);
			std::string requestKey = _page_key->getValue(parameters, variables, rootObject, request);
			std::string content = _content->getValue(parameters, variables, NULL, request);
			std::string icon;
			if (_icon != NULL)
				icon = _icon->getValue(parameters, variables, NULL, request);

			if (!requestKey.empty() && request)
			{
				try
				{
					ActionFunctionRequest<LogoutAction,SimplePageFunction> redirRequest(*request);
					redirRequest.getFunction()->setPage(_page->getInterface()->getPage(NonPredefinedInterfacePage::FACTORY_KEY, requestKey));
					stream << redirRequest.getHTMLForm().getLinkButton(content, "", icon);
				}
				catch (InterfacePageException e)
				{
					Log::GetInstance().debug("No such interface page "+ requestKey);
				}
			}
			if (url.size() && request)
			{
				ActionFunctionRequest<LogoutAction,RedirFunction> redirRequest(*request);
				redirRequest.getFunction()->setRedirURL(url);
				stream << redirRequest.getHTMLForm().getLinkButton(content, "", icon);
			}
			return string();
		}
	}
}
