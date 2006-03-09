#include "Conversion.h"

#include <sstream>


namespace synthese
{
namespace util
{


int 
Conversion::ToInt (std::string s)
{
    return atoi (s.c_str ());
}


std::string 
Conversion::ToString (int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str ();
}





}
}
