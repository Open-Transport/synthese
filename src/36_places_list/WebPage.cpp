
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
			_startDate(posix_time::not_a_date_time),
			_endDate(posix_time::not_a_date_time)
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



		const boost::posix_time::ptime& WebPage::getStartDate() const
		{
			return _startDate;
		}



		const boost::posix_time::ptime& WebPage::getEndDate() const
		{
			return _endDate;
		}



		void WebPage::setStartDate( const boost::posix_time::ptime& value )
		{
			_startDate = value;
		}



		void WebPage::setEndDate( const boost::posix_time::ptime& value )
		{
			_endDate = value;
		}



		bool WebPage::mustBeDisplayed( boost::posix_time::ptime now /*= boost::posix_time::second_clock::local_time()*/ ) const
		{
			return
				(_startDate.is_not_a_date_time() || _startDate <= now) &&
				(_endDate.is_not_a_date_time() || _endDate >= now)
			;
		}
	}
}
