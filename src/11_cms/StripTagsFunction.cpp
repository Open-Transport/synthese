
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

#include <boost/foreach.hpp>
#include <stdio.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

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
		const TranslateHTMLChars StripTagsFunction::HTML_CHARS = TranslateHTMLChars();
		const int StripTagsFunction::MAX_CODE_LENGTH(8);

		ParametersMap StripTagsFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void StripTagsFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_text = map.getDefault<string>(PARAMETER_TEXT);
			_decode = map.getDefault<bool>(PARAMETER_CODE, false);

			/* If specified, decode url encoded content */
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
				int html_flag = 0;
				bool special_char_flag = false;
				string htmlcode;

				/* Initialize special characters to convert */
													
				for (std::string::const_iterator it=_text.begin(); it!=_text.end(); ++it)
				{
					if (*it == '<')
					{
						html_flag++;
					}
					else if (*it == '>' && html_flag > 0)
					{
						html_flag--;
					}
					else if (*it == '&' && html_flag == 0)
					{
						special_char_flag = true;
						for (int i=1; i < MAX_CODE_LENGTH; i++)
						{
							if ((it+i == _text.end() || *(it+i) == ' ') || (i == MAX_CODE_LENGTH-1 && *(it+i) != ';'))
							{
								special_char_flag = false;
								break;
							}
							
							if (*(it+i) != ';')
							{
								htmlcode += *(it+i);
							}
							else
							{
								it = it+i;
								break;
							}

						}

						if (special_char_flag)
						{
							BOOST_FOREACH(const TranslateHTMLChars::HTMLChars::value_type& sc, HTML_CHARS._htmlchars)
							{
								if (htmlcode == sc.first)
								{
									stream << sc.second;
									htmlcode.clear();
									break;
								}
							}
							if (!htmlcode.empty())
							{
								stream << "&" << htmlcode << ";";
								htmlcode.clear();
							}
						}
						else
						{
							stream << *it;
						}
					}
					else if (*it != '>' && html_flag == 0)
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
			unsigned int i, j;
			
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
