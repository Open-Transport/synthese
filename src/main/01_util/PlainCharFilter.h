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

