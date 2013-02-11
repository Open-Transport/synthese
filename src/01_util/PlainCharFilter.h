
/** PlainCharFilter class header.
	@file PlainCharFilter.h

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

#ifndef SYNTHESE_UTIL_PLAINCHARFILTER_H
#define SYNTHESE_UTIL_PLAINCHARFILTER_H

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
		/** Filters any stressed or special alphabetical character, which are converted
			to their closest equivalent in A-Za-z.
			Input stream must be ISO8859-1

			@ingroup m01
		*/
		class PlainCharFilter:
		public boost::iostreams::multichar_output_filter
		{
		public:

			PlainCharFilter() {}

			template<typename Sink>
			std::streamsize write(Sink& dest, const char* s, std::streamsize n);
		};



		template<typename Sink>
		std::streamsize PlainCharFilter::write(Sink& dest, const char* s, std::streamsize n)
		{
			int i = 0;

			while (i != n)
			{
				unsigned char c = s[i];

				if (c == 0xc3)
				{
					i++;
					if(i == n)
					{
						boost::iostreams::put(dest, c) ;
						break;
					}
					c = s[i];
					switch(c)
					{
						case 0xa8 : //'è' = c3a8
						case 0xa9 : //'é' = c3a9
						case 0xaa : //'ê' = c3aa
						case 0xab : //'ë' = c3ab
							boost::iostreams::put(dest, 'e');
							break;
						case 0xa7 : //'ç' = c3a7
							boost::iostreams::put(dest, 'c');
							break;
						case 0xa0 : //'à' = c3a0á
						case 0xa1 : //'á' = c3a1
						case 0xa2 : //'â' = c3a2
						case 0xa4 : //'ä' = c3a4
							boost::iostreams::put(dest, 'a');
							break;
						case 0xb9 : //'ù' = c3b9
						case 0xba : //'ú' = c3ba
						case 0xbb : //'û' = c3bb
						case 0xbc : //'ü' = c3bc
							boost::iostreams::put(dest, 'u');
							break;
						case 0xb4 : //'ô' = c3b4
						case 0xb6 : //'ö' = c3b6
							boost::iostreams::put(dest, 'o');
							break;
						case 0xae : //'î' = c3ae
						case 0xaf : //'ï' = c3af
							boost::iostreams::put(dest, 'i');
							break;
						default : //Put last two chars
							boost::iostreams::put(dest, s[i-1]) ;
							boost::iostreams::put(dest, c);
							break;
					}
				}
				else if (c == 0xc2 )
				{
					i++;
					if(i == n)
					{
						boost::iostreams::put(dest, c) ;
						break;
					}
					c = s[i];
					switch((unsigned int)c)
					{
						case 0xb0 : //'°' = c2b0
							boost::iostreams::put(dest, 'o');
							break;
						default ://Put last two chars
							boost::iostreams::put(dest, s[i-1]) ;
							boost::iostreams::put(dest, c);
							break;
					}
				}
				else if (c == 0xc5 )
				{
					i++;
					if(i == n)
					{
						boost::iostreams::put(dest, c) ;
						break;
					}
					c = s[i];
					switch(c)
					{
						case 0x93 : //'œ' = c593
							boost::iostreams::put(dest, 'o');
							boost::iostreams::put(dest, 'e');
							break;
						default ://Put last two chars
							boost::iostreams::put(dest, s[i-1]) ;
							boost::iostreams::put(dest, c);
							break;
					}
				}
				else
				{
					boost::iostreams::put(dest, c) ;
				}

				++i;
			}

			return i;
		}
	}
}

#endif
