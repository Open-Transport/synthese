#ifndef SYNTHESE_UTIL_CONVERSION_H
#define SYNTHESE_UTIL_CONVERSION_H


#include "module.h"

#include <string>
#include <iostream>



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

    static double ToDouble (const std::string& s);

    static std::string ToString (int i);
    static std::string ToString (unsigned int i);

    static std::string ToString (long l);
    static std::string ToString (unsigned long l);


};



}

}
#endif
