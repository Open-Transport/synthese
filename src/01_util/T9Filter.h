
/** T9Filter class header.
	@file T9Filter.h

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
			if (c >= 'A' && c <= 'C' || c >= 'a' && c <= 'c' || c == 'à' || c == 'Â' || c == 'Ä' || c == 'ç' || c == 'À')
				return boost::iostreams::put(dest, '2');
			else if (c >= 'D' && c <= 'F' || c >= 'd' && c <= 'f' || c == 'é' || c == 'ê' || c == 'è' || c == 'Ê' || c == 'Ë' || c == 'È')
				return boost::iostreams::put(dest, '3');
			else if (c >= 'G' && c <= 'I' || c >= 'g' && c <= 'i' || c == 'î' || c == 'ï' || c == 'Î' || c == 'Ï' || c == 'ì' || c == 'Ì')
				return boost::iostreams::put(dest, '4');
			else if (c >= 'J' && c <= 'L' || c >= 'j' && c <= 'l')
				return boost::iostreams::put(dest, '5');
			else if (c >= 'M' && c <= 'O' || c >= 'm' && c <= 'o' || c == 'ñ' || c == 'Ñ' || c == 'ô' || c == 'ö' || c == 'õ' || c == 'Ô' || c == 'Ö' || c == 'Õ' || c == 'ò' || c == 'Ò')
				return boost::iostreams::put(dest, '6');
			else if (c >= 'P' && c <= 'S' || c >= 'p' && c <= 's')
				return boost::iostreams::put(dest, '7');
			else if (c >= 'T' && c <= 'V' || c >= 't' && c <= 'v' || c == 'ù' || c == 'Ù' || c == 'û' || c == 'Û' || c == 'ü' || c == 'Ü')
				return boost::iostreams::put(dest, '8');
			else if (c >= 'W' && c <= 'Z' || c >= 'w' && c <= 'z')
				return boost::iostreams::put(dest, '7');
			else if (c >= '0' && c <= '9')
				return boost::iostreams::put(dest, c);
			else
				return boost::iostreams::put(dest, '0');
		}
	}
}
#endif

