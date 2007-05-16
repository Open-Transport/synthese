
/** SqlFilter class header.
	@file SqlFilter.h

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

#ifndef SYNTHESE_UTIL_LOWERCASEFILTER_H
#define SYNTHESE_UTIL_LOWERCASEFILTER_H



#include <string>
#include <iostream>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/write.hpp>




namespace synthese
{
namespace util
{


/** Converts any forbidden character in SQL queries.
 *  
@ingroup m01
*/
class SqlFilter : public boost::iostreams::output_filter {
private:
public:
    
    SqlFilter () {}

    template<typename Sink> 
	bool put(Sink& dest, int c);


};




template<typename Sink>
bool 
SqlFilter::put(Sink& dest, int c)
{

    switch ( c )
    {
        case '\0':
	    boost::iostreams::write(dest, "\\0", 2 );
            return 2;

        case '\n':
                boost::iostreams::write (dest, "\\n", 2 );
            return 2;
        case '\r':
                boost::iostreams::write (dest, "\\r", 2 );
            return 2;
        case '\\':
                boost::iostreams::write (dest, "\\\\", 2 );
            return 2;
        case '\"':
                boost::iostreams::write (dest, "'", 1 );
            return 1;
        case '\032':
                boost::iostreams::write (dest, "\\Z;", 2 );
            return 2;
        case ';':
	    return boost::iostreams::put(dest, ',');
    }
    // Default
    return boost::iostreams::put(dest, c);

}
 
 



}
}
#endif

