#include "Conversion.h"

#include <sstream>
#include <boost/algorithm/string.hpp>


namespace synthese
{
namespace util
{



bool 
Conversion::ToBool (const std::string& s)
{
    std::string str = boost::to_lower_copy (s);
    boost::trim (str);
    if ((str == "true") || (str == "yes") || (str == "1")) return true;
    return false;
}


int 
Conversion::ToInt (const std::string& s)
{
    return atoi (s.c_str ());
}



long long 
Conversion::ToLongLong (const std::string& s)
{
    long long i;
    sscanf(s.c_str (), INT64_FORMAT, &i);
    return i;
}



double 
Conversion::ToDouble (const std::string& s)
{
    return atof (s.c_str ());
}



std::string 
Conversion::ToString (int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str ();
}


std::string 
Conversion::ToString (long long l)
{
    std::stringstream ss;
    ss << l;
    return ss.str ();
}


std::string 
Conversion::ToString (unsigned long long l)
{
    std::stringstream ss;
    ss << l;
    return ss.str ();
}



std::string 
Conversion::ToString (unsigned int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str ();
}



std::string 
Conversion::ToString (unsigned long l)
{
    std::stringstream ss;
    ss << l;
    return ss.str ();
}


std::string 
Conversion::ToString (double d)
{
    std::stringstream ss;
    ss << d;
    return ss.str ();
}


std::string 
Conversion::ToString (const std::string& s)
{
    return s;
}



}
}
