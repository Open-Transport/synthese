/** RGBColor class implementation.
	@file RGBColor.cpp

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

#include "RGBColor.h"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{

		RGBColor::RGBColor (const std::string& colorName)
		{
			// If color name starts with '(' we expect
			// a triplet of colors (red, green, blue)
			if ((colorName.size() > 0) && (colorName[0] == '('))
			{
				typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
				boost::char_separator<char> sep(",()");
				tokenizer tokens(colorName, sep);

				tokenizer::iterator tok_iter = tokens.begin();
				std::string rs = *(tok_iter); ++tok_iter;
				std::string gs = *(tok_iter); ++tok_iter;
				std::string bs = *(tok_iter);

				r = lexical_cast<int>(rs);
				g = lexical_cast<int>(gs);
				b = lexical_cast<int>(bs);

			} else if (colorName == "blue") {
			r = 0; g = 0; b = 255;
			} else if (colorName == "red") {
			r = 255; g = 0; b = 0;
			} else if (colorName == "green") {
			r = 0; g = 255; b = 0;
			} else if (colorName == "yellow") {
			r = 255; g = 255; b = 0;
			} else if (colorName == "magenta") {
			r = 255; g = 0; b = 255;
			} else if (colorName == "cyan") {
			r = 0; g = 255; b = 255;
			} else if (colorName == "white") {
			r = 255; g = 255; b = 255;
			} else if (colorName == "black") {
			r = 0; g = 0; b = 0;
			}
			else
			{
				throw Exception();
			}
		}

		bool RGBColor::operator == (const RGBColor& op2 ) const
		{
			return (r == op2.r) && (g == op2.g) && (b == op2.b);
		}

		std::string RGBColor::toString() const
		{
			stringstream s;
			s << "(" << r << "," << g << "," << b << ")";
			return s.str();
		}



		std::string RGBColor::toXMLColor() const
		{
			stringstream s;
			s << "#";
			s << setw(2) << setfill('0');
			s << hex << r;
			s << setw(2) << setfill('0');
			s << hex << g;
			s << setw(2) << setfill('0');
			s << hex << b;
			return s.str();
		}



		RGBColor RGBColor::FromXMLColor( const std::string& value )
		{
			vector<int> values;
			for(size_t p(1); p<value.size() && p<6; p+=2)
			{
				string number("0x"+ value.substr(p,2));
				istringstream str(number);
				int v;
				str >> hex >> v;
				values.push_back(v);
			}
			if(values.size() != 3)
			{
				throw RGBColor::Exception();
			}
			return RGBColor(values[0], values[1], values[2]);
		}



		RGBColor::Exception::Exception()
			: synthese::Exception("No such color")
		{

		}
	}
}
