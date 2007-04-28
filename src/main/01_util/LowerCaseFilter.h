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


/** Converts any upper case letter going through this filter to lower case.
Any numerical character is preserved. All other characters are converted to space.
 *  
@ingroup m01
*/
class LowerCaseFilter : public boost::iostreams::output_filter {
private:
public:
    
    LowerCaseFilter () {}

    template<typename Sink> 
	bool put(Sink& dest, int c);


};




template<typename Sink>
bool 
LowerCaseFilter::put(Sink& dest, int c)
{
    if ( c >= 'A' && c <= 'Z' )
    {
	return boost::iostreams::put(dest, c - 'A' + 'a');
    }
    else if ( c >= '0' && c <= '9' || c >= 'a' && c <= 'z' )
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

