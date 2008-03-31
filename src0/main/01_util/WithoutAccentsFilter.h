
/** WithoutAccentsFilter class header.
	@file WithoutAccentsFilter.h

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

#ifndef SYNTHESE_UTIL_WITHOUTACCTENSFILTER_H
#define SYNTHESE_UTIL_WITHOUTACCTENSFILTER_H

#include <string>
#include <iostream>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/write.hpp>


namespace synthese
{
	namespace util
	{
		/** Converts any accentuated character into the corresponding non accentuated one.
			Any numerical character is preserved. All other characters are converted to space.
			@ingroup m01
		*/
		class WithoutAccentsFilter : public boost::iostreams::output_filter
		{
		private:
		public:

			WithoutAccentsFilter() {}

			template<typename Sink> 
			bool put(Sink& dest, int c);


		};


		template<typename Sink>
		bool 
			WithoutAccentsFilter::put(Sink& dest, int c)
		{
			switch (c)
			{
			case 'à':
			case 'â':
			case 'ä':
				return boost::iostreams::put(dest, 'a');
				break;

			case 'é':
			case 'è':
			case 'ê':
			case 'ë':
				return boost::iostreams::put(dest, 'e');
				break;

			case 'ì':
			case 'î':
			case 'ï':
				return boost::iostreams::put(dest, 'i');
				break;

			case 'ò':
			case 'ô':
			case 'ö':
				return boost::iostreams::put(dest, 'o');
				break;

			case 'ù':
			case 'û':
			case 'ü':
				return boost::iostreams::put(dest, 'u');
				break;
			}
			if ( c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c >= 'a' && c <= 'z' )
			{
				return boost::iostreams::put(dest, c);
			}
			else 
			{
				return boost::iostreams::put(dest, ' ');
			}
		}
	}
}

#endif
