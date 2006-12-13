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


    

boost::logic::tribool 
Conversion::ToTribool (const std::string& s)
{
    std::string str = boost::to_lower_copy (s);
    boost::trim (str);
    if ((str == "true") || (str == "yes") || (str == "1")) return true;
    if ((str == "false") || (str == "no") || (str == "0")) return false;
    return boost::logic::indeterminate;
    
}



int 
Conversion::ToInt (const std::string& s)
{
    return atoi (s.c_str ());
}


long 
Conversion::ToLong (const std::string& s)
{
    return atol (s.c_str ());
}



long long 
Conversion::ToLongLong (const std::string& s)
{
    long long i = 0;
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

std::string Conversion::ToString( boost::logic::tribool t )
{
	return (t == true) ? "1" : "0";
}

std::string Conversion::ToSQLiteString( const std::string& s, bool withApostrophes)
{
	std::string result;
	if (withApostrophes)
		result.push_back('\'');
	for (size_t i=0; i<s.size(); ++i)
	{
		// Escape several characters
		if (s.at(i) == '\'' || s.at(i) == '\'')
			result.push_back('\\');
		
		result.push_back(s.at(i));
	}
	if (withApostrophes)
		result.push_back('\'');
	return result;
}
}
}

