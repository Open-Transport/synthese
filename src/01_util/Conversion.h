////////////////////////////////////////////////////////////////////////////////
/// Conversion class header.
///	@file Conversion.h
///	@author Marc Jambert
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

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
			//lint --e{1704}
		private:

			Conversion ();
			~Conversion ();

		public:

			static boost::logic::tribool ToTribool (const std::string& s);

			static std::string ToPrepaddedString (const std::string& s, char padchar, std::size_t paddedsize);
			static std::string ToPostpaddedString (const std::string& s, char padchar, std::size_t paddedsize);

			static std::string ToTruncatedString (const std::string& s, std::size_t size = 255);

			static std::string ToOctalString (long l);

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
			static std::string ToDBString (const std::string& s, bool withApostrophes=true);

			static std::string ToFixedSizeString(int number, size_t size);

			/** Converts string to a string that can be used as a XML attribute.
				@param inputString String to convert
				@return std::string Converted string
				@author Sylvain Pasche
				@date 2011
				@since 3.3.0
			*/
			static std::string ToXMLAttrString(const std::string& inputString);
		};
	}
}
#endif
