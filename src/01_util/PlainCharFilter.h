
/** PlainCharFilter class header.
	@file PlainCharFilter.h

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
			Note that this filter is written given the encoding of this source file,
			which is expected to be ISO8859-15.

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
				char c = s[i];
					
				if ( 
					(c == 'á') || 
					(c == 'à') || 
					(c == 'â') || 
					(c == 'ä')    
					)
				{ 
					boost::iostreams::put(dest, 'a') ;
				}
				else if ( 
					(c == 'Á') || 
					(c == 'À') || 
					(c == 'Ä') || 
					(c == 'Â')    
					)
				{ 
					boost::iostreams::put(dest, 'A') ;
				}
				
				else if ( 
					(c == 'é') || 
					(c == 'è') || 
					(c == 'ê') || 
					(c == 'ë')    
					)
				{ 
					boost::iostreams::put(dest, 'e') ;
				} 
				else if ( 
					(c == 'É') || 
					(c == 'È') || 
					(c == 'Ê') || 
					(c == 'Ë')    
					)
				{ 
					boost::iostreams::put(dest, 'E') ;
				} 
				
				else if (
					(c == 'í') || 
					(c == 'ì') || 
					(c == 'ï') || 
					(c == 'î')    
					)
				{ 
					boost::iostreams::put(dest, 'i') ;
				} 
				else if (
					(c == 'Í') || 
					(c == 'Ì') || 
					(c == 'Ï') || 
					(c == 'Î')    
					)
				{ 
					boost::iostreams::put(dest, 'I') ;
				} 
				
			    
				else if (
					(c == 'ó') || 
					(c == 'ò') || 
					(c == 'ö') || 
					(c == 'ô')    
					)
				{ 
					boost::iostreams::put(dest, 'o') ;
				} 
				else if (
					(c == 'Ó') || 
					(c == 'Ò') || 
					(c == 'Ö') || 
					(c == 'Ô')    
					)
				{ 
					boost::iostreams::put(dest, 'O') ;
				} 
			    
			    
				else if (
					(c == 'ú') || 
					(c == 'ù') || 
					(c == 'ü') || 
					(c == 'û')    
					)
				{ 
					boost::iostreams::put(dest, 'u') ;
				} 
				else if ( 
					(c == 'Ú') || 
					(c == 'Ù') || 
					(c == 'Ü') || 
					(c == 'Û')    
					)
				{ 
					boost::iostreams::put(dest, 'U') ;
				} 
			    
				else if ( 
					(c == 'ç')
					)
				{ 
					boost::iostreams::put(dest, 'c') ;
				} 
				else if (
					(c == 'Ç')
					)
				{ 
					boost::iostreams::put(dest, 'C') ;
				}
				else if (
					(c == '°')
				){
					boost::iostreams::put(dest, 'o');
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
