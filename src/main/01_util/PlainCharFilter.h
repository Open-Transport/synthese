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
    while (i != n) {

	if ((s[i] & 0x80) == 0) // Is first char ASCII ?
	{
	    // Test ASCII characters
	    char c = s[i];
	    switch ( c )
	    {
	    case '(':
	    case ')':
		boost::iostreams::put(dest, ' ');
		break;
	    default:
		boost::iostreams::put(dest, s[i]);
	    }
	    i++;
	} 
	else if ( n-i >= 2 )
	{
	    // Test some special characters encoded on 2 bytes (UTF-8)
	    // Naive algorithm, but special characters to test
	    // take two bytes max...
	    int c = ((unsigned char) s[i]);
	    c = (c << 8) + (unsigned char) s[i+1];
	    
	    if ( // áàâä
		(c == 0xC3A1) || 
		(c == 0xC3A0) || 
		(c == 0xC3A2) || 
		(c == 0xC3A4)    
		)
	    { 
		boost::iostreams::put(dest, 'a') ;
	    }
	    else if ( // ÁÀÄÂ
		(c == 0xC381) || 
		(c == 0xC380) || 
		(c == 0xC382) || 
		(c == 0xC384)    
		)
	    { 
		boost::iostreams::put(dest, 'A') ;
	    }

	    else if ( // éèêë
		(c == 0xC3A9) || 
		(c == 0xC3A8) || 
		(c == 0xC3AA) || 
		(c == 0xC3AB)    
		)
	    { 
		boost::iostreams::put(dest, 'e') ;
	    } 
	    else if ( // ÉÈÊË
		(c == 0xC389) || 
		(c == 0xC388) || 
		(c == 0xC38A) || 
		(c == 0xC38B)    
		)
	    { 
		boost::iostreams::put(dest, 'E') ;
	    } 

	    else if ( // íìïî
		(c == 0xC3AD) || 
		(c == 0xC3AC) || 
		(c == 0xC3AF) || 
		(c == 0xC3AE)    
		)
	    { 
		boost::iostreams::put(dest, 'i') ;
	    } 
	    else if ( // ÍÌÏÎ
		(c == 0xC38D) || 
		(c == 0xC38C) || 
		(c == 0xC38F) || 
		(c == 0xC38E)    
		)
	    { 
		boost::iostreams::put(dest, 'I') ;
	    } 


	    else if ( // óòöô
		(c == 0xC3B3) || 
		(c == 0xC3B2) || 
		(c == 0xC3B6) || 
		(c == 0xC3B4)    
		)
	    { 
		boost::iostreams::put(dest, 'o') ;
	    } 
	    else if ( // ÓÒÖÔ
		(c == 0xC393) || 
		(c == 0xC392) || 
		(c == 0xC396) || 
		(c == 0xC394)    
		)
	    { 
		boost::iostreams::put(dest, 'O') ;
	    } 


	    else if ( // úùüû
		(c == 0xC3BA) || 
		(c == 0xC3B9) || 
		(c == 0xC3BC) || 
		(c == 0xC3BB)    
		)
	    { 
		boost::iostreams::put(dest, 'u') ;
	    } 
	    else if ( // ÚÙÜÛ
		(c == 0xC39A) || 
		(c == 0xC399) || 
		(c == 0xC39C) || 
		(c == 0xC39B)    
		)
	    { 
		boost::iostreams::put(dest, 'U') ;
	    } 

	    else if ( // ç
		(c == 0xC3A7)
		)
	    { 
		boost::iostreams::put(dest, 'c') ;
	    } 
	    else if ( // Ç
		(c == 0xC387)
		)
	    { 
		boost::iostreams::put(dest, 'C') ;
	    } 

	    else 
	    {
		boost::iostreams::put(dest, s[i]) ;
		boost::iostreams::put(dest, s[i+1]) ;
	    }
	    i++;
	    i++;
	    //return 2;
	} else {
	    boost::iostreams::put(dest, s[i]);
	    i++;
	}

	
    }
    return i;
}

	
}
}
#endif

