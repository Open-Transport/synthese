
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

#include "WebPageFormFunction.hpp"

#include "RequestException.h"
#include "Request.h"
#include "Webpage.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "HTMLForm.h"
#include "Action.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace cms;
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace html;

	template<>
	const string FactorableTemplate<FunctionWithSite<false>, WebPageFormFunction>::FACTORY_KEY = "form";

	namespace cms
	{
		const string WebPageFormFunction::PARAMETER_NAME("name");
		const string WebPageFormFunction::PARAMETER_FORM_ID("form_id");
		const string WebPageFormFunction::PARAMETER_PAGE_ID("page_id");
		const string WebPageFormFunction::PARAMETER_TARGET = "target";
		const string WebPageFormFunction::PARAMETER_SCRIPT("script");
		const string WebPageFormFunction::PARAMETER_CLASS("class");
		const string WebPageFormFunction::PARAMETER_IDEM("idem");

		ParametersMap WebPageFormFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_FORM_ID, _formId);
			map.insert(PARAMETER_SCRIPT, _script);
			map.insert(PARAMETER_CLASS, _class);
			if(_page)
			{
				map.insert(PARAMETER_TARGET, _page->getKey());
			}
			map.insert(PARAMETER_IDEM, _idem);
			return map;
		}



		void WebPageFormFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.getDefault<string>(PARAMETER_NAME);
			_formId = map.getDefault<string>(PARAMETER_FORM_ID);
			_script = map.getDefault<string>(PARAMETER_SCRIPT);
			_class = map.getDefault<string>(PARAMETER_CLASS);
			_idem = map.getDefault<bool>(PARAMETER_IDEM, false);

			// Web page target
			if(!_idem)
			{
				string targetStr(map.getDefault<string>(PARAMETER_TARGET));
				if(targetStr.empty())
				{
					targetStr = map.getDefault<string>(PARAMETER_PAGE_ID);
				}
				ParametersMap::Trim(targetStr);
				if(targetStr[0] >= '0' && targetStr[0] <= '9')
				{	// Page by ID
					try
					{
						RegistryKeyType pageId(lexical_cast<RegistryKeyType>(targetStr));
						_page = Env::GetOfficialEnv().get<Webpage>(pageId).get();
					}
					catch(bad_lexical_cast&)
					{
						throw RequestException("Bad cast in page id");
					}
					catch(ObjectNotFoundException<Webpage>&)
					{
						throw RequestException("No such web page");
					}
				}
				else
				{	// Page by smart URL
					_page = getSite()->getPageBySmartURL(targetStr);
					if(!_page)
					{
						throw RequestException("No such web page");
					}
				}
			}

			// Additional parameters
			BOOST_FOREACH(const ParametersMap::Map::value_type& item, map.getMap())
			{
				if(item.first == PARAMETER_NAME ||
					item.first == PARAMETER_FORM_ID ||
					item.first == PARAMETER_SCRIPT ||
					item.first == PARAMETER_CLASS ||
					item.first == PARAMETER_IDEM ||
					item.first == PARAMETER_PAGE_ID ||
					item.first == PARAMETER_TARGET
				){
					continue;
				}

				// Using the getValue virtual method instead of item.second in order to handle DelayedEvaluationParametersMap
				_parameters.insert(item.first, map.getValue(item.first));
			}
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
			if(!_formId.empty())
			{
				htmlComplement += " id=\""+ _formId +"\"";
			}

			if(_idem)
			{
				HTMLForm form(request.getHTMLForm(_name));
				stream << form.open(htmlComplement);
				stream << form.getHiddenFields();
			}
			else
			{
				boost::shared_ptr<const Function> fws(request.getFunction());
				if(!fws.get())
				{
					return ParametersMap();
				}

				try
				{
					// The form
					auto_ptr<HTMLForm> form;

					if(_page->get<SmartURLPath>().empty())
					{
						// Case without smart URL : call the page display function with a page ID
						StaticFunctionRequest<WebPageDisplayFunction> openRequest(request, false);
						openRequest.getFunction()->setPage(_page);

						// Use site smart URL if available
						if(!_page->getRoot()->get<ClientURL>().empty())
						{
							openRequest.setClientURL(_page->getRoot()->get<ClientURL>());
						}

						// Form initialization
						form.reset(new HTMLForm(openRequest.getHTMLForm(_name)));
					}
					else
					{
						// Case with smart URL : use it in the action attribute
						form.reset(
							new HTMLForm(
								_name,
								_page->getRoot()->get<ClientURL>() + _page->get<SmartURLPath>()
						)	);
					}

					// Postcondition : form is now initialized

					// Add additional parameters
					BOOST_FOREACH(const ParametersMap::Map::value_type& parameter, _parameters.getMap())
					{
						form->addHiddenField(
							parameter.first,
							ParametersMap::Trim(parameter.second)
						);
					}

					// Output
					stream << form->open(htmlComplement);
					stream << form->getHiddenFields();
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
