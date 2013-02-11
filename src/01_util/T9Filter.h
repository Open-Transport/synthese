
/** T9Filter class header.
	@file T9Filter.h

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

#ifndef SYNTHESE_UTIL_T9FILTER_H
#define SYNTHESE_UTIL_T9FILTER_H

#include <string>
#include <iostream>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/write.hpp>

namespace synthese
{
	namespace util
	{

		/** Converts any letter into T9 replacement digit.
			The digits are unchanged.
			All other characters are converted to 0.
			Input stream must be ISO8859-1

			@ingroup m01
		*/
		class T9Filter : public boost::iostreams::output_filter
		{
		private:
		public:

			T9Filter () {}

			template<typename Sink>
			bool put(Sink& dest, int c);


		};




		template<typename Sink>
		bool
			T9Filter::put(Sink& dest, int c)
		{
			if ((c >= 'A' && c <= 'C') || (c >= 'a' && c <= 'c') || c == 0xE0 || c == 0xC2 || c == 0xC4 || c == 0xE7 || c == 0xC0)
				return boost::iostreams::put(dest, '2');
			else if ((c >= 'D' && c <= 'F') || (c >= 'd' && c <= 'f') || c == 0xE9 || c == 0xEA || c == 0xE8 || c == 0XCA || c == 0XCB || c == 0XC8)
				return boost::iostreams::put(dest, '3');
			else if ((c >= 'G' && c <= 'I') || (c >= 'g' && c <= 'i') || c == 0XEE || c == 0XEF || c == 0XCE || c == 0XCF || c == 0XEC || c == 0XCC)
				return boost::iostreams::put(dest, '4');
			else if ((c >= 'J' && c <= 'L') || (c >= 'j' && c <= 'l'))
				return boost::iostreams::put(dest, '5');
			else if ((c >= 'M' && c <= 'O') || (c >= 'm' && c <= 'o') || c == 0XF1 || c == 0XD1 || c == 0XF4 || c == 0XF6 || c == 0XF5 || c == 0XD4 || c == 0XD6 || c == 0XD5 || c == 0XF2 || c == 0XD2)
				return boost::iostreams::put(dest, '6');
			else if ((c >= 'P' && c <= 'S') || (c >= 'p' && c <= 's'))
				return boost::iostreams::put(dest, '7');
			else if ((c >= 'T' && c <= 'V') || (c >= 't' && c <= 'v') || c == 0XF9 || c == 0XD9 || c == 0XFB || c == 0XDB || c == 0XFC || c == 0XDC)
				return boost::iostreams::put(dest, '8');
			else if ((c >= 'W' && c <= 'Z') || (c >= 'w' && c <= 'z'))
				return boost::iostreams::put(dest, '7');
			else if (c >= '0' && c <= '9')
				return boost::iostreams::put(dest, c);
			else
				return boost::iostreams::put(dest, '0');
		}
	}
}
#endif

