
/** WebPageFormFunction class implementation.
	@file WebPageFormFunction.cpp
	@author Hugues Romain
	@date 2010

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
		const string WebPageFormFunction::PARAMETER_CLASS("class");
		const string WebPageFormFunction::PARAMETER_IDEM("idem");

		ParametersMap WebPageFormFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_SCRIPT, _script);
			map.insert(PARAMETER_CLASS, _class);
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
			_class = map.getDefault<string>(PARAMETER_CLASS);
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

			// Additional parameters
			_parameters = map;
			_parameters.remove(PARAMETER_NAME);
			_parameters.remove(PARAMETER_SCRIPT);
			_parameters.remove(PARAMETER_CLASS);
			_parameters.remove(PARAMETER_IDEM);
			_parameters.remove(PARAMETER_PAGE_ID);
		}



		util::ParametersMap WebPageFormFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			std::string htmlComplement;
			if(!_script.empty())
			{
				htmlComplement += "onsubmit=\"return "+ _script +"\"";
			}
			if(!_class.empty())
			{
				htmlComplement += " class=\""+ _class +"\"";
			}

			if(_idem)
			{
				HTMLForm form(request.getHTMLForm(_name));
				stream << form.open(htmlComplement);
				stream << form.getHiddenFields();
			}
			else
			{
				shared_ptr<const Function> fws(request.getFunction());
				if(!fws.get())
				{
					return ParametersMap();
				}

				try
				{
					StaticFunctionRequest<WebPageDisplayFunction> openRequest(request, false);
					openRequest.getFunction()->setDontRedirectIfSmartURL(_templateParameters.getDefault<bool>(WebPageDisplayFunction::PARAMETER_DONT_REDIRECT_IF_SMART_URL, false));
					openRequest.getFunction()->setPage(_page);
					if(!_page->getRoot()->get<ClientURL>().empty())
					{
						openRequest.setClientURL(_page->getRoot()->get<ClientURL>());
					}

					HTMLForm form(openRequest.getHTMLForm(_name));
					BOOST_FOREACH(const ParametersMap::Map::value_type& parameter, _parameters.getMap())
					{
						form.addHiddenField(parameter.first, ParametersMap::Trim(parameter.second));
					}
					stream << form.open(htmlComplement);
					stream << form.getHiddenFields();
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
				}
			}

			return util::ParametersMap();
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
