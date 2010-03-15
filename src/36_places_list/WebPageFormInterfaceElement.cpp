
/** WebPageFormInterfaceElement class implementation.
	@file WebPageFormInterfaceElement.cpp
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

#include "WebPageFormInterfaceElement.h"
#include "ValueElementList.h"
#include "HTMLForm.h"
#include "WebPageDisplayFunction.h"
#include "WebPage.h"
#include "Env.h"
#include "StaticFunctionRequest.h"
#include "Site.h"
#include "Interface.h"

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
		template<> const string FactorableTemplate<LibraryInterfaceElement, transportwebsite::WebPageFormInterfaceElement>::FACTORY_KEY("form");
	}

	namespace transportwebsite
	{
		void WebPageFormInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_formName = vel.front();
			_pageId = vel.front();
			_verificationScript = vel.front();
		}

		string WebPageFormInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			string formName(_formName.get() ? _formName->getValue(parameters,variables,object,request) : string());
			RegistryKeyType pageId(_pageId.get() ? lexical_cast<RegistryKeyType>(_pageId->getValue(parameters,variables,object,request)) : RegistryKeyType(0));
			string verificationScript(_verificationScript.get() ? _verificationScript->getValue(parameters,variables,object,request) : string());
			
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

				HTMLForm form(openRequest.getHTMLForm(formName));
				stream << form.open(verificationScript.empty() ? string() : ("onsubmit=\"return "+ verificationScript +"\""));
				stream << form.getHiddenFields();
			}
			catch(ObjectNotFoundException<WebPage>)
			{

			}
			return string();
		}

		WebPageFormInterfaceElement::~WebPageFormInterfaceElement()
		{
		}
	}
}
