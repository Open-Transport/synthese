
/** WebPageLinkFunction class implementation.
	@file WebPageLinkFunction.cpp
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
#include "WebPageLinkFunction.hpp"
#include "HTMLModule.h"
#include "Webpage.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace html;

	template<> const string util::FactorableTemplate<Function,cms::WebPageLinkFunction>::FACTORY_KEY("link");
	
	namespace cms
	{
		const string WebPageLinkFunction::PARAMETER_TARGET("target");
		const string WebPageLinkFunction::PARAMETER_TEXT("text");
		const string WebPageLinkFunction::PARAMETER_USE_SMART_URL("use_smart_url");
		
		ParametersMap WebPageLinkFunction::_getParametersMap() const
		{
			ParametersMap map(_otherParameters);
			if(_target.get())
			{
				map.insert(PARAMETER_TARGET, _target->getKey());
			}
			map.insert(PARAMETER_TEXT, _text);
			map.insert(PARAMETER_USE_SMART_URL, _useSmartURL);
			return map;
		}



		void WebPageLinkFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_target = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_TARGET));
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such web page");
			}
			optional<string> ot(map.getOptional<string>(PARAMETER_TEXT));
			_text = ot ? *ot : _target->getName();
			_otherParameters = map;
			_otherParameters.remove(PARAMETER_TARGET);
			_otherParameters.remove(PARAMETER_TEXT);
			_otherParameters.remove(Request::PARAMETER_FUNCTION);
			_useSmartURL = map.getDefault<bool>(PARAMETER_USE_SMART_URL, true);
		}



		void WebPageLinkFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			string url;
			shared_ptr<const WebPageDisplayFunction> webpageDisplayFunction(
				dynamic_pointer_cast<const WebPageDisplayFunction,const Function>(
					request.getFunction()
			)	);

			if(	_useSmartURL &&
				!_target->getSmartURLPath().empty()
			){	// URL is smart URL
				url = _target->getSmartURLPath();

				// TODO : add parameter
			}
			else
			{	// Classic URL
				StaticFunctionRequest<WebPageDisplayFunction> openRequest(request, false);
				openRequest.getFunction()->setPage(_target);
				openRequest.getFunction()->setAditionnalParametersMap(_otherParameters);
				if(!_target->getRoot()->getClientURL().empty())
				{
					openRequest.setClientURL(_target->getRoot()->getClientURL());
				}
				url = openRequest.getURL();
			}
			stream << HTMLModule::getHTMLLink(url, _text);
		}
		
		
		
		bool WebPageLinkFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageLinkFunction::getOutputMimeType() const
		{
			return "text/html";
		}



		WebPageLinkFunction::WebPageLinkFunction():
			_useSmartURL(true)
		{

		}
	}
}
