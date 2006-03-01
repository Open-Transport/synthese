#ifndef SYNTHESE_UTIL_LOWERCASEFILTER_H
#define SYNTHESE_UTIL_LOWERCASEFILTER_H


#include "module.h"

#include <string>
#include <iostream>

#include <boost/iostreams/concepts.hpp>




namespace synthese
{
namespace util
{


/** 
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
    else 
    {
	return boost::iostreams::put(dest, c);
    }
}
 
 



}
}
#endif
