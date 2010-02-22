
/** WebPage class implementation.
	@file WebPage.cpp

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

#include "WebPage.h"
#include "ParametersMap.h"
#include "DynamicRequest.h"
#include "Site.h"
#include "Function.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace util
	{
		template<> const string Registry<transportwebsite::WebPage>::KEY("WebPage");
	}

	namespace transportwebsite
	{
		WebPage::WebPage( util::RegistryKeyType id /*= UNKNOWN_VALUE*/ ):
			Registrable(id),
			_site(NULL)
		{
			
		}


		const std::string& WebPage::getContent1() const
		{
			return _content1;
		}



		const std::string& WebPage::getInclude1() const
		{
			return _include1;
		}



		const std::string& WebPage::getContent2() const
		{
			return _content2;
		}



		const std::string& WebPage::getInclude2() const
		{
			return _include2;
		}



		const std::string& WebPage::getContent3() const
		{
			return _content3;
		}



		void WebPage::setContent1( const std::string& value )
		{
			_content1 = value;
		}



		void WebPage::setInclude1( const std::string& value )
		{
			_include1 = value;
		}



		void WebPage::setContent2( const std::string& value )
		{
			_content2 = value;
		}



		void WebPage::setInclude2( const std::string& value )
		{
			_include2 = value;
		}



		void WebPage::setContent3( const std::string& value )
		{
			_content3 = value;
		}



		void WebPage::DisplayInclude(
			std::ostream& stream,
			const std::string& includeString,
			const Request& request
		){
			ParametersMap parametersMap(includeString);
			ParametersMap requestParametersMap(
				dynamic_cast<const DynamicRequest*>(&request) ?
				dynamic_cast<const DynamicRequest&>(request).getParametersMap() :
				ParametersMap()
			);
			requestParametersMap.remove(Request::PARAMETER_FUNCTION);
			parametersMap.merge(requestParametersMap);
			string functionName(parametersMap.getDefault<string>(Request::PARAMETER_FUNCTION));
			if(!functionName.empty() && Factory<Function>::contains(functionName))
			{
				shared_ptr<Function> _function(Factory<Function>::create(functionName));
				if(_function.get())
				{
					try
					{
						_function->_setFromParametersMap(parametersMap);
						if (_function->isAuthorized(request.getSession()))
						{
							_function->run(stream, request);
						}
					}
					catch(...)
					{

					}
				}

			}

		}



		void WebPage::setSite( Site* value )
		{
			_site = value;
		}



		Site* WebPage::getSite() const
		{
			return _site;
		}



		const std::string& WebPage::getTitle() const
		{
			return _title;
		}



		void WebPage::setTitle( const std::string& value )
		{
			_title = value;
		}
	}
}
