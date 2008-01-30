
/** Conversion class header.
	@file Conversion.h

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

#ifndef SYNTHESE_UTIL_CONVERSION_H
#define SYNTHESE_UTIL_CONVERSION_H

#include <string>
#include <iostream>
#include <vector>

#include <boost/logic/tribool.hpp>


#ifdef WIN32
#define INT64_FORMAT "%I64i"
#endif

#ifndef WIN32
#define INT64_FORMAT "%lli"
#endif


namespace synthese
{
	namespace util
	{

		/** Service class for basic conversions.
		@ingroup m01
		*/
		class Conversion
		{
		private:

			Conversion ();
			~Conversion ();

		public:

			/** Converts a string to a boolean value.
			*  The input string is trimmed before parsing.
			*  Accepted syntaxes for string are (case insensitive):
			*   - true/false
			*   - yes/no
			*   - 0/1
			*/
			static bool ToBool (const std::string& s);

			static boost::logic::tribool ToTribool (const std::string& s);

			static int ToInt (const std::string& s);
			static long ToLong (const std::string& s);
			static long long ToLongLong (const std::string& s);

			static double ToDouble (const std::string& s);

			static std::string ToPrepaddedString (const std::string& s, char padchar, int paddedsize);
			static std::string ToPostpaddedString (const std::string& s, char padchar, int paddedsize);

			static std::string ToTruncatedString (const std::string& s, int size = 255);

			static std::string ToString (int i);
			static std::string ToString (unsigned int i);

			static std::string ToString (long long l);
			static std::string ToString (unsigned long long l);

			static std::string ToString (long l);

			static std::string ToString (unsigned long l);

			static std::string ToString (double d);
			static std::string ToString (const std::string& s);

			static std::string ToOctalString (long l);

			static std::string ToString (boost::logic::tribool t);

			static std::vector<std::string> ToStringVector(const std::string& text);


			/** Converts string to SQL constant string.
				@param s String to convert
				@return std::string Converted string
				@author Hugues
				@date 2006
				
				The conversion consists in :
					- escape some characters
					- add apostrophes at the beginning and at the end of the string
			*/
			static std::string ToSQLiteString (const std::string& s, bool withApostrophes=true);

			static std::string ToFixedSizeString(int number, int size);

		};
	}
}
#endif
