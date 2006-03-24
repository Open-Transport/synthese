#ifndef SYNTHESE_UTIL_LOWERCASEFILTER_H
#define SYNTHESE_UTIL_LOWERCASEFILTER_H


#include "module.h"

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
