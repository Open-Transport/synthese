
/** URI class implementation.
	@file URI.cpp

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

#include "URI.hpp"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	namespace util
	{
		const string URI::PARAMETER_SEPARATOR("&");
		const string URI::PARAMETER_ASSIGNMENT("=");



		std::string URI::Encode( const std::string& value )
		{
			stringstream result;
			BOOST_FOREACH(unsigned char c, value)
			{
				if(	(c >= 48 && c <= 57) ||
					(c >= 65 && c <= 90) ||
					(c >= 97 && c <= 122)
				){
					result << c;
				}
				else
				{
					result << "%";
					if(c < 16) result << "0";
					result << hex << static_cast<int>(c);
				}
			}
			return result.str();
		}



		bool URI::Decode(
			const std::string& in,
			std::string& out
		){
			out.clear();
			out.reserve(in.size());
			for(std::size_t i = 0; i < in.size(); ++i)
			{
				if (in[i] == '%')
				{
					if(i+2 <= in.size() && in[i+1] == 'u')
					{
						// Non standard unicode character is rejected
						i+=5;
					}
					else if (i + 3 <= in.size())
					{
						int value;
						std::istringstream is(in.substr(i + 1, 2));
						if (is >> std::hex >> value)
						{
							out += static_cast<char>(value);
							i += 2;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else if (in[i] == '+')
				{
					out += ' ';
				}
				else
				{
					out += in[i];
				}
			}
			return true;
		}
	}
}
