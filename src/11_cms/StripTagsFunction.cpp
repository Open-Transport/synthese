
/** StripTagsFunction class implementation.
	@file StripTagsFunction.cpp
	@date 2013

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
#include "StripTagsFunction.hpp"

#include <stdio.h>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::StripTagsFunction>::FACTORY_KEY("StripTags");

	namespace cms
	{
		const string StripTagsFunction::PARAMETER_TEXT("t");
		const string StripTagsFunction::PARAMETER_CODE("d");



		ParametersMap StripTagsFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void StripTagsFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_text = map.getDefault<string>(PARAMETER_TEXT);
			_decode = map.getDefault<bool>(PARAMETER_CODE, false);
	
			if(_decode)
			{
				_text = urlDecode(_text);
			}
		}



		util::ParametersMap StripTagsFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_text.size() > 0)
			{
				int flag = 0;
				for (std::string::const_iterator it=_text.begin(); it!=_text.end(); ++it)
				{
					if (*it == '<')
					{
						flag++;
					}
					else if (*it == '>' && flag > 0)
					{
						flag--;
					}

					if (flag == 0 && *it != '>')
					{
						stream << *it;
					}
				}
			}
				
			return util::ParametersMap();
		}



		bool StripTagsFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string StripTagsFunction::getOutputMimeType() const
		{
			return "text/plain";
		}



		std::string StripTagsFunction::urlDecode(std::string &todecode) 
		{
			std::string ret;
			char ch;
			int i, j;
			
			for (i=0; i < todecode.length(); i++)
			{
				if (int(todecode[i]) == 37)
				{
					sscanf(todecode.substr(i+1,2).c_str(), "%x", &j);
					ch = static_cast<char>(j);
					ret += ch;
					i = i+2;
				}
				else
				{
					ret += todecode[i];
				}
			}
			return (ret);
		}
	}
}
