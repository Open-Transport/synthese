
/** WebPageLinkInterfaceElement class implementation.
	@file WebPageLinkInterfaceElement.cpp
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

#include "WebPageLinkInterfaceElement.h"
#include "ValueElementList.h"
#include "WebPage.h"
#include "WebPageDisplayFunction.h"
#include "HTMLModule.h"
#include "Env.h"
#include "StaticFunctionRequest.h"
#include "Site.h"
#include "Interface.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace html;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, transportwebsite::WebPageLinkInterfaceElement>::FACTORY_KEY("link");
	}

	namespace transportwebsite
	{
		void WebPageLinkInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_target = vel.front();
			_text = vel.front();
		}

		string WebPageLinkInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			string target(_target.get() ? _target->getValue(parameters,variables,object,request) : string());
			string text(_text.get() ? _text->getValue(parameters,variables,object,request) : string());

			if(target.empty())
			{
				stream << text;
				return string();
			}

			RegistryKeyType pageId(0);
			try
			{
				pageId = lexical_cast<RegistryKeyType>(target);
			}
			catch (bad_lexical_cast)
			{				
			}

			if(pageId == 0)
			{
				stream << HTMLModule::getHTMLLink(
					target,
					text.empty() ? target : text
				);
			}
			else
			{
				if(!request)
				{
					return string();
				}

				shared_ptr<const Function> fws(request->getFunction());
				if(!fws.get())
				{
					return string();
				}

				try
				{
					StaticFunctionRequest<WebPageDisplayFunction> openRequest(*request, false);
					shared_ptr<const WebPage> page(Env::GetOfficialEnv().get<WebPage>(pageId));
					openRequest.getFunction()->setPage(page);
					if(page->getRoot()->getInterface() && !page->getRoot()->getInterface()->getDefaultClientURL().empty())
					{
						openRequest.setClientURL(page->getRoot()->getInterface()->getDefaultClientURL());
					}
					stream << HTMLModule::getHTMLLink(
						openRequest.getURL(),
						text.empty() ? page->getName() : text
					);
				}
				catch(ObjectNotFoundException<WebPage>)
				{

				}
			}
			return string();
		}

		WebPageLinkInterfaceElement::~WebPageLinkInterfaceElement()
		{
		}
	}
}
