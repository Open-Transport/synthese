
/** Conversion class implementation.
	@file Conversion.cpp

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

#include "Conversion.h"

#include "01_util/Constants.h"

#include <complex>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{
		bool Conversion::ToBool (const std::string& s)
		{
			string str(to_lower_copy(s));
			trim(str);
			if((ToInt(str) > 0) || (str == "true") || (str == "yes")) return true;
			return false;
		}


		    

		boost::logic::tribool 
		Conversion::ToTribool (const std::string& s)
		{
			std::string str = boost::to_lower_copy (s);
			boost::trim (str);
			if ((str == "true") || (str == "yes") || (ToInt(str)>0)) return true;
			if ((str == "false") || (str == "no") || (str == "0")) return false;
			return boost::logic::indeterminate;
		    
		}



		int 
		Conversion::ToInt (const std::string& s)
		{
			return atoi (s.c_str ());
		}


		long 
		Conversion::ToLong (const std::string& s)
		{
			return atol (s.c_str ());
		}



		long long 
		Conversion::ToLongLong (const std::string& s)
		{
			long long i = 0;
			sscanf(s.c_str (), INT64_FORMAT, &i);
			return i;
		}



		double 
		Conversion::ToDouble (const std::string& s)
		{
			return atof (s.c_str ());
		}



		std::string 
		Conversion::ToString (int i)
		{
			std::stringstream ss;
			ss << i;
			return ss.str ();
		}


		std::string 
		Conversion::ToString (long long l)
		{
			std::stringstream ss;
			ss << l;
			return ss.str ();
		}


		std::string 
		Conversion::ToString (unsigned long long l)
		{
			std::stringstream ss;
			ss << l;
			return ss.str ();
		}



		std::string 
		Conversion::ToString (unsigned int i)
		{
			std::stringstream ss;
			ss << i;
			return ss.str ();
		}



		std::string 
		Conversion::ToString (long l)
		{
			std::stringstream ss;
			ss << l;
			return ss.str ();
		}

		std::string 
		Conversion::ToOctalString (long l)
		{
			std::stringstream ss;
			ss << std::oct;
			ss << l;
			return ss.str ();
		}

		std::string 
		Conversion::ToString (unsigned long l)
		{
			std::stringstream ss;
			ss << l;
			return ss.str ();
		}


		std::string 
		Conversion::ToString (double d)
		{
			std::stringstream ss;
			ss << std::fixed << d;
			return ss.str ();
		}


		std::string 
		Conversion::ToString (const std::string& s)
		{
			return s;
		}

		std::string Conversion::ToString( boost::logic::tribool t )
		{
			return (t == true) ? "1" : "0";
		}

		std::string Conversion::ToSQLiteString( const std::string& s, bool withApostrophes)
		{
			std::string result;
			if (withApostrophes)
				result.push_back('\'');
			for (size_t i=0; i<s.size(); ++i)
			{
				// Escape several characters
				if (s.at(i) == '\'' || s.at(i) == '\'')
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



	    std::string Conversion::ToPrepaddedString (const std::string& s, char padchar, int paddedsize) 
	    {
		assert (s.length () <= paddedsize);
		std::stringstream ss;
		for (size_t i=0; i<(paddedsize - s.length ()); ++i) ss << padchar;
		ss << s;
		return ss.str ();
	    }


	    std::string Conversion::ToPostpaddedString (const std::string& s, char padchar, int paddedsize) 
	    {
		assert (s.length () <= paddedsize);
		std::stringstream ss;
		ss << s;
		for (size_t i=0; i<(paddedsize - s.length ()); ++i) ss << padchar;
		return ss.str ();
	    }


	    std::string Conversion::ToTruncatedString (const std::string& s, int size)
	    {
		return (s.length () <= size ? s : (s.substr (0, size) + "  (...)"));
	    }

		std::string Conversion::ToFixedSizeString( int number, int size )
		{
			if (size < 1)
				return ToString(number);
			stringstream s;
			for (; size; size--)
				if (number < pow(10.0, size - 1))
					s << "0";
			s << number;
			return s.str();
		}
	}
}
