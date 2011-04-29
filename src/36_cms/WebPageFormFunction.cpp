
/** WebPageFormFunction class implementation.
	@file WebPageFormFunction.cpp
	@author Hugues Romain
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
#include "WebPageFormFunction.hpp"
#include "Webpage.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "HTMLForm.h"
#include "Action.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace html;

	template<> const string util::FactorableTemplate<Function,cms::WebPageFormFunction>::FACTORY_KEY("form");

	namespace cms
	{
		const string WebPageFormFunction::PARAMETER_NAME("name");
		const string WebPageFormFunction::PARAMETER_PAGE_ID("page_id");
		const string WebPageFormFunction::PARAMETER_SCRIPT("script");
		const string WebPageFormFunction::PARAMETER_IDEM("idem");

		ParametersMap WebPageFormFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_SCRIPT, _script);
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			map.insert(PARAMETER_IDEM, _idem);
			return map;
		}



		void WebPageFormFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.get<string>(PARAMETER_NAME);
			_script = map.getDefault<string>(PARAMETER_SCRIPT);
			_idem = map.getDefault<bool>(PARAMETER_IDEM, false);
			if(!_idem)
			{
				try
				{
					_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such page");
				}
			}
			_savedParameters.remove(PARAMETER_PAGE_ID);
			_savedParameters.remove(PARAMETER_NAME);
			_savedParameters.remove(PARAMETER_SCRIPT);
			_savedParameters.remove(PARAMETER_IDEM);
		}



		void WebPageFormFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			if(_idem)
			{
				HTMLForm form(request.getHTMLForm(_name));
				stream << form.open(_script.empty() ? string() : ("onsubmit=\"return "+ _script +"\""));
				stream << form.getHiddenFields();
			}
			else
			{
				shared_ptr<const Function> fws(request.getFunction());
				if(!fws.get())
				{
					return;
				}

				try
				{
					StaticFunctionRequest<WebPageDisplayFunction> openRequest(request, false);
					openRequest.getFunction()->setSavedParameters(_savedParameters);
					openRequest.getFunction()->setPage(_page);
					if(!_page->getRoot()->getClientURL().empty())
					{
						openRequest.setClientURL(_page->getRoot()->getClientURL());
					}

					HTMLForm form(openRequest.getHTMLForm(_name));
					stream << form.open(_script.empty() ? string() : ("onsubmit=\"return "+ _script +"\""));
					stream << form.getHiddenFields();
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
				}
			}
		}



		bool WebPageFormFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageFormFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
