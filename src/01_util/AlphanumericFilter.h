
/** AlphanumericFilter class header.
	@file AlphanumericFilter.h

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

#ifndef SYNTHESE_util_AlphanumericFilter_h__
#define SYNTHESE_util_AlphanumericFilter_h__

#include <string>
#include <iostream>
#include <sstream>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/write.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace synthese
{
	namespace util
	{
		/** AlphanumericFilter class.
			@ingroup m01

			Filters any non alphanumeric character, which are replaced by spaces.
			Notes :
				- accentuated characters are kept (use PlainCharFilter to remove them).
				- note that this filter is written given the encoding of this source file,
				  which is expected to be ISO8859-15.
		*/
		class AlphanumericFilter:
		public boost::iostreams::multichar_output_filter
		{
		public:

			AlphanumericFilter() {}

			template<typename Sink>
			std::streamsize write(Sink& dest, const char* s, std::streamsize n);
		};



		template<typename Sink>
		std::streamsize AlphanumericFilter::write(
			Sink& dest,
			const char* s,
			std::streamsize n
		){
			int i = 0;
			while (i != n)
			{
				char c = s[i];

				if( c == 'á' ||
					c == 'à' ||
					c == 'â' ||
					c == 'ä' ||
					c == 'Á' ||
					c == 'À' ||
					c == 'Ä' ||
					c == 'Â' ||
					c == 'é' ||
					c == 'è' ||
					c == 'ê' ||
					c == 'ë' ||
					c == 'É' ||
					c == 'È' ||
					c == 'Ê' ||
					c == 'Ë' ||
					c == 'í' ||
					c == 'ì' ||
					c == 'ï' ||
					c == 'î' ||
					c == 'Í' ||
					c == 'Ì' ||
					c == 'Ï' ||
					c == 'Î' ||
					c == 'ó' ||
					c == 'ò' ||
					c == 'ö' ||
					c == 'ô' ||
					c == 'Ó' ||
					c == 'Ò' ||
					c == 'Ö' ||
					c == 'Ô' ||
					c == 'ú' ||
					c == 'ù' ||
					c == 'ü' ||
					c == 'û' ||
					c == 'Ú' ||
					c == 'Ù' ||
					c == 'Ü' ||
					c == 'Û' ||
					c == 'ç' ||
					c == 'Ç' ||
					c == 'Œ' ||
					c == 'œ' ||
					c == 'Æ' ||
					c == 'æ' ||
					(c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					(c >= '0' && c <= '9')
				){
					boost::iostreams::put(dest, c);
				}
				else
				{
					boost::iostreams::put(dest, ' ') ;
				}
				++i;
			}

			return i;

		}
	}
}

#endif // SYNTHESE_util_AlphanumericFilter_h__
