
/** Conversion class implementation.
	@file Conversion.cpp

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

#include "Conversion.h"

#include "UtilConstants.h"

#include <complex>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{
		boost::logic::tribool
		Conversion::ToTribool (const std::string& s)
		{
			std::string str = boost::to_lower_copy (s);
			boost::trim (str);
			if(	str == "false" ||
				str == "no" ||
				str == "0"
			){
				return false;
			}
			try
			{
				if(	str == "true" ||
					str == "yes" ||
					(!str.empty() && lexical_cast<int>(str)>0)
				){
					return true;
				}
			}
			catch(bad_lexical_cast&)
			{
			}
			return boost::logic::indeterminate;
		}



		std::string	Conversion::ToOctalString (long l)
		{
			std::stringstream ss;
			ss << std::oct;
			ss << l;
			return ss.str ();
		}



		std::string Conversion::ToDBString( const std::string& s, bool withApostrophes)
		{
			std::string result;
			if (withApostrophes)
				result.push_back('\'');
			for (size_t i=0; i<s.size(); ++i)
			{
				// Escape single quotes
				if (s.at(i) == '\'')
					result.push_back('\'');

				result.push_back(s.at(i));
			}
			if (withApostrophes)
				result.push_back('\'');
			return result;
		}



		std::vector<std::string> Conversion::ToStringVector(const std::string& text)
		{
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep1 (",");
			tokenizer tokens (text, sep1);
			std::vector<std::string> result;

			for (tokenizer::iterator it = tokens.begin(); it != tokens.end (); ++it)
			{
				result.push_back(*it);
			}
			return result;

		}



	    std::string Conversion::ToPrepaddedString (const std::string& s, char padchar, size_t paddedsize)
	    {
			assert (s.length () <= paddedsize);
			std::stringstream ss;
			for (size_t i=0; i<(paddedsize - s.length ()); ++i) ss << padchar;
			ss << s;
			return ss.str ();
	    }



	    std::string Conversion::ToPostpaddedString (const std::string& s, char padchar, size_t paddedsize)
	    {
			assert (s.length () <= paddedsize);
			std::stringstream ss;
			ss << s;
			for (size_t i=0; i<(paddedsize - s.length ()); ++i) ss << padchar;
			return ss.str ();
	    }


	    std::string Conversion::ToTruncatedString (const std::string& s, size_t size)
	    {
			return (s.length () <= size ? s : (s.substr (0, size) + "  (...)"));
	    }



		std::string Conversion::ToFixedSizeString( int number, size_t size )
		{
			if (size < 1)
				return lexical_cast<string>(number);
			stringstream s;
			for (; size; size--)
				if (number < pow(10.0, long(size) - 1))
					s << "0";
			s << number;
			return s.str();
		}



		std::string Conversion::ToXMLAttrString(const std::string& inputString)
		{
			static map<char, string> encodingTable;
			if(encodingTable.empty())
			{
				encodingTable['&'] = "&amp;";
				encodingTable['>'] = "&gt;";
				encodingTable['<'] = "&lt;";
				encodingTable['"'] = "&quot;";
				encodingTable['\''] = "&apos;";
			}
			stringstream ss;
			BOOST_FOREACH(char c, inputString)
			{
				map<char, string>::const_iterator it(encodingTable.find(c));
				if(it != encodingTable.end())
				{
					ss << it->second;
					continue;
				}

				ss << c;
			}
			return ss.str();
		}
}	}
