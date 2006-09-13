#ifndef SYNTHESE_UTIL_CONVERSION_H
#define SYNTHESE_UTIL_CONVERSION_H


#include "module.h"

#include <string>
#include <iostream>


#ifdef WIN32
#define INT64_FORMAT "%I64i"
#endif

#ifndef WIN32
#define INT64_FORMAT "%lli"
#endif


namespace synthese
{
namespace util
{

/** Service class for basic conversions.
@ingroup m01
*/
class Conversion
{
 private:

    Conversion ();
    ~Conversion ();

    static char _Buffer[128];

 public:

    /** Converts a string to a boolean value.
     *  The input string is trimmed before parsing.
     *  Accepted syntaxes for string are (case insensitive):
     *   - true/false
     *   - yes/no
     *   - 0/1
     */
    static bool ToBool (const std::string& s);

    static int ToInt (const std::string& s);
    static long ToLong (const std::string& s);
    static long long ToLongLong (const std::string& s);

    static double ToDouble (const std::string& s);

    static std::string ToString (int i);
    static std::string ToString (unsigned int i);

    static std::string ToString (long long l);
    static std::string ToString (unsigned long long l);

    static std::string ToString (unsigned long l);

    static std::string ToString (double d);
    static std::string ToString (const std::string& s);


};



}

}
#endif
