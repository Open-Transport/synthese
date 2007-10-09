
/** QueryString class implementation.
	@file QueryString.cpp

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

#include "QueryString.h"

#include <boost/algorithm/string.hpp>

using namespace std;

namespace synthese
{
	namespace server
	{
		const string QueryString::PARAMETER_SEPARATOR ("&");
		const string QueryString::PARAMETER_ASSIGNMENT ("=");
		const int QueryString::MAX_SIZE (4096);
		const uid QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION = -2;

		const string QueryString::PARAMETER_STARTER ("?");
		const string QueryString::PARAMETER_ACTION ("a");
		const string QueryString::PARAMETER_FUNCTION = "fonction";
		const string QueryString::PARAMETER_SESSION = "sid";
		const string QueryString::PARAMETER_IP = "ipaddr";
		const string QueryString::PARAMETER_CLIENT_URL = "clienturl";
		const string QueryString::PARAMETER_OBJECT_ID = "roid";
		const string QueryString::PARAMETER_ACTION_FAILED = "raf";
		const string QueryString::PARAMETER_ERROR_MESSAGE = "rem";
		const string QueryString::PARAMETER_ERROR_LEVEL = "rel";

		const std::string& QueryString::getContent() const
		{
			return _content;
		}

		QueryString::QueryString( const std::string& text, bool normalize)
			: _content(text)
		{
			if (normalize)
			{
				// The + characters are added by the web browsers instead of spaces
				boost::algorithm::replace_all (_content, "+", " ");

				// Deletes the end of line code
				size_t pos(_content.size() - 1);
				for (; pos && (_content.substr(pos, 1) == "\r" || _content.substr(pos, 1) == "\n"); --pos);
				_content = _content.substr(0, pos+1);

				/* ?? what do we do with this code ?
				if (s.size () > MAX_REQUEST_SIZE) {
				bool parameterTruncated = (s.substr (MAX_REQUEST_SIZE, 1) != PARAMETER_SEPARATOR);
				s = s.substr (0, MAX_REQUEST_SIZE);

				// Filter last parameter which if it has been truncated
				if (parameterTruncated) 
				{
				s = s.substr (0, s.rfind (PARAMETER_SEPARATOR));
				}
				}
				*/
			}
		}
	}
}
