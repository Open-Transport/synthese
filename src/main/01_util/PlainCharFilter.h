#ifndef SYNTHESE_UTIL_PLAINCHARFILTER_H
#define SYNTHESE_UTIL_PLAINCHARFILTER_H

#include <string>
#include <iostream>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/write.hpp>




namespace synthese
{
namespace util
{


/** Filters any stressed or special alphabetical character, whiwh are converted
    to their closest equivalent in A-Za-z.
@ingroup m01
*/
class PlainCharFilter : public boost::iostreams::multichar_output_filter {
private:
public:
    
    PlainCharFilter () {}

    /** @todo Complete filtering implementation for other special characters
     */
    template<typename Sink> 
	std::streamsize write(Sink& dest, const char* s, std::streamsize n);

};




template<typename Sink>
std::streamsize 
PlainCharFilter::write(Sink& dest, const char* s, std::streamsize n)
{
    int i = 0;
    while (i != n)
    {
	char c = s[i];
	
	if ( 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'a') ;
	}
	else if ( 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'A') ;
	}
	
	else if ( 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'e') ;
	} 
	else if ( 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'E') ;
	} 
	
	else if (
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'i') ;
	} 
	else if (
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'I') ;
	} 
	
    
	else if (
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'o') ;
	} 
	else if (
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'O') ;
	} 
    
    
	else if (
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'u') ;
	} 
	else if ( 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�') || 
	    (c == '�')    
	    )
	{ 
	    boost::iostreams::put(dest, 'U') ;
	} 
    
	else if ( 
	    (c == '�')
	    )
	{ 
	    boost::iostreams::put(dest, 'c') ;
	} 
	else if (
	    (c == '�')
	    )
	{ 
	    boost::iostreams::put(dest, 'C') ;
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

